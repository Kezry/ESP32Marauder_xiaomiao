<!---[![License: MIT](https://img.shields.io/github/license/mashape/apistatus.svg)](https://github.com/justcallmekoko/ESP32Marauder/blob/master/LICENSE)--->
<!---[![Gitter](https://badges.gitter.im/justcallmekoko/ESP32Marauder.png)](https://gitter.im/justcallmekoko/ESP32Marauder)--->
<!---[![Build Status](https://travis-ci.com/justcallmekoko/ESP32Marauder.svg?branch=master)](https://travis-ci.com/justcallmekoko/ESP32Marauder)--->
<!---Shields/Badges https://shields.io/--->

# ESP32 Marauder
<p align="center"><img alt="Marauder logo" src="https://github.com/justcallmekoko/ESP32Marauder/blob/master/pictures/marauder_skull_patch_04_full_final.png?raw=true" width="300"></p>
<p align="center">
  <b>A suite of WiFi/Bluetooth offensive and defensive tools for the ESP32</b>
  <br><br>
  <a href="https://github.com/justcallmekoko/ESP32Marauder/blob/master/LICENSE"><img alt="License" src="https://img.shields.io/github/license/mashape/apistatus.svg"></a>
  <a href="https://gitter.im/justcallmekoko/ESP32Marauder"><img alt="Gitter" src="https://badges.gitter.im/justcallmekoko/ESP32Marauder.png"/></a>
  <br>
  <a href="https://twitter.com/intent/follow?screen_name=jcmkyoutube"><img src="https://img.shields.io/twitter/follow/jcmkyoutube?style=social&logo=twitter" alt="Twitter"></a>
  <a href="https://www.instagram.com/just.call.me.koko"><img src="https://img.shields.io/badge/Follow%20Me-Instagram-orange" alt="Instagram"/></a>
  <br><br>
</p>
    
[![Build and Push](https://github.com/justcallmekoko/ESP32Marauder/actions/workflows/build_push.yml/badge.svg)](https://github.com/justcallmekoko/ESP32Marauder/actions/workflows/build_push.yml)

## Getting Started
Download the [latest release](https://github.com/justcallmekoko/ESP32Marauder/releases/latest) of the firmware.  

Check out the project [wiki](https://github.com/justcallmekoko/ESP32Marauder/wiki) for a full overview of the ESP32 Marauder

# For Sale Now
You can buy the ESP32 Marauder using [this link](https://www.justcallmekokollc.com)

---

# 学而思小喵掌机 (XiaoMiao) 刷写教程

本仓库在原版 ESP32 Marauder 基础上，新增了对**学而思小喵掌机 (XiaoMiao)** 的适配。

- **设备**：ESP32-D0WD，4MB Flash，ST7735 128×160（横屏 160×128）
- **按键**：上/下/左/右 + **A=确认/进入** + **B=返回上一级**
- **屏幕**：横屏，`SCREEN_ORIENTATION=3`（logo 头朝上）
- **固件 Release**：[v0.0.2](https://github.com/Kezry/ESP32Marauder_xiaomiao/releases/tag/v0.0.2)

## 1. 下载固件

从 [Releases · v0.0.2](https://github.com/Kezry/ESP32Marauder_xiaomiao/releases/tag/v0.0.2) 下载这 3 个文件（同一编译产物，必须配套）：

| 文件 | 烧录地址 | 说明 |
|------|---------|------|
| `bootloader_xiaomiao.bin` | `0x1000` | 引导加载器（与 app 配套，不能混用原厂/bootloader） |
| `partitions_xiaomiao.bin` | `0x8000` | min_spiffs 分区表 |
| `esp32_marauder_v0_0_2_20260628_xiaomiao.bin` | `0x10000` | 应用固件本体 |

> 还需要 `boot_app0.bin`（OTA 数据初始镜像），可从 [arduino-esp32 核心](https://github.com/espressif/arduino-esp32/blob/3.3.4/tools/partitions/boot_app0.bin) 下载，烧到 `0xe000`。

## 2. 安装 esptool

```bash
pip install esptool
# 或 Windows 上直接用仓库自带的 FlashFiles/esptool.exe
```

把设备用 USB 连到电脑，确认串口（Windows 设备管理器里看，例如 `COM5`）。

## 3. 备份原厂固件（强烈建议）

烧录会覆盖设备现有固件。**先全片备份**，以便随时恢复：

```bash
# Windows
FlashFiles\esptool.exe --chip esp32 --port COM5 --baud 115200 read_flash 0 0x400000 factory_backup.bin

# Linux / macOS
esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 115200 read_flash 0 0x400000 factory_backup.bin
```

得到 4MB（4,194,304 字节）的 `factory_backup.bin`，妥善保存。

## 4. 烧录固件

按标准 Marauder 地址一次性写入 4 个文件（把 `COM5` 换成你的串口）：

```bash
# Windows（用仓库自带 esptool.exe）
FlashFiles\esptool.exe --chip esp32 --port COM5 --baud 921600 ^
  write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect ^
  0x1000  bootloader_xiaomiao.bin ^
  0x8000  partitions_xiaomiao.bin ^
  0xe000  boot_app0.bin ^
  0x10000 esp32_marauder_v0_0_2_20260628_xiaomiao.bin

# Linux / macOS
esptool.py --chip esp32 --port /dev/ttyUSB0 --baud 921600 \
  write_flash -z --flash_mode dio --flash_freq 80m --flash_size detect \
  0x1000  bootloader_xiaomiao.bin \
  0x8000  partitions_xiaomiao.bin \
  0xe000  boot_app0.bin \
  0x10000 esp32_marauder_v0_0_2_20260628_xiaomiao.bin
```

看到 `Hash of data verified.` + `Hard resetting via RTS pin...` 即烧录成功。设备会自动重启，屏幕显示 Marauder 骷髅 logo 和主菜单。

## 5. 操作说明

| 按键 | 功能 |
|------|------|
| 上 / 下 | 菜单项导航 |
| 左 / 右 | 切换 / 翻页（部分模式下调信道） |
| **A** | 确认 / 进入子菜单 / 长按进入隐身模式 |
| **B** | 返回上一级菜单 |

> ⚠️ **注意**：B 键是 GPIO12（boot strapping 引脚）。**正常使用没问题，但上电启动时不要按住 B 键**，否则芯片会误判 Flash 电压进入异常状态。松开 B 键后再开机即可。

## 6. 恢复原厂固件

如需还原，用第 3 步备份的 `factory_backup.bin` 全片写回：

```bash
FlashFiles\esptool.exe --chip esp32 --port COM5 --baud 921600 write_flash 0 factory_backup.bin
```

## 7. 屏幕朝向 / 颜色不对？

| 现象 | 修改 |
|------|------|
| 上下颠倒 | `esp32_marauder/configs.h` → `MARAUDER_XIAOMIAO` 块的 `SCREEN_ORIENTATION` 在 `1`/`3` 间切换 |
| 颜色反（红蓝互换） | `User_Setup_xiaomiao.h` → `TFT_RGB_ORDER` 在 `TFT_RGB`/`TFT_BGR` 间切换 |
| 边缘花屏/偏移 | `User_Setup_xiaomiao.h` → `ST7735_BLACKTAB` 改成 `ST7735_REDTAB` 或 `GREENTAB3` |

改完需重新编译（见下）。

## 8. 自行编译

本仓库配置了 GitHub Actions 自动构建（`.github/workflows/build_xiaomiao.yml`）。推送到 `feat/xiaomiao` 分支或手动触发 workflow，CI 会在 Linux 上几分钟编出固件并发布到 Release。

如需本地编译：Arduino IDE 选板 **LOLIN D32**，分区方案 **Minimal SPIFFS**，在 `esp32_marauder/configs.h` 顶部取消注释 `#define MARAUDER_XIAOMIAO`，按 [ESP32_Info.md](doc/ESP32_Info.md) 配置 TFT_eSPI 后编译。

> 完整设备硬件信息见 [doc/ESP32_Info.md](doc/ESP32_Info.md)。

