#!/usr/bin/env python3
"""Convert a PNG to a 24-bit BMP (cross-platform replacement for macOS sips)."""
import sys

try:
    from PIL import Image
except ImportError:
    sys.exit("Pillow is required: python -m pip install pillow")


def main() -> int:
    if len(sys.argv) != 3:
        sys.exit(f"usage: {sys.argv[0]} <src.png> <dst.bmp>")
    src, dst = sys.argv[1], sys.argv[2]
    img = Image.open(src).convert("RGBA")
    background = Image.new("RGBA", img.size, (255, 255, 255, 255))
    img = Image.alpha_composite(background, img)
    img.convert("RGB").save(dst, format="BMP")
    return 0


if __name__ == "__main__":
    sys.exit(main())
