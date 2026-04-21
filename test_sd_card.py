"""
Test script: SD Card Module (SPI, 6-pin)
RP2040 Zero default wiring:
  CS   -> GP5
  SCK  -> GP2  (SPI0 SCK)
  MOSI -> GP3  (SPI0 TX)
  MISO -> GP4  (SPI0 RX)
  VCC  -> 3.3V  (some modules accept 5V — check your module)
  GND  -> GND

Requires: sdcard.py driver on the device
  https://github.com/micropython/micropython-lib/blob/master/micropython/drivers/storage/sdcard/sdcard.py
"""

import machine
import sdcard
import uos
import time

SPI_ID   = 0
PIN_SCK  = 2
PIN_MOSI = 3
PIN_MISO = 4
PIN_CS   = 5
SPI_BAUD = 1_000_000   # 1 MHz for init; driver raises it after mount

MOUNT_POINT = "/sd"
TEST_FILE   = f"{MOUNT_POINT}/test_rp2040.txt"

def init_sd():
    spi = machine.SPI(SPI_ID,
                      baudrate=SPI_BAUD,
                      polarity=0,
                      phase=0,
                      sck=machine.Pin(PIN_SCK),
                      mosi=machine.Pin(PIN_MOSI),
                      miso=machine.Pin(PIN_MISO))
    cs = machine.Pin(PIN_CS, machine.Pin.OUT)
    sd = sdcard.SDCard(spi, cs)
    vfs = uos.VfsFat(sd)
    uos.mount(vfs, MOUNT_POINT)
    print(f"SD card mounted at {MOUNT_POINT}")
    return sd

def test_statvfs():
    print("Test 1: filesystem stats")
    stats = uos.statvfs(MOUNT_POINT)
    block_size  = stats[0]
    total_blocks = stats[2]
    free_blocks  = stats[3]
    total_mb = (block_size * total_blocks) / (1024 * 1024)
    free_mb  = (block_size * free_blocks)  / (1024 * 1024)
    print(f"  Total: {total_mb:.1f} MB   Free: {free_mb:.1f} MB")

def test_listdir():
    print("Test 2: list root directory")
    entries = uos.listdir(MOUNT_POINT)
    print(f"  Entries ({len(entries)}): {entries}")

def test_write():
    print("Test 3: write file")
    payload = "RP2040 Zero SD test\nLine 2\nLine 3\n"
    with open(TEST_FILE, "w") as f:
        f.write(payload)
    print(f"  Wrote {len(payload)} bytes to {TEST_FILE}")

def test_read():
    print("Test 4: read file back")
    with open(TEST_FILE, "r") as f:
        data = f.read()
    print(f"  Read: {repr(data)}")
    assert "RP2040 Zero SD test" in data, "Content mismatch"
    print("  Content verified OK")

def test_append():
    print("Test 5: append to file")
    with open(TEST_FILE, "a") as f:
        f.write("Appended line\n")
    with open(TEST_FILE, "r") as f:
        lines = f.readlines()
    print(f"  File now has {len(lines)} lines")
    assert lines[-1].strip() == "Appended line", "Append failed"
    print("  Append verified OK")

def test_delete():
    print("Test 6: delete file")
    uos.remove(TEST_FILE)
    entries = uos.listdir(MOUNT_POINT)
    assert "test_rp2040.txt" not in entries, "File still exists after remove"
    print("  File deleted OK")

def test_speed():
    print("Test 7: write speed (~64 KB)")
    chunk = b"X" * 512
    start = time.ticks_ms()
    path = f"{MOUNT_POINT}/speed_test.bin"
    with open(path, "wb") as f:
        for _ in range(128):
            f.write(chunk)
    elapsed = time.ticks_diff(time.ticks_ms(), start)
    kb_per_s = (128 * 512 / 1024) / (elapsed / 1000)
    print(f"  64 KB written in {elapsed} ms  (~{kb_per_s:.0f} KB/s)")
    uos.remove(path)

def run():
    print("--- SD Card Test ---")
    init_sd()
    test_statvfs()
    test_listdir()
    test_write()
    test_read()
    test_append()
    test_delete()
    test_speed()
    uos.umount(MOUNT_POINT)
    print("SD card unmounted")
    print("--- All tests complete ---")

run()
