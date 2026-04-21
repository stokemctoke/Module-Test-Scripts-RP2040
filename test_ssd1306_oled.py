"""
Test script: 0.96in OLED SSD1306 (I2C)
RP2040 Zero default wiring:
  SDA -> GP4
  SCL -> GP5
  VCC -> 3.3V
  GND -> GND

Requires: ssd1306.py driver on the device
  https://github.com/micropython/micropython-lib/blob/master/micropython/drivers/display/ssd1306/ssd1306.py
"""

import machine
import ssd1306
import time

I2C_SDA = 4
I2C_SCL = 5
I2C_FREQ = 400_000
OLED_WIDTH = 128
OLED_HEIGHT = 64

def init_display():
    i2c = machine.I2C(0, sda=machine.Pin(I2C_SDA), scl=machine.Pin(I2C_SCL), freq=I2C_FREQ)
    devices = i2c.scan()
    if not devices:
        raise RuntimeError("No I2C devices found — check wiring")
    print(f"I2C devices found: {[hex(d) for d in devices]}")
    return ssd1306.SSD1306_I2C(OLED_WIDTH, OLED_HEIGHT, i2c)

def test_fill(oled):
    print("Test 1: fill screen white")
    oled.fill(1)
    oled.show()
    time.sleep(1)
    print("Test 2: fill screen black")
    oled.fill(0)
    oled.show()
    time.sleep(1)

def test_text(oled):
    print("Test 3: text rendering")
    oled.fill(0)
    oled.text("SSD1306 OK", 0, 0)
    oled.text("RP2040 Zero", 0, 12)
    oled.text("Line 3", 0, 24)
    oled.text("Line 4", 0, 36)
    oled.text("Line 5", 0, 48)
    oled.show()
    time.sleep(2)

def test_pixels(oled):
    print("Test 4: pixel corners")
    oled.fill(0)
    oled.pixel(0, 0, 1)
    oled.pixel(OLED_WIDTH - 1, 0, 1)
    oled.pixel(0, OLED_HEIGHT - 1, 1)
    oled.pixel(OLED_WIDTH - 1, OLED_HEIGHT - 1, 1)
    oled.pixel(OLED_WIDTH // 2, OLED_HEIGHT // 2, 1)
    oled.show()
    time.sleep(2)

def test_lines(oled):
    print("Test 5: rectangle border")
    oled.fill(0)
    oled.rect(0, 0, OLED_WIDTH, OLED_HEIGHT, 1)
    oled.show()
    time.sleep(2)

def test_hline_vline(oled):
    print("Test 6: crosshair")
    oled.fill(0)
    oled.hline(0, OLED_HEIGHT // 2, OLED_WIDTH, 1)
    oled.vline(OLED_WIDTH // 2, 0, OLED_HEIGHT, 1)
    oled.show()
    time.sleep(2)

def test_scroll(oled):
    print("Test 7: scrolling text")
    oled.fill(0)
    for i in range(8):
        oled.fill(0)
        oled.text(f"Scroll line {i}", 0, 0)
        oled.show()
        time.sleep(0.3)

def run():
    print("--- SSD1306 OLED Test ---")
    oled = init_display()
    test_fill(oled)
    test_text(oled)
    test_pixels(oled)
    test_lines(oled)
    test_hline_vline(oled)
    test_scroll(oled)
    oled.fill(0)
    oled.text("ALL TESTS PASS", 4, 28)
    oled.show()
    print("--- All tests complete ---")

run()
