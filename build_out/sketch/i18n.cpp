#line 1 "/home/runner/work/ESP32Marauder_xiaomiao/ESP32Marauder_xiaomiao/esp32_marauder/i18n.cpp"
// =============================================================================
// i18n.cpp — Localized string tables for ESP32 Marauder (XiaoMiao port)
// =============================================================================
// text_tableN_en[]  : alias the existing English textN_M from lang_var.h
// zh_text_tableN[]  : Chinese (UTF-8) counterparts, index-aligned with English
//
// IMPORTANT: every zh_text_tableN[i] MUST correspond to text_tableN[i]'s English
// meaning. Index ordering is identical to lang_var.h.
// =============================================================================

#include "i18n.h"
#include "lang_var.h"

// ---------------------------------------------------------------------------
// English tables — reference the existing PROGMEM strings from lang_var.h.
// Defined as non-static so the inline T0..T4() accessors can reach them.
// ---------------------------------------------------------------------------
const char* const text_table0_en[] = {
  text0_0, text0_1, text0_2, text0_3, text0_4, text0_5, text0_6, text0_7, text0_8,
};

const char* const text_table1_en[] = {
  text1_0, text1_1, text1_2, text1_3, text1_4, text1_5, text1_6, text1_7, text1_8,
  text1_9, text1_10, text1_11, text1_12, text1_13, text1_14, text1_15, text1_16,
  text1_17, text1_18, text1_19, text1_20, text1_21, text1_22, text1_23, text1_24,
  text1_25, text1_26, text1_27, text1_28, text1_29, text1_30, text1_31, text1_32,
  text1_33, text1_34, text1_35, text1_36, text1_37, text1_38, text1_39, text1_40,
  text1_41, text1_42, text1_43, text1_44, text1_45, text1_46, text1_47, text1_48,
  text1_49, text1_50, text1_51, text1_52, text1_53, text1_54, text1_55, text1_56,
  text1_57, text1_58, text1_59, text1_60, text1_61, text1_62, text1_63, text1_64,
  text1_65, text1_66, text1_67,
};

const char* const text_table2_en[] = {
  text2_0, text2_1, text2_2, text2_3, text2_4, text2_5, text2_6, text2_7, text2_8,
  text2_9, text2_10, text2_11, text2_12, text2_13, text2_14,
};

const char* const text_table3_en[] = {
  text3_0, text3_1, text3_2, text3_3, text3_4, text3_5,
};

// text_table4[8] is text1_54 (shared). Mirrored here for consistency.
const char* const text_table4_en[] = {
  text4_0, text4_1, text4_2, text4_3, text4_4, text4_5, text4_6, text4_7, text1_54,
  text4_9, text4_10, text4_11, text4_12, text4_13, text4_14, text4_15, text4_16,
  text4_17, text4_18, text4_19, text4_20, text4_21, text4_22, text4_23, text4_24,
  text4_25, text4_26, text4_27, text4_28, text4_29, text4_30, text4_31, text4_32,
  text4_33, text4_34, text4_35, text4_36, text4_37, text4_38, text4_39, text4_40,
  text4_41, text4_42, text4_43, text4_44, text4_45, text4_46, text4_47, text4_48,
  text4_49,
};

// ---------------------------------------------------------------------------
// Chinese tables — index-aligned with the English tables above.
// UTF-8 encoded. Rendered via the CJK drawChar patch in TFT_eSPI.
// ---------------------------------------------------------------------------

const char* const zh_text_table0[] = {
  u8"为硬件串口预留空间...",   // 0  Giving room for HardwareSerial...
  u8"串口已启动",             // 1  Started Serial
  u8"已检查内存",             // 2  Checked RAM
  u8"SD卡已初始化",           // 3  Initialized SD Card
  u8"SD卡初始化失败",         // 4  Failed to Initialize SD Card
  u8"已检查电池配置",         // 5  Checked battery configuration
  u8"温度接口已初始化",       // 6  Initialized temperature interface
  u8"LED接口已初始化",        // 7  Initialized LED Interface
  u8"启动中...",              // 8  Starting...
};

