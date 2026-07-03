#pragma once

// =============================================================================
// i18n.h — Chinese / English language toggle for ESP32 Marauder (XiaoMiao port)
// =============================================================================
// Design:
//   - g_lang_en == false  -> Chinese (default)
//   - g_lang_en == true   -> English
//   - Set once at boot from SPIFFS settings ("Language" bool), see settings.cpp.
//   - L("english") returns the localized form of a literal. Lookup is O(n) over
//     a small static table; call sites pass compile-time string literals so the
//     compiler can fold the address.
//   - Existing text_tableN[] references keep working as English; their localized
//     counterparts live in zh_text_tableN[] and are returned by LT0..LT4(i).
//
// Scope (per user request): screen UI only. Serial/debug output stays English.
// Brand names (ESP32 Marauder / JustCallMeKoko / version) stay English in both.
// =============================================================================

#ifndef I18N_H
#define I18N_H

#include "configs.h"
#include <cstring>
#include <stdint.h>
// Arduino WString (for the String type used by i18n_isBackName).
// Pulled in transitively by configs.h -> Arduino.h in practice, but be explicit
// so the inline helpers compile in any translation unit.
#ifdef ARDUINO
#include <WString.h>
#else
// WString forward when not building under Arduino (host tooling).
class String;
#endif

// Runtime language flag. false = Chinese (default), true = English.
// Defined in esp32_marauder.ino; set from SPIFFS in settings_obj.begin().
extern bool g_lang_en;

// ---------------------------------------------------------------------------
// L() — literal translation macro.
//   L("Settings")  ->  (g_lang_en ? "Settings" : "设置")
// Implemented as an inline function with a static lookup table so any literal
// passed at compile time resolves to its Chinese counterpart at runtime, or
// falls back to the English argument if no translation is registered.
// ---------------------------------------------------------------------------
struct I18nEntry { const char* en; const char* zh; };

