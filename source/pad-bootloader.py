# A script to add padding to the end of the bootloader.bin
# so that it occupies precisely 32K (first 2 sectors of the flash)

BOOTLOADER_SIZE = 0x8000
BOOTLOADER_FILE = "../cmake-build-debug-mingw/bare-metal-series-bootloader.bin"

with open(BOOTLOADER_FILE, "rb") as f:
    raw_file = f.read()

bytes_to_pad = BOOTLOADER_SIZE - len(raw_file)
padding = bytes([0xff for _ in range(bytes_to_pad)])

with open(BOOTLOADER_FILE, "wb") as f:
    f.write(raw_file + padding)