const char* const zh_text_table1[] = {
  u8"SSID列表",               // 0  SSID List
  u8"添加SSID",               // 1  Add SSIDs
  u8"SSID: ",                 // 2  SSID:
  u8"密码:",                  // 3  Password:
  u8"设置已禁用",             // 4  Setting disabled
  u8"设置已开启",             // 5  Setting on
  "ESP32 Marauder ",          // 6  brand kept English
  "WiFi ",                    // 7  WiFi
  u8"Bad USB ",               // 8  Bad USB
  u8"设备 ",                   // 9  Device
  u8"通用应用 ",               // 10 General Apps
  u8"更新中... ",              // 11 Updating...
  u8"选择方式 ",               // 12 Select Method
  u8"确认更新 ",               // 13 Confirm Update
  u8"ESP8266更新 ",            // 14 ESP8266 Update
  u8"更新固件 ",               // 15 Update Firmware
  u8"语言 ",                   // 16 Language
  u8"设备信息 ",               // 17 Device Info
  u8"设置 ",                   // 18 Settings
  u8"蓝牙 ",                   // 19 Bluetooth
  u8"WiFi嗅探 ",               // 20 WiFi Sniffers
  u8"WiFi攻击 ",               // 21 WiFi Attacks
  u8"WiFi通用 ",               // 22 WiFi General
  u8"蓝牙嗅探 ",               // 23 Bluetooth Sniffers
  u8"蓝牙通用 ",               // 24 Bluetooth General
  u8"关闭WiFi ",               // 25 Shutdown WiFi
  u8"关闭BLE ",                // 26 Shutdown BLE
  u8"生成SSID ",               // 27 Generate SSIDs
  u8"清空SSID ",               // 28 Clear SSIDs
  u8"清空AP ",                 // 29 Clear APs
  u8"重启",                    // 30 Reboot
  u8"嗅探",                    // 31 Sniffers
  u8"攻击",                    // 32 Attacks
  u8"通用",                    // 33 General
  u8"蓝牙嗅探器",              // 34 Bluetooth Sniffer
  u8"检测刷卡器",              // 35 Detect Card Skimmers
  u8"测试BadUSB",              // 36 Test BadUSB
  u8"运行Ducky脚本",           // 37 Run Ducky Script
  u8"绘制",                    // 38 Draw
  u8"网络更新",                // 39 Web Update
  u8"SD卡更新",                // 40 SD Update
  u8"ESP8266更新",             // 41 ESP8266 Update
  u8"探测请求嗅探",            // 42 Probe Request Sniff
  u8"信标嗅探",                // 43 Beacon Sniff
  u8"反认证嗅探",              // 44 Deauth Sniff
  u8"数据包监视",              // 45 Packet Monitor
  u8"EAPOL/PMKID扫描",        // 46 EAPOL/PMKID Scan
  u8"检测Pwnagotchi",          // 47 Detect Pwnagotchi
  u8"检测Espressif",           // 48 Detect Espressif
  u8"扫描AP",                  // 49 Scan APs
  u8"信标轰炸列表",            // 50 Beacon Spam List
  u8"随机信标轰炸",            // 51 Beacon Spam Random
  u8"Rick Roll信标",           // 52 Rick Roll Beacon
  u8"探测请求洪泛",            // 53 Probe Req Flood
  u8"反认证洪泛",              // 54 Deauth Flood
  u8"加入WiFi",                // 55 Join WiFi
  u8"选择AP",                  // 56 Select APs
  u8"AP克隆轰炸",              // 57 AP Clone Spam
  u8"原始抓包",                // 58 Raw Capture
  u8"站点嗅探",                // 59 Station Sniff
  u8"清空站点",                // 60 Clear Stations
  u8"选择站点",                // 61 Select Stations
  u8"定向反认证",              // 62 Deauth Targeted
  u8"检测Pineapple",           // 63 Detect Pineapple
  u8"检测多SSID",              // 64 Detect MultiSSID
  u8"选择探测SSID",            // 65 Select probe SSIDs
  "GPS",                      // 66 GPS (acronym kept)
  u8"趣味SSID信标",            // 67 Funny SSID Beacon
};

const char* const zh_text_table2[] = {
  u8"错误:找不到update.bin",            // 0  Error, could not find update.bin
  u8"开始SD卡更新...",                  // 1  Starting SD Update...
  u8"错误:update.bin为空",              // 2  Error, update.bin is empty
  u8"\n重启中...\n",                    // 3  \nRebooting...\n
  u8"无法从/加载update.bin",            // 4  Could not load update.bin from /
  u8"文件大小: ",                       // 5  File size:
  u8"正在写入分区...",                  // 6  Writing file to partition...
  u8"已写入: ",                         // 7  Written:
  u8"仅写入: ",                         // 8  Written only:
  u8"。重试?",                          // 9  . Retry?
  u8" 成功",                            // 10 successfully
  u8"更新完成",                         // 11 Update complete
  u8"更新未能完成",                     // 12 Update could not complete
  u8"发生错误。错误号: ",               // 13 Error Occurred. Error #:
  u8"空间不足,无法开始OTA",             // 14 Not enough space to begin OTA
};

