#!/usr/bin/env python3
"""Audit: find user-facing string literals NOT wrapped in L() or routed via
LT0..LT4(). Helps catch UI text that still renders English-only in CN mode.
"""
import re
import sys

FILES = [
    'esp32_marauder/MenuFunctions.cpp',
    'esp32_marauder/WiFiScan.cpp',
    'esp32_marauder/SDInterface.cpp',
    'esp32_marauder/Display.cpp',
    'esp32_marauder/esp32_marauder.ino',
    'esp32_marauder/Buffer.cpp',
]

# call sites that print to screen (and addNodes label / .name assignment)
UI_PATTERNS = [
    'addNodes', 'showCenterText', 'drawCentreString', 'twoPartDisplay',
    'displaySetting', 'drawString', '.name =',
]

# Match a string literal NOT wrapped by L( . The L is always followed by '('
# before the quote, so a 2-char lookbehind for "L(" excludes wrapped ones.
STR = re.compile(r'(?<!L\()"((?:[^"\\]|\\.)*?)"')


def main():
    missing = []
    for f in FILES:
        try:
            with open(f, encoding='utf-8', errors='replace') as fh:
                lines = fh.readlines()
        except FileNotFoundError:
            continue
        for i, line in enumerate(lines, 1):
            if 'Serial.' in line:
                continue
            if not any(p in line for p in UI_PATTERNS):
                continue
            for m in STR.finditer(line):
                s = m.group(1)
                if len(s) < 4:
                    continue
                if s.startswith('/') or s.endswith(('.bin', '.log', '.json')):
                    continue
                if not (s[0].isalpha() and s[0].isupper()):
                    continue
                missing.append((f.split('/')[-1], i, s))
    for m in missing:
        print(f'  {m[0]}:{m[1]}  {m[2]!r}')
    print(f'total unwrapped UI literals: {len(missing)}')


if __name__ == '__main__':
    main()
