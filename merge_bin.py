Import("env")
import os

def merge_bin(source, target, env):
    build_dir  = env.subst("$BUILD_DIR")
    prog_name  = env.subst("$PROGNAME")
    merged_bin = os.path.join(build_dir, prog_name + "_merged.bin")

    FLASH_SIZE = 16 * 1024 * 1024  # 16MB

    files = [
        (0x00000, os.path.join(build_dir, "bootloader.bin")),
        (0x08000, os.path.join(build_dir, "partitions.bin")),
        (0x10000, os.path.join(build_dir, prog_name + ".bin")),
    ]

    # Buffer 16MB rempli de 0xFF
    flash = bytearray(b'\xff' * FLASH_SIZE)

    for offset, path in files:
        with open(path, "rb") as f:
            data = f.read()
        flash[offset:offset + len(data)] = data
        print(f"  0x{offset:05X}  {os.path.basename(path):20s}  {len(data):>8} bytes")

    with open(merged_bin, "wb") as f:
        f.write(flash)

    print(f"\n✓ Merged → {merged_bin} (16MB)")

env.AddPostAction("$BUILD_DIR/${PROGNAME}.bin", merge_bin)


