#!/usr/bin/env python3
"""Validate that i18n.cpp's en/zh tables are index-aligned with lang_var.h.

Properly handles commas inside string literals (the naive split-by-comma
over-counts when a Chinese string contains a comma like "扫描...HC-03, HC-05").
"""
import re
import sys


def count_entries(block: str) -> int:
    # strip line comments first
    clean = '\n'.join(line.split('//')[0] for line in block.split('\n'))
    entries = 0
    cur = ''
    in_str = False
    esc = False
    for c in clean:
        if esc:
            cur += c
            esc = False
            continue
        if c == '\\':
            cur += c
            esc = True
            continue
        if c == '"':
            in_str = not in_str
            cur += c
        elif c == ',' and not in_str:
            if cur.strip():
                entries += 1
            cur = ''
        else:
            cur += c
    if cur.strip():
        entries += 1
    return entries


def main():
    with open('esp32_marauder/i18n.cpp', encoding='utf-8') as f:
        ic = f.read()
    with open('esp32_marauder/lang_var.h', encoding='utf-8') as f:
        lv = f.read()
    ok = True
    print(f"{'table':<8} {'lang_var':<10} {'i18n_en':<10} {'zh':<6} {'status'}")
    for n in range(5):
        m = re.search(rf'text_table{n}\[\][^=]*=\s*\{{(.*?)\}};', lv, re.S)
        lv_c = count_entries(m.group(1)) if m else 0
        m = re.search(rf'text_table{n}_en\[\][^=]*=\s*\{{(.*?)\}};', ic, re.S)
        en_c = count_entries(m.group(1)) if m else 0
        mzh = re.search(rf'zh_text_table{n}\[\][^=]*=\s*\{{(.*?)\}};', ic, re.S)
        zh_c = count_entries(mzh.group(1)) if mzh else 0
        status = 'OK' if lv_c == en_c == zh_c else 'MISMATCH'
        if status != 'OK':
            ok = False
        print(f"table{n:<3} {lv_c:<10} {en_c:<10} {zh_c:<6} {status}")
    return 0 if ok else 1


if __name__ == '__main__':
    sys.exit(main())
