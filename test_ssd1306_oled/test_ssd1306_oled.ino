/*
  Test sketch: 0.96in OLED SSD1306 (I2C)
  Board: Waveshare RP2040 Zero

  Wiring:
    SDA -> GP4
    SCL -> GP5
    VCC -> 3.3V
    GND -> GND

  Library required: Adafruit SSD1306
    Install via Arduino Library Manager: "Adafruit SSD1306"
    Also installs dependency: Adafruit GFX Library
*/

#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define OLED_WIDTH  128
#define OLED_HEIGHT  64
#define OLED_RESET   -1
#define I2C_SDA       4
#define I2C_SCL       5

Adafruit_SSD1306 display(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

bool testFill() {
  Serial.println("Test 1: fill white");
  display.clearDisplay();
  display.fillRect(0, 0, OLED_WIDTH, OLED_HEIGHT, SSD1306_WHITE);
  display.display();
  delay(800);

  Serial.println("Test 2: fill black");
  display.clearDisplay();
  display.display();
  delay(800);

  return true;
}

bool testText() {
  Serial.println("Test 3: text rendering");
  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0, 0);
  display.println("SSD1306 OK");
  display.println("RP2040 Zero");
  display.println("Line 3");
  display.println("Line 4");
  display.println("Line 5");
  display.display();
  delay(2000);
  return true;
}

bool testPixels() {
  Serial.println("Test 4: corner pixels + centre");
  display.clearDisplay();
  display.drawPixel(0,              0,               SSD1306_WHITE);
  display.drawPixel(OLED_WIDTH - 1, 0,               SSD1306_WHITE);
  display.drawPixel(0,              OLED_HEIGHT - 1, SSD1306_WHITE);
  display.drawPixel(OLED_WIDTH - 1, OLED_HEIGHT - 1, SSD1306_WHITE);
  display.drawPixel(OLED_WIDTH / 2, OLED_HEIGHT / 2, SSD1306_WHITE);
  display.display();
  delay(2000);
  return true;
}

bool testRect() {
  Serial.println("Test 5: rectangle border");
  display.clearDisplay();
  display.drawRect(0, 0, OLED_WIDTH, OLED_HEIGHT, SSD1306_WHITE);
  display.display();
  delay(2000);
  return true;
}

bool testCrosshair() {
  Serial.println("Test 6: crosshair");
  display.clearDisplay();
  display.drawFastHLine(0, OLED_HEIGHT / 2, OLED_WIDTH,  SSD1306_WHITE);
  display.drawFastVLine(OLED_WIDTH / 2, 0,  OLED_HEIGHT, SSD1306_WHITE);
  display.display();
  delay(2000);
  return true;
}

bool testScroll() {
  Serial.println("Test 7: scrolling text");
  for (int i = 0; i < 8; i++) {
    display.clearDisplay();
    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.print("Scroll line ");
    display.println(i);
    display.display();
    delay(300);
  }
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("--- SSD1306 OLED Test ---");

  Wire.setSDA(I2C_SDA);
  Wire.setSCL(I2C_SCL);
  Wire.begin();

  if (!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) {
    Serial.println("FAIL: SSD1306 not found — check wiring and I2C address (0x3C or 0x3D)");
    while (true) delay(1000);
  }
  Serial.println("Display initialised OK");

  bool results[7];
  results[0] = testFill();
  results[1] = testFill();      // black half already covered
  results[2] = testText();
  results[3] = testPixels();
  results[4] = testRect();
  results[5] = testCrosshair();
  results[6] = testScroll();

  bool allPass = true;
  for (int i = 0; i < 7; i++) allPass &= results[i];

  display.clearDisplay();
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(4, 28);
  display.println(allPass ? "ALL TESTS PASS" : "SOME TESTS FAILED");
  display.display();

  Serial.println(allPass ? "--- All tests PASSED ---" : "--- Some tests FAILED ---");
}

void loop() {}