const char* const zh_text_table3[] = {
  u8"正在配置更新服务器...\n\n",        // 0  Configuring update server...
  u8"IP地址: ",                         // 1  IP address:
  u8"更新: ",                           // 2  Update:
  u8"已完成字节: ",                     // 3  Bytes complete:
  u8"更新成功: ",                       // 4  Update Success:
  u8"\n更新服务器设置完成",             // 5  \nCompleted update server setup
};

const char* const zh_text_table4[] = {
  u8" RSSI: ",                          // 0   RSSI:
  u8"潜在刷卡器: ",                     // 1   Potential Skimmer:
  u8"已连接",                           // 2   Already Connected
  u8"连接失败",                         // 3   Failed to connect
  u8"已连接",                           // 4   Connected
  "ForcePMKID",                        // 5   setting key (kept)
  "ForceProbe",                        // 6   setting key (kept)
  "SavePCAP",                          // 7   setting key (kept)
  u8"反认证洪泛",                       // 8   text_table4[8] == text1_54 == "Deauth Flood"
  u8"清空AP中...",                      // 9   Clearing APs...
  u8"已清空AP: ",                       // 10  APs Cleared:
  u8"清空SSID中...",                    // 11  Clearing SSIDs...
  u8"已清空SSID: ",                     // 12  SSIDs Cleared:
  u8"生成SSID中...",                    // 13  Generating SSIDs...
  u8"已生成SSID: ",                     // 14  SSIDs Generated:
  u8"    SSID总数: ",                   // 15     Total SSIDs:
  u8"关闭WiFi中...",                    // 16  Shutting down WiFi...
  u8"WiFi当前未初始化",                 // 17  WiFi not currently initialized
  u8"关闭BLE中...",                     // 18  Shutting down BLE...
  u8"BLE当前未初始化",                  // 19  BLE not currently initialized
  "Marauder固件: ",                    // 20  Firmware: Marauder  (brand kept, label localized)
  u8"版本: ",                           // 21  Version:
  "ESP-IDF: ",                         // 22  ESP-IDF: (kept)
  u8"WSL绕过:已启用",                   // 23  WSL Bypass: enabled
  u8"WSL绕过:已禁用",                   // 24  WSL Bypass: disabled
  u8"站点MAC: ",                        // 25  Station MAC:
  u8"AP MAC: ",                         // 26  AP MAC:
  "",                                  // 27  (empty)
  u8"SD卡:已连接",                      // 28  SD Card: Connected
  u8"SD卡容量: ",                       // 29  SD Card Size:
  u8"SD卡:未连接",                      // 30  SD Card: Not Connected
  u8"SD卡容量:0",                       // 31  SD Card Size: 0
  u8"电池监测:支持",                    // 32  Battery Monitor: supported
  u8"电池电量: ",                       // 33  Battery Lvl:
  u8"电池监测:不支持",                  // 34  Battery Monitor: not supported
  u8"内部温度: ",                       // 35  Internal temp:
  u8" 检测Espressif ",                  // 36  Detect Espressif
  u8" 检测Pwnagotchi ",                 // 37  Detect Pwnagotchi
  u8" 信标嗅探器 ",                     // 38  Beacon Sniffer
  u8" 反认证嗅探器 ",                   // 39  Deauthentication Sniffer
  u8" 探测请求嗅探器 ",                 // 40  Probe Request Sniffer
  u8" 蓝牙嗅探 ",                       // 41  Bluetooth Sniff
  u8" 检测刷卡器 ",                     // 42  Detect Card Skimmers
  u8"正在扫描蓝牙刷卡器\nHC-03, HC-05, HC-06...", // 43
  u8" AP扫描 ",                         // 44  AP Scan
  u8"清空站点中...",                    // 45  Clearing Stations...
  u8"已清空站点: ",                     // 46  Stations Cleared:
  u8"定向反认证",                       // 47  Targeted Deauth
  u8" 检测Pineapple ",                  // 48  Detect Pineapple
  u8" 检测多SSID ",                     // 49  Detect MultiSSID
};
