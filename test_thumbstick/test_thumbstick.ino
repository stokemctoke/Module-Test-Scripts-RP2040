/*
  Test sketch: 8-pin Thumbstick Module
  5-direction joystick (UP/DOWN/LEFT/RIGHT/MID) + SET and RESET buttons
  Board: Waveshare RP2040 Zero

  Wiring (adjust pin numbers to match your setup):
    Pin 1 (COM)   -> GND
    Pin 2 (UP)    -> GP6
    Pin 3 (DOWN)  -> GP7
    Pin 4 (LEFT)  -> GP8
    Pin 5 (RIGHT) -> GP9
    Pin 6 (MID)   -> GP10
    Pin 7 (SET)   -> GP11
    Pin 8 (RESET) -> GP12

  All inputs are active-LOW; internal pull-ups are enabled.
  No extra library required.
*/

#define PIN_UP     6
#define PIN_DOWN   7
#define PIN_LEFT   8
#define PIN_RIGHT  9
#define PIN_MID   10
#define PIN_SET   11
#define PIN_RESET 12

#define DEBOUNCE_MS     50
#define PROMPT_TIMEOUT 5000   // ms to wait for each input during guided test
#define MONITOR_SECS   30     // duration of live monitor at end

struct Input {
  const char* name;
  uint8_t     pin;
};

const Input INPUTS[] = {
  { "UP",    PIN_UP    },
  { "DOWN",  PIN_DOWN  },
  { "LEFT",  PIN_LEFT  },
  { "RIGHT", PIN_RIGHT },
  { "MID",   PIN_MID   },
  { "SET",   PIN_SET   },
  { "RESET", PIN_RESET },
};
const int INPUT_COUNT = sizeof(INPUTS) / sizeof(INPUTS[0]);

bool isPressed(const Input& in) {
  return digitalRead(in.pin) == LOW;   // active-LOW
}

bool anyPressed() {
  for (int i = 0; i < INPUT_COUNT; i++) {
    if (isPressed(INPUTS[i])) return true;
  }
  return false;
}

// ----- Test 1: idle state -----
bool testIdle() {
  Serial.println("\nTest 1: idle state — release everything, waiting 1s...");
  delay(1000);
  bool fail = false;
  for (int i = 0; i < INPUT_COUNT; i++) {
    if (isPressed(INPUTS[i])) {
      Serial.print("  FAIL: ");
      Serial.print(INPUTS[i].name);
      Serial.println(" reads active when nothing should be pressed");
      fail = true;
    }
  }
  if (!fail) Serial.println("  OK: all inputs idle");
  return !fail;
}

// ----- Test 2: individual detection -----
bool testIndividual() {
  Serial.println("\nTest 2: individual input detection");
  int passed = 0;
  int failed = 0;

  for (int i = 0; i < INPUT_COUNT; i++) {
    const Input& target = INPUTS[i];

    Serial.print("  Press ");
    Serial.print(target.name);
    Serial.println(" within 5 seconds...");

    // Wait for any previous press to release
    unsigned long releaseDeadline = millis() + 2000;
    while (isPressed(target) && millis() < releaseDeadline) delay(10);

    // Wait for a press
    unsigned long deadline = millis() + PROMPT_TIMEOUT;
    bool detected = false;
    while (millis() < deadline) {
      if (isPressed(target)) { detected = true; break; }
      delay(10);
    }

    if (!detected) {
      Serial.print("  TIMEOUT — ");
      Serial.print(target.name);
      Serial.println(" not detected");
      failed++;
    } else {
      Serial.print("  OK — ");
      Serial.println(target.name);
      passed++;
    }

    // Wait for release before next
    unsigned long relDeadline = millis() + 2000;
    while (anyPressed() && millis() < relDeadline) delay(10);
    delay(200);
  }

  Serial.print("  Passed: "); Serial.print(passed);
  Serial.print("  Failed: "); Serial.println(failed);
  return failed == 0;
}

// ----- Test 3: simultaneous buttons -----
bool testSimultaneous() {
  Serial.println("\nTest 3: hold SET + RESET together for 2 seconds...");
  delay(2000);
  bool setOk   = (digitalRead(PIN_SET)   == LOW);
  bool resetOk = (digitalRead(PIN_RESET) == LOW);
  if (setOk && resetOk) {
    Serial.println("  OK: both SET and RESET detected simultaneously");
    return true;
  }
  Serial.print("  FAIL — SET=");
  Serial.print(setOk   ? "pressed" : "not pressed");
  Serial.print("  RESET=");
  Serial.println(resetOk ? "pressed" : "not pressed");
  return false;
}

// ----- Test 4: live monitor -----
void liveMonitor() {
  Serial.print("\nTest 4: live monitor for ");
  Serial.print(MONITOR_SECS);
  Serial.println("s — move the stick and press buttons");

  bool lastState[INPUT_COUNT] = {};
  unsigned long deadline = millis() + (unsigned long)MONITOR_SECS * 1000;

  while (millis() < deadline) {
    bool changed = false;
    bool current[INPUT_COUNT];

    for (int i = 0; i < INPUT_COUNT; i++) {
      current[i] = isPressed(INPUTS[i]);
      if (current[i] != lastState[i]) changed = true;
    }

    if (changed) {
      bool anyActive = false;
      for (int i = 0; i < INPUT_COUNT; i++) {
        if (current[i]) { anyActive = true; break; }
      }
      if (anyActive) {
        Serial.print("  ACTIVE:");
        for (int i = 0; i < INPUT_COUNT; i++) {
          if (current[i]) { Serial.print(" "); Serial.print(INPUTS[i].name); }
        }
        Serial.println();
      } else {
        Serial.println("  (released)");
      }
      for (int i = 0; i < INPUT_COUNT; i++) lastState[i] = current[i];
    }

    delay(DEBOUNCE_MS);
  }
  Serial.println("  Live monitor ended");
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  for (int i = 0; i < INPUT_COUNT; i++) {
    pinMode(INPUTS[i].pin, INPUT_PULLUP);
  }

  Serial.println("--- Thumbstick Module Test ---");

  bool idleOk  = testIdle();
  bool individOk = testIndividual();
  bool simultOk  = testSimultaneous();
  liveMonitor();

  Serial.println("\n--- Results ---");
  Serial.print("  Idle state:   "); Serial.println(idleOk   ? "PASS" : "FAIL");
  Serial.print("  Individual:   "); Serial.println(individOk ? "PASS" : "FAIL");
  Serial.print("  Simultaneous: "); Serial.println(simultOk  ? "PASS" : "FAIL");
  Serial.println(idleOk && individOk && simultOk ? "\nALL TESTS PASSED" : "\nSOME TESTS FAILED");
}

void loop() {}
