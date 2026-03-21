# build_merged.py
import os

BUILD_DIR = r".pio\build\esp32-s3-devkitc-1"
OUTPUT    = "firmware_merged.bin"
FLASH_SIZE = 16 * 1024 * 1024  # 16MB

files = [
    (0x00000, os.path.join(BUILD_DIR, "bootloader.bin")),
    (0x08000, os.path.join(BUILD_DIR, "partitions.bin")),
    (0x10000, os.path.join(BUILD_DIR, "firmware.bin")),
]

# Crée un buffer 16MB rempli de 0xFF
flash = bytearray(b'\xff' * FLASH_SIZE)

for offset, path in files:
    with open(path, "rb") as f:
        data = f.read()
    flash[offset:offset + len(data)] = data
    print(f"  0x{offset:05X}  {os.path.basename(path):20s}  {len(data):>8} bytes")

with open(OUTPUT, "wb") as f:
    f.write(flash)

print(f"\n✓ {OUTPUT} ({FLASH_SIZE // 1024 // 1024}MB) prêt")
