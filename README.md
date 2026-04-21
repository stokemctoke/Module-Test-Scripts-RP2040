# RP2040 Zero Module Test Scripts

Standalone MicroPython test scripts for validating individual hardware modules on the **RP2040 Zero** before integrating them into a project.

Each script is self-contained, prints clear PASS/FAIL output to the serial console, and has a pin-map block at the top for easy rewiring.

---

## Requirements

- [MicroPython](https://micropython.org/download/RPI_PICO/) flashed on the RP2040 Zero
- A serial terminal (Thonny, `mpremote`, `minicom`, etc.)
- Module-specific drivers (noted per script below)

---

## Modules

### 0.96in OLED — SSD1306 (`test_ssd1306_oled.py`)

| Module Pin | RP2040 Zero |
|------------|-------------|
| SDA        | GP4         |
| SCL        | GP5         |
| VCC        | 3.3V        |
| GND        | GND         |

**Driver required:** [`ssd1306.py`](https://github.com/micropython/micropython-lib/blob/master/micropython/drivers/display/ssd1306/ssd1306.py) — copy to the device root.

**Tests run:**
1. Fill screen white / black
2. Text rendering across all rows
3. Corner pixels + centre pixel
4. Rectangle border
5. Crosshair (hline + vline)
6. Scrolling text

Ends with `ALL TESTS PASS` on the display if successful.

---

### SD Card Module — 6-pin SPI (`test_sd_card.py`)

| Module Pin | RP2040 Zero |
|------------|-------------|
| CS         | GP5         |
| SCK        | GP2         |
| MOSI       | GP3         |
| MISO       | GP4         |
| VCC        | 3.3V        |
| GND        | GND         |

> Some modules accept 5V on VCC — check your module's datasheet.

**Driver required:** [`sdcard.py`](https://github.com/micropython/micropython-lib/blob/master/micropython/drivers/storage/sdcard/sdcard.py) — copy to the device root.

**Tests run:**
1. Filesystem stats (total / free MB)
2. List root directory
3. Write a text file
4. Read back and verify content
5. Append to file and verify
6. Delete file
7. ~64 KB write speed benchmark (KB/s reported)

---

### 8-pin Thumbstick Module (`test_thumbstick.py`)

5-direction joystick (UP / DOWN / LEFT / RIGHT / CENTER press) plus 2 extra buttons.
All inputs are digital active-LOW; internal pull-ups are enabled automatically.

| Module Pin | Function | RP2040 Zero |
|------------|----------|-------------|
| 1          | UP       | GP6         |
| 2          | DOWN     | GP7         |
| 3          | LEFT     | GP8         |
| 4          | RIGHT    | GP9         |
| 5          | CENTER   | GP10        |
| 6          | BTN_A    | GP11        |
| 7          | BTN_B    | GP12        |
| 8          | GND      | GND         |

**No extra driver needed.**

**Tests run:**
1. Idle state — verifies all inputs read released at startup
2. Guided individual detection — prompts you to press each input in sequence (5s per input), verifies correct detection
3. Simultaneous BTN_A + BTN_B — hold both at once to verify no blocking
4. 30-second live monitor — prints any state change to the console in real time

---

## Adjusting Pin Assignments

Every script has a clearly labelled constant block near the top:

```python
# test_thumbstick.py
PIN_MAP = {
    "UP":     6,
    "DOWN":   7,
    ...
}

# test_sd_card.py
PIN_SCK  = 2
PIN_MOSI = 3
...
```

Change the GP numbers there to match your wiring — nothing else needs to touch.

---

## Uploading Scripts

Using `mpremote`:

```bash
mpremote connect /dev/ttyACM0 cp test_ssd1306_oled.py :
mpremote connect /dev/ttyACM0 run test_ssd1306_oled.py
```

Or open the file in **Thonny** and press **Run**.