inline const char* i18n_lookup(const char* en) {
  static const I18nEntry TABLE[] = {
    // ---- Common / shared ----
    {"Back",            "返回"},
    {"Settings",        "设置"},
    {"ON",              "开"},
    {"OFF",             "关"},
    {"Save",            "保存"},
    {"Load",            "加载"},
    {"Exit",            "退出"},
    {"Cancel",          "取消"},
    {"Yes",             "是"},
    {"Close",           "关闭"},
    {"FAIL",            "失败"},
    {"Open",            "开放"},
    {"Unknown",         "未知"},
    {"TRUE",            "是"},
    {"FALSE",           "否"},

    // ---- Main menu headers ----
    {"WiFi ",           "WiFi"},
    {"Device ",         "设备"},
    {"Bluetooth ",      "蓝牙"},
    {"Settings ",       "设置"},
    {"ESP32 Marauder ", "ESP32 Marauder"},   // brand kept English
    {"Sniffers",        "嗅探"},
    {"Attacks",         "攻击"},
    {"General",         "通用"},
    {"Scanners",        "扫描器"},
    {"Save/Load Files", "保存/加载文件"},
    {"Save SSIDs",      "保存SSID"},
    {"Load SSIDs",      "加载SSID"},
    {"Save APs",        "保存AP"},
    {"Load APs",        "加载AP"},
    {"Save Airtags",    "保存Airtag"},
    {"Load Airtags",    "加载Airtag"},
    {"Bluetooth Attacks", "蓝牙攻击"},
    {"Select",          "选择"},
    {"Select Stations", "选择站点"},
    {"Select ALL",      "全选"},
    {"Active IPs",      "活跃IP"},
    {"AP Info",         "AP信息"},
    {"Set MACs",        "设置MAC"},
    {"Generate AP MAC", "生成AP MAC"},
    {"Generate STA MAC","生成STA MAC"},
    {"GPS",             "GPS"},
    {"GPS Data",        "GPS数据"},
    {"GPS POI",         "GPS兴趣点"},
    {"EP HTML List",    "EP HTML列表"},
    {"Mini Keyboard",   "迷你键盘"},
    {"Delete SD Files", "删除SD文件"},
    {"SD Files",        "SD文件"},
    {"Bin Files",       "Bin文件"},
    {"Probe Requests",  "探测请求"},
    {"Evil Portal",     "邪恶门户"},
    {"SSIDs",           "SSID列表"},

    // ---- WiFi menu nodes ----
    {"Wardriving",          "战争驾驶"},
    {"Ping Scan",           "Ping扫描"},
    {"ARP Scan",            "ARP扫描"},
    {"Port Scan All",       "端口扫描"},
    {"SSH Scan",            "SSH扫描"},
    {"Telnet Scan",         "Telnet扫描"},
    {"SMTP Scan",           "SMTP扫描"},
    {"DNS Scan",            "DNS扫描"},
    {"HTTP Scan",           "HTTP扫描"},
    {"HTTPS Scan",          "HTTPS扫描"},
    {"RDP Scan",            "RDP扫描"},
    {"Packet Count",        "数据包计数"},
    {"Channel Analyzer",    "信道分析"},
    {"Channel Summary",     "信道汇总"},
    {"Scan AP/STA",         "扫描AP/STA"},
    {"Fox Hunt",            "寻狐"},
    {"MAC Monitor",         "MAC监视"},
    {"SAE Commit",          "SAE Commit"},
    {"SAE Commit Flood",    "SAE Commit洪泛"},
    {"Wardrive",            "战争驾驶"},
    {"Station Wardrive",    "站点战争驾驶"},
    {"Karma",               "Karma"},
    {"Bad Msg",             "恶意报文"},
    {"Bad Msg Targeted",    "定向恶意报文"},
    {"Assoc Sleep",         "关联睡眠"},
    {"Assoc Sleep Targ",    "定向关联睡眠"},
    {"Channel Switch",      "信道切换"},
    {"Quiet Time",          "静默时段"},
    {"Access Points",       "接入点"},
    {"User SSIDs",          "用户SSID"},
    {"Select EP HTML File", "选择EP HTML文件"},
    {"Select APs",          "选择AP"},
    {"View AP Info",        "查看AP信息"},
    {"Select Stations",     "选择站点"},
    {"Join WiFi",           "加入WiFi"},
    {"Join Saved WiFi",     "加入已存WiFi"},
    {"Start AP",            "启动AP"},
    {"Host AP Info",        "本机AP信息"},
    {"Shutdown WiFi",       "关闭WiFi"},
    {"Clone AP MAC",        "克隆AP MAC"},
    {"Clone STA MAC",       "克隆STA MAC"},
    {"SSID List",           "SSID列表"},
    {"Add SSIDs",           "添加SSID"},

    // ---- Bluetooth menu nodes ----
    {"Bluetooth Attacks",   "蓝牙攻击"},
    {"Flipper Sniff",       "Flipper嗅探"},
    {"Airtag Sniff",        "Airtag嗅探"},
    {"Airtag Monitor",      "Airtag监视"},
    {"Bluetooth Analyzer",  "蓝牙分析"},
    {"Flock Sniff",         "Flock嗅探"},
    {"Meta Detect",         "Meta检测"},
    {"Sour Apple",          "酸苹果"},
    {"Apple Juice",         "苹果汁"},
    {"Swiftpair Spam",      "Swiftpair轰炸"},
    {"Samsung BLE Spam",    "三星BLE轰炸"},
    {"Google BLE Spam",     "谷歌BLE轰炸"},
    {"Flipper BLE Spam",    "Flipper BLE轰炸"},
    {"BLE Spam All",        "BLE全轰炸"},
    {"Spoof Airtag",        "伪造Airtag"},
    {"Simple Sniff",        "简单嗅探"},
    {"Simple Sniff 2",      "简单嗅探2"},

    // ---- Device menu nodes ----
    {"Update Firmware",     "更新固件"},
    {"Brightness",          "亮度"},

    // ---- GPS menu nodes ----
    {"NMEA Stream",         "NMEA数据流"},
    {"GPS Tracker",         "GPS追踪"},
    {"Mark POI",            "标记兴趣点"},
    {"Delete Selected",     "删除选中"},

    // ---- Scan / status screens (WiFiScan.cpp drawCentreString etc.) ----
    {"Packet Rate",         "包速率"},
    {"EAPOL Sniff",         "EAPOL嗅探"},
    {"Signal Monitor",      "信号监视"},
    {"BLE Spam Samsung",    "BLE三星轰炸"},
    {"BLE Spam Google",     "BLE谷歌轰炸"},
    {"BLE Spam Flipper",    "BLE Flipper轰炸"},
    {"BLE Spoof Airtag",    "BLE伪造Airtag"},
    {"Targeted Networks",   "定向网络"},
    {"No Networks Selected","未选择网络"},
    {"Transmitting...",     "发送中..."},
    {"Connected!",          "已连接!"},        // used as \nConnected!
    {"Returning...",        "返回中..."},
    {"Failed to connect",   "连接失败"},        // used as \nFailed to connect
    {"connected",           "已连接"},          // used as \nconnected
    {"Client disconnected", "客户端断开"},      // \nClient disconnected
    {"Loading...",          "加载中..."},
    {"Starting",            "启动中"},
    {"Connecting",          "连接中"},
    {"Filter Active",       "过滤已启用"},
    {"Stats",               "统计"},
    {"Loaded APs: ",        "已加载AP: "},
    {"Loaded SSIDs: ",      "已加载SSID: "},
    {"Loaded Airtags: ",    "已加载Airtag: "},
    {"Saved APs: ",         "已保存AP: "},
    {"Saved SSIDs: ",       "已保存SSID: "},
    {"Saved Airtags: ",     "已保存Airtag: "},
    {"POI Logged",          "兴趣点已记录"},
    {"POI Log Failed",      "兴趣点记录失败"},
    {"Deleting /",          "删除中 /"},
    {"Opening ",            "打开 "},

    // ---- Info / Device info labels ----
    {"Hardware: ",          "硬件: "},
    {"Security: ",          "加密: "},
    {"Channel: ",           "信道: "},
    {"Gateway: ",           "网关: "},
    {"Netmask: ",           "掩码: "},
    {"IP address: ",        "IP地址: "},
    {"Clients: ",           "客户端: "},
    {"Stations: ",          "站点: "},
    {"Size: ",              "大小: "},
    {"File: ",              "文件: "},

    // ---- WiFi security enum labels (keep WPA/WEP/WAPI acronyms) ----
    {"WEP",                 "WEP"},
    {"WPA",                 "WPA"},
    {"WPA2",                "WPA2"},
    {"WPA3",                "WPA3"},
    {"WPA/WPA2 Mixed",      "WPA/WPA2混合"},
    {"WPA2 Enterprise",     "WPA2企业版"},
    {"WPA3 Enterprise",     "WPA3企业版"},
    {"WAPI",                "WAPI"},

    // ---- Channel analyzer / packet monitor axis labels ----
    {"X Scale:",            "X缩放:"},
    {"Y Scale:",            "Y缩放:"},
    {" - Beacons",          " - 信标"},
    {" - Deauths",          " - 反认证"},
    {" - Probes",           " - 探测"},
    {" - EAPOL",            " - EAPOL"},

    // ---- Mini keyboard / touch UI hints ----
    {"C - Add Char",        "C - 添加字符"},
    {"C - Save",            "C - 保存"},
    {"C(Hold) - Enter",     "C(长按) - 确认"},
    {"C(Hold) - Exit",      "C(长按) - 退出"},
    {"C(Hold) - Rem Char",  "C(长按) - 删字符"},
    {"L/R - Prev/Nxt Char", "L/R - 上/下一字符"},
    {"U/D - Prev/Nxt Char", "U/D - 上/下一字符"},
    {"U/D - Rem/Add Char",  "U/D - 删/加字符"},
    {"U(Hold) - Enter",     "U(长按) - 确认"},
    {"Enter - Enter",       "Enter - 确认"},
    {"Enter - Save",        "Enter - 保存"},
    {"Esc - Exit",          "Esc - 退出"},

    // ---- Brightness setup hints (full-screen, harmless on mini) ----
    {"TAP TOP = BRIGHTER",          "点顶部=更亮"},
    {"TAP BOTTOM = DIMMER",         "点底部=更暗"},
    {"TAP MIDDLE or WAIT 3s = SAVE","点中部或等3秒=保存"},

    // ---- Status / banner ----
    {" | WIFI | ",         " | WiFi | "},
    {"(Filtered)",         "(已过滤)"},
    {"Initializing...",    "初始化中..."},
    // ---- minor display prefixes / axis labels ----
    {"CH: ",               "信道: "},
    {"Frames/",            "帧/"},
    {"BLE Beacons/",       "BLE信标/"},
    {"BRIGHTNESS",         "亮度"},
  };
  for (const auto& e : TABLE) {
    if (strcmp(e.en, en) == 0)
      return g_lang_en ? e.en : e.zh;
  }
  return en;  // fallback: English unchanged
}

