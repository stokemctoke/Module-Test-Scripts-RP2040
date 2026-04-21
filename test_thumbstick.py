"""
Test script: 8-pin Thumbstick Module
5-direction joystick (UP/DOWN/LEFT/RIGHT/CENTER press) + 2 extra buttons

Typical 8-pin module pinout (adjust GP numbers to your wiring):
  Pin 1 (UP)    -> GP6
  Pin 2 (DOWN)  -> GP7
  Pin 3 (LEFT)  -> GP8
  Pin 4 (RIGHT) -> GP9
  Pin 5 (CENTER/SEL) -> GP10   joystick center press
  Pin 6 (BTN_A) -> GP11        extra button A
  Pin 7 (BTN_B) -> GP12        extra button B
  Pin 8 (GND)   -> GND
  VCC (if present) -> 3.3V

All inputs are active-LOW with internal pull-ups enabled.
Adjust GP numbers in PIN_MAP below to match your actual wiring.
"""

import machine
import time

PIN_MAP = {
    "UP":     6,
    "DOWN":   7,
    "LEFT":   8,
    "RIGHT":  9,
    "CENTER": 10,
    "BTN_A":  11,
    "BTN_B":  12,
}

DEBOUNCE_MS  = 50
POLL_INTERVAL_MS = 10
TEST_DURATION_S  = 30   # how long to run the interactive test

def init_pins():
    pins = {}
    for name, gp in PIN_MAP.items():
        pins[name] = machine.Pin(gp, machine.Pin.IN, machine.Pin.PULL_UP)
    return pins

def read_state(pins):
    # Returns dict of name -> True if pressed (active-LOW)
    return {name: not pin.value() for name, pin in pins.items()}

def wait_for_input(pins, timeout_ms=5000):
    deadline = time.ticks_add(time.ticks_ms(), timeout_ms)
    while time.ticks_diff(deadline, time.ticks_ms()) > 0:
        state = read_state(pins)
        if any(state.values()):
            return state
        time.sleep_ms(POLL_INTERVAL_MS)
    return None

def test_individual_inputs(pins):
    """Ask the tester to press each input in sequence and verify detection."""
    print("\nTest 1: individual input detection")
    order = ["UP", "DOWN", "LEFT", "RIGHT", "CENTER", "BTN_A", "BTN_B"]
    passed = []
    failed = []

    for target in order:
        print(f"  Press {target} within 5 seconds...")
        # Wait for release first in case something is held
        while read_state(pins)[target]:
            time.sleep_ms(POLL_INTERVAL_MS)

        state = wait_for_input(pins, timeout_ms=5000)
        if state is None:
            print(f"  TIMEOUT — {target} not detected")
            failed.append(target)
            continue

        detected = [k for k, v in state.items() if v]
        if target in detected:
            print(f"  OK — {target} detected (also active: {[d for d in detected if d != target]})")
            passed.append(target)
        else:
            print(f"  FAIL — expected {target}, got {detected}")
            failed.append(target)

        # Wait for release before next input
        timeout = time.ticks_add(time.ticks_ms(), 2000)
        while any(read_state(pins).values()):
            if time.ticks_diff(timeout, time.ticks_ms()) <= 0:
                break
            time.sleep_ms(POLL_INTERVAL_MS)
        time.sleep_ms(200)

    print(f"\n  Passed: {passed}")
    print(f"  Failed: {failed}")
    return len(failed) == 0

def test_no_ghosting(pins):
    """Verify all inputs read released when nothing is pressed."""
    print("\nTest 2: idle state (all released) — release everything and wait 1s")
    time.sleep(1)
    state = read_state(pins)
    active = [k for k, v in state.items() if v]
    if active:
        print(f"  FAIL — inputs still active when nothing pressed: {active}")
        return False
    print("  OK — all inputs idle")
    return True

def test_simultaneous(pins):
    """Check that holding BTN_A and BTN_B together both register."""
    print("\nTest 3: simultaneous BTN_A + BTN_B — hold both for 2 seconds")
    print("  Press and hold BTN_A and BTN_B now...")
    time.sleep(2)
    state = read_state(pins)
    a_ok = state["BTN_A"]
    b_ok = state["BTN_B"]
    if a_ok and b_ok:
        print("  OK — both BTN_A and BTN_B detected simultaneously")
        return True
    else:
        print(f"  FAIL — BTN_A={a_ok} BTN_B={b_ok}")
        return False

def test_live_monitor(pins, duration_s=TEST_DURATION_S):
    """Live readout — press anything to see it reflected in the terminal."""
    print(f"\nTest 4: live monitor for {duration_s}s — move the stick and press buttons")
    last_state = {k: False for k in PIN_MAP}
    deadline = time.ticks_add(time.ticks_ms(), duration_s * 1000)

    while time.ticks_diff(deadline, time.ticks_ms()) > 0:
        state = read_state(pins)
        active = [k for k, v in state.items() if v]

        # Only print on change
        if state != last_state:
            if active:
                print(f"  ACTIVE: {active}")
            else:
                print("  (released)")
            last_state = dict(state)

        time.sleep_ms(DEBOUNCE_MS)

    print("  Live monitor ended")

def run():
    print("--- Thumbstick Module Test ---")
    print(f"Pin map: {PIN_MAP}")
    pins = init_pins()

    idle_ok  = test_no_ghosting(pins)
    indiv_ok = test_individual_inputs(pins)
    simul_ok = test_simultaneous(pins)
    test_live_monitor(pins)

    print("\n--- Results ---")
    print(f"  Idle state:    {'PASS' if idle_ok  else 'FAIL'}")
    print(f"  Individual:    {'PASS' if indiv_ok else 'FAIL'}")
    print(f"  Simultaneous:  {'PASS' if simul_ok else 'FAIL'}")
    overall = idle_ok and indiv_ok and simul_ok
    print(f"\n{'ALL TESTS PASSED' if overall else 'SOME TESTS FAILED'}")

run()
