"""
PlatformIO pre-build extra script for the xiaomiao env.

TFT_eSPI selects its hardware config via User_Setup_Select.h, which by default
includes the library's own User_Setup.h (ILI9341, wrong pins). The CI workflow
(build_xiaomiao_v2core.yml) patches that by uncommenting the xiaomiao include
after copying our User_Setup_xiaomiao.h into the lib. We mirror that here so a
local `pio run -e xiaomiao` build picks up the same ST7735 + shared-SPI pins.

Idempotent: safe to run on every build.
"""
import os
import re
import shutil

Import("env")

PROJECT_DIR = env["PROJECT_DIR"]


def patch_tft_espi():
    # TFT_eSPI was installed by PlatformIO under .pio/libdeps/<env>/TFT_eSPI.
    libdeps = os.path.join(PROJECT_DIR, ".pio", "libdeps")
    if not os.path.isdir(libdeps):
        return  # nothing to patch yet (first install runs LDF before this)

    tft_dir = None
    for env_name in os.listdir(libdeps):
        candidate = os.path.join(libdeps, env_name, "TFT_eSPI")
        if os.path.isdir(candidate):
            tft_dir = candidate
            break
    if tft_dir is None:
        return

    # 1. Copy our setup file(s) into the library root.
    for name in ("User_Setup_xiaomiao.h", "User_Setup.h"):
        src = os.path.join(PROJECT_DIR, name)
        if os.path.isfile(src):
            shutil.copy2(src, os.path.join(tft_dir, name))

    # 2. Make User_Setup_Select.h include the xiaomiao setup.
    sel = os.path.join(tft_dir, "User_Setup_Select.h")
    if not os.path.isfile(sel):
        return
    with open(sel, "r", encoding="utf-8", errors="ignore") as f:
        text = f.read()

    # Ensure exactly one active xiaomiao include. Disable the generic
    # User_Setup.h line so our config wins, then enable our include.
    text = re.sub(
        r"^(\s*)#include\s+<User_Setup\.h>",
        r"\1//#include <User_Setup.h>",
        text,
        count=1,
        flags=re.MULTILINE,
    )
    if re.search(r"^(\s*)#include\s+<User_Setup_xiaomiao\.h>", text, re.MULTILINE):
        # already active
        return
    text = re.sub(
        r"^(\s*)//#include\s+<User_Setup_xiaomiao\.h>",
        r"\1#include <User_Setup_xiaomiao.h>",
        text,
        count=1,
        flags=re.MULTILINE,
    )
    # If the line didn't exist at all, append it after the USER_SETUP_LOADED guard.
    if not re.search(r"#include\s+<User_Setup_xiaomiao\.h>", text):
        text = text.replace(
            "#ifndef USER_SETUP_LOADED",
            "#ifndef USER_SETUP_LOADED\n#include <User_Setup_xiaomiao.h>",
            1,
        )

    with open(sel, "w", encoding="utf-8") as f:
        f.write(text)


def patch_tft_drawchar():
    """Inject CJK glyph rendering into TFT_eSPI's drawChar for the GLCD font path.
    When uniCode is in the CJK range (0x4E00-0x9FFF or fullwidth 0xFF00-0xFFEF),
    render from the embedded cjk_glyphs.h bitmap table instead of the ASCII font.
    """
    libdeps = os.path.join(PROJECT_DIR, ".pio", "libdeps")
    if not os.path.isdir(libdeps):
        return
    tft_cpp = None
    for env_name in os.listdir(libdeps):
        candidate = os.path.join(libdeps, env_name, "TFT_eSPI", "TFT_eSPI.cpp")
        if os.path.isfile(candidate):
            tft_cpp = candidate
            break
    if tft_cpp is None:
        return

    # Copy cjk_glyphs.h into the TFT_eSPI library directory so #include resolves
    tft_dir = os.path.dirname(tft_cpp)
    cjk_src = os.path.join(PROJECT_DIR, "esp32_marauder", "cjk_glyphs.h")
    if os.path.isfile(cjk_src):
        shutil.copy2(cjk_src, os.path.join(tft_dir, "cjk_glyphs.h"))

    with open(tft_cpp, "r", encoding="utf-8", errors="ignore") as f:
        text = f.read()

    # Already patched?
    if "CJK_BITMAPS" in text:
        return

    # Add include at the top of TFT_eSPI.cpp
    include_line = '#include "cjk_glyphs.h" // CJK font (auto-injected)\n'
    text = include_line + text

    # Inject CJK rendering at the start of drawChar(uniCode, x, y, font),
    # right after the _vpOoB guard.
    cjk_block = """  // === CJK glyph rendering (auto-injected by apply_tft_setup.py) ===
  if (font == 1 && ((uniCode >= 0x4E00 && uniCode <= 0x9FFF) || (uniCode >= 0xFF00 && uniCode <= 0xFFEF))) {
    int16_t idx = cjkLookup(uniCode);
    if (idx >= 0) {
      int32_t xd = x + _xDatum;
      int32_t yd = y + _yDatum;
      for (int row = 0; row < 12; row++) {
        uint8_t hi = pgm_read_byte(&CJK_BITMAPS[idx][row * 2]);
        uint8_t lo = pgm_read_byte(&CJK_BITMAPS[idx][row * 2 + 1]);
        uint16_t bits = (hi << 8) | lo;
        for (int col = 0; col < 12; col++) {
          if (bits & (1 << (15 - col)))
            drawPixel(xd + col, yd + row, textcolor);
          else if (textbgcolor != textcolor)
            drawPixel(xd + col, yd + row, textbgcolor);
        }
      }
      return 13; // advance width: 12px glyph + 1px spacing
    }
  }
"""

    # Insert after the first occurrence of "if (_vpOoB || !uniCode) return 0;"
    text = text.replace(
        "  if (_vpOoB || !uniCode) return 0;",
        "  if (_vpOoB || !uniCode) return 0;\n" + cjk_block,
        1,
    )

    with open(tft_cpp, "w", encoding="utf-8") as f:
        f.write(text)


patch_tft_espi()
patch_tft_drawchar()