// Convenience macro for literal call sites: L("Settings")
#define L(en) (i18n_lookup(en))

// Detect a "Back" menu node by name, in either language. Used to suppress the
// icon on back/return rows (MenuFunctions.cpp previously compared against the
// English text09 literal, which breaks once the row label is localized).
inline bool i18n_isBackName(const String& name) {
  return name == "Back" || name == "\xe8\xbf\x94\xe5\x9b\x9e"; // "返回" in UTF-8
}

// ---------------------------------------------------------------------------
// LT0..LT4 — localized accessors for the existing lang_var.h tables.
//   text_table1[18]  ->  LT1(18)
// Returns Chinese entry when !g_lang_en, English otherwise.
// The zh_* tables are defined in i18n.cpp (kept out of the header to avoid
// duplicate-symbol linker errors across translation units).
// ---------------------------------------------------------------------------
extern const char* const text_table0_en[];
extern const char* const text_table1_en[];
extern const char* const text_table2_en[];
extern const char* const text_table3_en[];
extern const char* const text_table4_en[];
extern const char* const zh_text_table0[];
extern const char* const zh_text_table1[];
extern const char* const zh_text_table2[];
extern const char* const zh_text_table3[];
extern const char* const zh_text_table4[];

inline const char* LT0(uint8_t i) { return g_lang_en ? text_table0_en[i] : zh_text_table0[i]; }
inline const char* LT1(uint8_t i) { return g_lang_en ? text_table1_en[i] : zh_text_table1[i]; }
inline const char* LT2(uint8_t i) { return g_lang_en ? text_table2_en[i] : zh_text_table2[i]; }
inline const char* LT3(uint8_t i) { return g_lang_en ? text_table3_en[i] : zh_text_table3[i]; }
inline const char* LT4(uint8_t i) { return g_lang_en ? text_table4_en[i] : zh_text_table4[i]; }

#endif // I18N_H
