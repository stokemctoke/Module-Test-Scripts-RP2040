/*
  Test sketch: SD Card Module (SPI, 6-pin)
  Board: Waveshare RP2040 Zero

  Wiring:
    CS   -> GP5
    SCK  -> GP2  (SPI0 SCK)
    MOSI -> GP3  (SPI0 TX)
    MISO -> GP4  (SPI0 RX)
    VCC  -> 3.3V  (check your module — some accept 5V)
    GND  -> GND

  Library required: SD (built-in to Arduino / RP2040 core)
*/

#include <SPI.h>
#include <SD.h>

#define PIN_CS    5
#define PIN_SCK   2
#define PIN_MOSI  3
#define PIN_MISO  4

#define TEST_FILENAME "rp2040tst.txt"

bool testInit() {
  Serial.println("Test 1: SD init");
  SPI.setRX(PIN_MISO);
  SPI.setTX(PIN_MOSI);
  SPI.setSCK(PIN_SCK);

  if (!SD.begin(PIN_CS)) {
    Serial.println("  FAIL: SD.begin() returned false — check wiring/card");
    return false;
  }
  Serial.println("  OK: card initialised");
  return true;
}

bool testWrite() {
  Serial.println("Test 2: write file");
  File f = SD.open(TEST_FILENAME, FILE_WRITE);
  if (!f) {
    Serial.println("  FAIL: could not open file for writing");
    return false;
  }
  f.println("RP2040 Zero SD test");
  f.println("Line 2");
  f.println("Line 3");
  f.close();
  Serial.println("  OK: file written");
  return true;
}

bool testRead() {
  Serial.println("Test 3: read file back");
  File f = SD.open(TEST_FILENAME, FILE_READ);
  if (!f) {
    Serial.println("  FAIL: could not open file for reading");
    return false;
  }
  bool found = false;
  while (f.available()) {
    String line = f.readStringUntil('\n');
    Serial.print("  > ");
    Serial.println(line);
    if (line.indexOf("RP2040 Zero SD test") >= 0) found = true;
  }
  f.close();
  if (!found) {
    Serial.println("  FAIL: expected content not found");
    return false;
  }
  Serial.println("  OK: content verified");
  return true;
}

bool testAppend() {
  Serial.println("Test 4: append to file");
  File f = SD.open(TEST_FILENAME, FILE_WRITE);
  if (!f) {
    Serial.println("  FAIL: could not open file for append");
    return false;
  }
  f.println("Appended line");
  f.close();

  // Verify appended line is present
  f = SD.open(TEST_FILENAME, FILE_READ);
  if (!f) {
    Serial.println("  FAIL: could not reopen for verification");
    return false;
  }
  bool found = false;
  while (f.available()) {
    String line = f.readStringUntil('\n');
    if (line.indexOf("Appended line") >= 0) { found = true; break; }
  }
  f.close();
  if (!found) {
    Serial.println("  FAIL: appended line not found");
    return false;
  }
  Serial.println("  OK: append verified");
  return true;
}

bool testDelete() {
  Serial.println("Test 5: delete file");
  SD.remove(TEST_FILENAME);
  if (SD.exists(TEST_FILENAME)) {
    Serial.println("  FAIL: file still exists after remove");
    return false;
  }
  Serial.println("  OK: file deleted");
  return true;
}

bool testSpeed() {
  Serial.println("Test 6: write speed (~32 KB)");
  const char* chunk = "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX";
  // 512 chars
  File f = SD.open("speedtst.bin", FILE_WRITE);
  if (!f) {
    Serial.println("  FAIL: could not create speed test file");
    return false;
  }
  unsigned long start = millis();
  for (int i = 0; i < 64; i++) {
    f.print(chunk);
  }
  f.close();
  unsigned long elapsed = millis() - start;
  float kbps = (32.0f * 1024.0f) / elapsed;
  Serial.print("  32 KB in ");
  Serial.print(elapsed);
  Serial.print(" ms  (~");
  Serial.print(kbps, 0);
  Serial.println(" KB/s)");
  SD.remove("speedtst.bin");
  return true;
}

void setup() {
  Serial.begin(115200);
  delay(2000);
  Serial.println("--- SD Card Test ---");

  bool r1 = testInit();
  if (!r1) {
    Serial.println("Init failed — aborting remaining tests");
    return;
  }

  bool r2 = testWrite();
  bool r3 = testRead();
  bool r4 = testAppend();
  bool r5 = testDelete();
  bool r6 = testSpeed();

  bool allPass = r1 && r2 && r3 && r4 && r5 && r6;
  Serial.println(allPass ? "--- All tests PASSED ---" : "--- Some tests FAILED ---");
}

void loop() {}
