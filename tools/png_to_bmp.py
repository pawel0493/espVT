#!/usr/bin/env python3
"""Convert PNG to 24-bit BMP (cross-platform replacement for macOS sips)."""

import sys
from PIL import Image


def main() -> None:
    if len(sys.argv) != 3:
        raise SystemExit("Usage: png_to_bmp.py <input.png> <output.bmp>")

    src, dst = sys.argv[1], sys.argv[2]
    img = Image.open(src)

    # Flatten alpha to white background for deterministic cropping in bmp_to_rgb565.py
    if img.mode in ("RGBA", "LA", "P"):
        img = img.convert("RGBA")
        bg = Image.new("RGB", img.size, (255, 255, 255))
        bg.paste(img, mask=img.split()[3])
        img = bg
    else:
        img = img.convert("RGB")

    img.save(dst, "BMP")


if __name__ == "__main__":
    main()
