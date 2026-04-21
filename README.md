# RP2040 Zero Module Test Scripts

Standalone Arduino sketches for validating individual hardware modules on the **Waveshare RP2040 Zero** before integrating them into a project.

Each sketch is self-contained, prints clear PASS/FAIL output to the Serial Monitor (115200 baud), and has a pin-definition block at the top for easy rewiring.

---

## Requirements

- [Arduino IDE](https://www.arduino.cc/en/software) with the **RP2040 / Waveshare RP2040 Zero** board package installed
  - Board manager URL: `https://github.com/earlephilhower/arduino-pico/releases/download/global/package_rp2040_index.json`
  - Board: **Waveshare RP2040 Zero** (`rp2040:rp2040:waveshare_rp2040_zero`)
- Module-specific libraries (noted per sketch below)

---

## Modules

### 0.96in OLED — SSD1306 (`test_ssd1306_oled/`)

| Module Pin | RP2040 Zero |
|------------|-------------|
| SDA        | GP4         |
| SCL        | GP5         |
| VCC        | 3.3V        |
| GND        | GND         |

**Library required:** Install via Arduino Library Manager:
- `Adafruit SSD1306`
- `Adafruit GFX Library` (installed automatically as a dependency)

**Tests run:**
1. Fill screen white / black
2. Text rendering across all rows
3. Corner pixels + centre pixel
4. Rectangle border
5. Crosshair (hline + vline)
6. Scrolling text

Ends with `ALL TESTS PASS` on the display and in Serial Monitor.

---

### SD Card Module — 6-pin SPI (`test_sd_card/`)

| Module Pin | RP2040 Zero |
|------------|-------------|
| CS         | GP5         |
| SCK        | GP2         |
| MOSI       | GP3         |
| MISO       | GP4         |
| VCC        | 3.3V        |
| GND        | GND         |

> Some modules accept 5V on VCC — check your module's datasheet.

**Library required:** `SD` — built into the Arduino / RP2040 core, no extra install needed.

**Tests run:**
1. SD card init
2. Write a text file
3. Read back and verify content
4. Append to file and verify
5. Delete file
6. ~32 KB write speed benchmark (KB/s reported)

---

### 8-pin Thumbstick Module (`test_thumbstick/`)

5-direction joystick (UP / DOWN / LEFT / RIGHT / MID press) plus SET and RESET buttons.
All inputs are digital active-LOW; internal pull-ups are enabled automatically.

| Module Pin | Function | RP2040 Zero |
|------------|----------|-------------|
| 1          | COM      | GND         |
| 2          | UP       | GP6         |
| 3          | DOWN     | GP7         |
| 4          | LEFT     | GP8         |
| 5          | RIGHT    | GP9         |
| 6          | MID      | GP10        |
| 7          | SET      | GP11        |
| 8          | RESET    | GP12        |

**No extra library needed.**

**Tests run:**
1. Idle state — verifies all inputs read released at startup
2. Guided individual detection — prompts you to press each input in sequence (5s per input), verifies correct detection
3. Simultaneous SET + RESET — hold both at once to verify no blocking
4. 30-second live monitor — prints any state change to Serial Monitor in real time

---

## Adjusting Pin Assignments

Every sketch has a clearly labelled `#define` block near the top:

```cpp
// test_thumbstick.ino
#define PIN_UP      6
#define PIN_DOWN    7
...

// test_sd_card.ino
#define PIN_CS    5
#define PIN_SCK   2
...
```

Change the GP numbers there to match your wiring — nothing else needs to touch.

---

## Serial Monitor

Open Serial Monitor at **115200 baud** to see test output. The thumbstick test is interactive — follow the prompts to press each input in sequence.
