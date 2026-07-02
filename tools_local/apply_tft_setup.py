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


patch_tft_espi()
