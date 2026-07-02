# ESP32 小喵掌机 (XiaoMiao) — 完整设备信息

## 一、设备标识

| 项目 | 值 |
|------|-----|
| 设备名称 | 学而思 ESP32 小喵掌机 (XiaoMiao) |
| 项目仓库 | https://github.com/pysn2012/xueersi-xiaomiao |
| 许可证 | Apache-2.0 |
| 教程来源 | 微信公众号「PY学习笔记」 |
| 仓库语言 | Python 100% (MicroPython) |
| 仓库目录 | `esphome/` `lvgl-mpy/` `microptyhon/` `bins/` `image/` |

## 二、芯片信息（esptool v5.3.0 实测）

| 项目 | 值 |
|------|-----|
| 芯片型号 | ESP32-D0WD (revision v1.0) |
| 功能 | Wi-Fi, BT, Dual Core + LP Core, 240MHz |
| 晶振频率 | 40MHz |
| MAC 地址 | 70:b8:f6:1a:4d:18 |
| Flash 厂商 ID | 0x5e |
| Flash Device ID | 0x4016 |
| Flash 容量 | **4MB** (4,194,304 bytes) |
| Flash 电压 | 3.3V (strapping pin) |
| Coding Scheme | None |
| Vref calibration | in eFuse |
| 串口 | COM5 (USB 转串口) |

> Device ID `0x4016` 中的 `16` 表示 2^16 = 64K * 64KB = 4MB

## 三、Flash 分区表（实测）

分区表地址：`0x00008000`

| # | Type | SubType | Offset | Size | Name |
|---|------|---------|--------|------|------|
| 0 | data | phy (0x02) | 0x00009000 | 24KB | nvs |
| 1 | data | ota (0x00) | 0x0000F000 | 4KB | phy_init |
| 2 | app | factory (0x00) | 0x00010000 | 1.9MB | factory |
| 3 | data | 0x81 | 0x00200000 | 1.5MB | vfs |
| 4 | data | 0x81 | 0x00380000 | 512KB | cfg |

> 最后分区结束地址：0x00380000 + 0x80000 = **0x00400000**（恰好 4MB）

## 四、固件备份

| 项目 | 值 |
|------|-----|
| 备份文件 | `esp32_wrover_backup_20260627.bin` |
| 文件大小 | 4,194,304 bytes (4.00 MB) |
| 备份方式 | `esptool read_flash 0 0x400000`（全片一次性） |
| 波特率 | 115200 |
| 耗时 | ~382 秒 |
| Bootloader 校验 | `0xE9` @ 0x1000 — VALID |
| 分区表校验 | `0xAA50` @ 0x8000 — VALID |
| App 校验 | `0xE9` @ 0x10000 — VALID |
| 备份脚本 | `esp32_backup.py`（同目录） |

## 五、显示屏详细参数

| 项目 | 值 |
|------|-----|
| 驱动 IC | **ST7735** |
| 物理分辨率 | **128 x 160** (RGB) |
| 颜色格式 | **RGB565** (16-bit) |
| 颜色模式 | RGB (非 BGR) |
| 颜色反转 | 默认关闭 (invert=False) |
| 接口 | SPI2 |
| SPI 速率 | 20MHz (MicroPython) / 40MHz (ESPHome/LVGL) |
| 旋转 | MicroPython: 90 度（横向显示 160 x 128） |
| 背光 | 无背光控制引脚（bl=None） |
| Framebuffer | `bytearray(width * height * 2)` = 40,960 bytes |
| LVGL 双缓冲 | 30,720 bytes x 2 (DMA) |

### Marauder 适配的屏幕方向（重要）

Marauder 固件在 TFT_eSPI 里把面板定义成 **portrait（竖屏）128×160**（`TFT_WIDTH=128 / TFT_HEIGHT=160`），再通过 `setRotation(SCREEN_ORIENTATION)` 旋转到横屏使用。Marauder 的菜单/绘制逻辑以**旋转后的可视区**为准：`SCREEN_WIDTH = 160`、`SCREEN_HEIGHT = 128`。

> 物理面板是 128(短)×160(长)。Marauder 用横屏 160×128，可视纵向只有 128 像素。

| 配置项 | 值 | 说明 |
|--------|-----|------|
| `SCREEN_ORIENTATION` | **3** | TFT_eSPI 旋转值 = 270°。**这是 Marauder 正确的朝向**（logo/文字头朝上）|
| 旋转值 0 | portrait 128×160 | 不用 |
| 旋转值 1 | landscape 160×128 | ❌ 实测**上下颠倒**（屏被翻转 180°）|
| 旋转值 2 | portrait 128×160 (倒) | 不用 |
| 旋转值 3 | landscape 160×128 | ✅ **正确朝向**，菜单布局据此适配 |

**排错指南**（若屏幕朝向不对，改 `User_Setup_xiaomiao.h` 无效，要改 `esp32_marauder/configs.h` 里 `MARAUDER_XIAOMIAO` 块的 `SCREEN_ORIENTATION`）：
- 上下颠倒 → 把 `SCREEN_ORIENTATION` 在 `1` 和 `3` 之间切换（两者都是横屏，互为 180° 翻转）
- 旋转 90°（变成竖屏长条）→ 值应为 `1` 或 `3`（横屏），不是 `0`/`2`
- 左右镜像或颜色反 → 改 `User_Setup_xiaomiao.h` 的 `TFT_RGB_ORDER`（RGB↔BGR）或 `ST7735_BLACKTAB`→`ST7735_REDTAB`

### 显示屏引脚

| 引脚 | GPIO | 备注 |
|------|------|------|
| SCK | GPIO18 | SPI2 共享 |
| MOSI | GPIO23 | SPI2 共享 |
| MISO | GPIO19 | SPI2 共享。**专作 MISO**（见下方"SD 卡与 TFT 复位"说明） |
| CS | GPIO5 | TFT 片选 |
| DC | GPIO4 | 数据/命令选择 |
| RES | (无独立引脚) | **不用硬件复位**。Marauder 固件里 `TFT_RST=-1`，ST7735 走软件复位（SWRESET 0x01），使 GPIO19 专作 SD MISO。参考 retro-go-for-xueersi-xiaomiao 移植方案。 |

### ST7735 初始化序列关键寄存器

| 寄存器 | 值 | 说明 |
|--------|-----|------|
| FRMCTR1 (0xB1) | 0x01, 0x2C, 0x2D | 帧率控制 - 正常模式 |
| FRMCTR2 (0xB2) | 0x01, 0x2C, 0x2D | 帧率控制 - 空闲模式 |
| FRMCTR3 (0xB3) | 0x01, 0x2C, 0x2D, 0x01, 0x2C, 0x2D | 帧率控制 - 部分模式 |
| INVCTR (0xB4) | 0x07 | 反转控制 |
| PWCTR1 (0xC0) | 0xA2, 0x02, 0x84 | 功率控制 1 |
| PWCTR2 (0xC1) | 0xC5 | 功率控制 2 |
| PWCTR3 (0xC2) | 0x0A, 0x00 | 功率控制 3 |
| PWCTR4 (0xC3) | 0x8A, 0x2A | 功率控制 4 |
| PWCTR5 (0xC4) | 0x8A, 0xEE | 功率控制 5 |
| VMCTR1 (0xC5) | 0x0E | VCOM 电压控制 |
| COLMOD (0x3A) | 0x05 | 16-bit/pixel (RGB565) |
| GMCTRP1 (0xE0) | 02 1C 07 12 37 32 29 2D 29 25 2B 39 00 01 03 10 | Gamma 正极性 |
| GMCTRN1 (0xE1) | 03 1D 07 06 2E 2C 29 2D 2E 2E 37 3F 00 00 02 10 | Gamma 负极性 |

### 旋转参数表 (MADCTL 0x36)

| 旋转值 | MADCTL | 方向 | 宽 x 高 | X偏移 | Y偏移 |
|--------|--------|------|---------|-------|-------|
| 0 | 0x00 | Portrait (默认) | 128 x 160 | 0 | 0 |
| 1 | 0x60 | Landscape (90°) | 160 x 128 | 0 | 0 |
| 2 | 0xC0 | Inverted Portrait (180°) | 128 x 160 | 0 | 0 |
| 3 | 0xA0 | Inverted Landscape (270°) | 160 x 128 | 0 | 0 |

> 实际使用 rotate=1（90°），横向显示 160 x 128

### 预定义颜色 (RGB565)

| 颜色 | 值 |
|------|-----|
| BLACK | 0x0000 |
| BLUE | 0x001F |
| RED | 0xF800 |
| GREEN | 0x07E0 |
| CYAN | 0x07FF |
| MAGENTA | 0xF81F |
| YELLOW | 0xFFE0 |
| WHITE | 0xFFFF |

### RGB565 转换公式

```python
# RGB(0-255) -> RGB565
def color(r, g, b):
    return (r & 0xF8) << 8 | (g & 0xFC) << 3 | b >> 3
```

## 六、MicroSD 卡详细参数

| 项目 | 值 |
|------|-----|
| 接口 | SPI2（与 TFT 共享） |
| SPI 速率 | 20MHz~40MHz |
| 文件系统 | FATFS |
| CS 主动拉高 | 是（通过各自 CS 分时复用） |

### MicroSD 引脚

| 引脚 | GPIO | 备注 |
|------|------|------|
| SCK | GPIO18 | SPI2 共享 |
| MOSI | GPIO23 | SPI2 共享 |
| MISO | GPIO19 | SPI2 共享（专作 MISO，TFT 不占用此脚） |
| CS | GPIO22 | SD 卡片选 |

> **SPI 共享方案**：TFT CS=GPIO5, SD CS=GPIO22，通过 CS 互斥实现分时复用。同一时刻只能访问一个设备。
>
> **重要（SD 修复）**：GPIO19 不能同时作 TFT RST 和 SD MISO。Marauder 固件设 `TFT_RST=-1`，TFT 走软件复位，GPIO19 全程作 MISO，SD 才能正常识别（对齐 retro-go-for-xueersi-xiaomiao）。

## 七、按键详细参数

### 引脚与电气特性

| 按键 | GPIO | 输入模式 | ADC 通道 | 硬件约束 |
|------|------|---------|---------|---------|
| 上 | GPIO2 | INPUT_PULLUP | — | — |
| 下 | GPIO13 | INPUT_PULLUP | — | — |
| 左 | GPIO27 | INPUT_PULLUP | — | — |
| 右 | GPIO35 | INPUT_PULLUP | 仅输入 | 无内部上拉（硬件需外部上拉） |
| A | GPIO34 | INPUT_PULLUP | 仅输入 | 无内部上拉（硬件需外部上拉） |
| B | GPIO12 | INPUT_PULLUP | — | **启动敏感**（上电避免高电平） |

### 按键事件类型

| 事件 | 说明 |
|------|------|
| down_func | 按键按下回调 |
| up_func | 按键释放回调 |

> 使用 EasyButton 库实现消抖，支持长按/短按事件

## 八、蜂鸣器详细参数

| 项目 | 值 |
|------|-----|
| 类型 | 无源蜂鸣器 |
| 引脚 | GPIO14 |
| 驱动方式 | LEDC PWM |
| PWM 频率 | 可调（默认 2000Hz） |
| PWM 占空比 | 0~65535 (duty_u16) |
| 音调范围 | 通过频率控制不同音调 |

### 预置音效

| 名称 | 说明 |
|------|------|
| mario | 超级马里奥旋律 |
| jingle | 铃儿响叮当 |

### MicroPython 初始化

```python
from machine import PWM
buzzer = PWM(Pin(14), freq=2000, duty=512)
```

## 九、ADC 传感器详细参数

### 光照传感器

| 项目 | 值 |
|------|-----|
| 引脚 | GPIO36 |
| ADC 通道 | ADC1_CH0 |
| 硬件约束 | 仅输入 |
| ADC 衰减 | 12dB (满量程 ~3.3V) |
| 采样范围 | 0 ~ 3.3V |
| 校准映射 | 0.0V → 0%, 3.3V → 100% |
| 更新间隔 | 10s (ESPHome) |
| 单位 | % |
| 设备类型 | illuminance |

### 热敏电阻 (NTC)

| 项目 | 值 |
|------|-----|
| 引脚 | GPIO39 |
| ADC 通道 | ADC1_CH3 |
| 硬件约束 | 仅输入 |
| ADC 衰减 | 12dB (满量程 ~3.3V) |
| 分压电路 | 下拉式 (DOWNSTREAM) |
| 分压电阻 | 10kΩ (板上) |
| 参考电压 | 3.3V |
| B 常数 | 3950 |
| 参考温度 | 25°C |
| 参考电阻 | 10kΩ |
| 更新间隔 | 10s |
| 单位 | °C |

### NTC 温度计算公式

```
R_ntc = R_ref * V_adc / (V_ref - V_adc)  (下拉分压)

T = 1 / (1/T_ref + ln(R_ntc/R_ref) / B)

其中:
  T_ref = 25°C = 298.15K
  R_ref = 10kΩ
  B = 3950
```

## 十、I2C 总线

| 项目 | 值 |
|------|-----|
| SCL | GPIO15 |
| SDA | GPIO21 |
| I2C 通道 | I2C0 |
| 共享设备地址 | 0x40 (电机/LED 共用) |

### MicroPython 初始化

```python
from machine import I2C
i2c = I2C(0, scl=Pin(15), sda=Pin(21))
```

## 十一、UART0

| 项目 | 值 |
|------|-----|
| TX | GPIO1 |
| RX | GPIO3 |
| 说明 | 原生串口（不经过 USB） |

## 十二、预留扩展 IO

| GPIO | 接口类型 | 功能 | 备注 |
|------|---------|------|------|
| GPIO25 | PH2.0 3P | DAC1 / I2S / 通用IO | I2S 时钟共享 |
| GPIO26 | PH2.0 3P | DAC2 / I2S / 通用IO | I2S 时钟共享 |
| GPIO32 | PH2.0 3P | 通用IO / 触摸 | — |
| GPIO33 | PH2.0 3P | 通用IO / 触摸 | — |

> GPIO25/26 共用 I2S 时钟，麦克风/功放采样率必须一致

## 十三、GPIO 完整总览

```
GPIO0  → (boot strapping, 下载模式)
GPIO1  → UART0 TX
GPIO2  → 按键-上 (boot strapping)
GPIO3  → UART0 RX
GPIO4  → TFT DC (数据/命令)
GPIO5  → TFT CS (片选) (boot strapping)
GPIO12 → 按键-B (启动敏感! boot strapping)
GPIO13 → 按键-下
GPIO14 → 蜂鸣器 (PWM)
GPIO15 → I2C SCL (boot strapping)
GPIO18 → SPI2 SCK (TFT/SD 共享)
GPIO19 → SPI2 MISO (TFT/SD 共享) / TFT RES
GPIO21 → I2C SDA
GPIO22 → SD卡 CS (片选)
GPIO23 → SPI2 MOSI (TFT/SD 共享)
GPIO25 → 扩展 (DAC1/I2S)
GPIO26 → 扩展 (DAC2/I2S)
GPIO27 → 按键-左
GPIO32 → 扩展 (触摸T9)
GPIO33 → 扩展 (触摸T8)
GPIO34 → 按键-A (仅输入, 无内部上拉)
GPIO35 → 按键-右 (仅输入, 无内部上拉)
GPIO36 → 光照传感器 (仅输入, ADC1_CH0)
GPIO39 → 热敏电阻 (仅输入, ADC1_CH3)
```

### Boot Strapping 引脚

| GPIO | 上电默认功能 | 约束 |
|------|------------|------|
| GPIO0 | 1=正常启动, 0=下载模式 | 电路需保证上电为高 |
| GPIO2 | 1=正常启动 | 上电需为高 |
| GPIO5 | 1=正常启动 | 上电需为高 |
| GPIO12 (B键) | 0=Flash 3.3V, 1=Flash 1.8V | **必须为低** (Flash 为 3.3V) |
| GPIO15 | 0=不输出调试日志, 1=输出 | 影响 boot 日志 |

## 十四、关键限制与注意事项

| 约束 | 引脚 | 说明 | 影响 |
|------|------|------|------|
| 仅输入引脚 | GPIO34/35/36/39 | 不可设为输出模式，无内部上拉/下拉电阻 | 按键 A/右需外部上拉 |
| 启动敏感 | GPIO12 | B 键，上电阶段避免外部高电平 | 按键设计需注意上电状态 |
| 共享 SPI | GPIO18/23/19 | TFT/SD 卡分时复用 | 需互斥锁，禁止同时访问 |
| RES/MISO 复用 | GPIO19 | TFT RESET 与 SD MISO 共用 | **Marauder 固件不用硬件 RST**（`TFT_RST=-1`，软件复位），GPIO19 专作 SD MISO，避免冲突 |
| I2C 地址冲突 | — | 电机/LED 共用 0x40 | 需协议层区分设备 |
| I2S 采样率 | GPIO25/26 | 麦克风/功放采样率必须一致 | 音频系统设计约束 |
| 无背光控制 | — | bl=None | 无法软件调节屏幕亮度 |

## 十五、开发框架支持

### ESPHome 配置

| 项目 | 值 |
|------|-----|
| 框架 | ESP-IDF |
| 最低芯片版本 | v3.1 (ESP32-D0WD-V3) |
| PSRAM | 已启用 |
| Flash 大小 | 4MB |
| ESPHome 最低版本 | 2025.5.0 |
| OTA | 支持 (esphome platform) |
| API | 支持 (Home Assistant) |
| WiFi | 支持 (含 AP fallback) |
| 字体 | Noto Sans SC 12px (Google Fonts) |

### MicroPython 配置

| 项目 | 值 |
|------|-----|
| SPI 速率 | 20MHz |
| 显示驱动 | st7735_buf (Framebuffer 模式) |
| 显示库 | EasyDisplay (RGB565) |
| 字体 | text_lite_16px_2312.v3.bmf (BMF v3, 16px) |
| 菜单库 | EasyMenu |
| 按键库 | EasyButton |
| 蜂鸣器库 | BUZZER (自定义) |
| 通信 | ESPNow (WiFi P2P) |

### LVGL MicroPython 配置

| 项目 | 值 |
|------|-----|
| SPI 速率 | 40MHz (screen.py) / 20MHz (screen2.py) |
| 显示驱动 | st7735 (LVGL 原生) |
| 颜色格式 | RGB565 |
| 字节序 | RGB |
| 字节交换 | True (rgb565_byte_swap) |
| 双缓冲 | 支持 (30,720 bytes x 2, DMA) |
| 旋转 | 90° (DISPLAY_ROTATION._90) |
| 缓冲内存 | INTERNAL + DMA |

## 十六、ESPHome 传感器配置详解

### 光照传感器 (ADC)

```yaml
# GPIO36, ADC1_CH0
- platform: adc
  pin: GPIO36
  id: adc_light
  name: "Light"
  update_interval: 10s
  attenuation: 12db        # 3.3V 满量程
  filters:
    - calibrate_linear:
        - 0.0 -> 0.0       # 0V = 0%
        - 3.3 -> 100.0     # 3.3V = 100%
  unit_of_measurement: "%"
  device_class: illuminance
  state_class: measurement
```

### 热敏电阻 (NTC)

```yaml
# GPIO39, ADC1_CH3
# 三级转换: ADC -> resistance -> NTC temperature
- platform: adc
  pin: GPIO39
  id: adc_ntc
  update_interval: 10s
  attenuation: 12db

- platform: resistance
  id: ntc_resistance
  sensor: adc_ntc
  configuration: DOWNSTREAM   # 分压电阻接 GND
  resistor: 10kOhm
  reference_voltage: 3.3V

- platform: ntc
  id: board_temp
  name: "Temp"
  sensor: ntc_resistance
  calibration:
    b_constant: 3950
    reference_temperature: 25°C
    reference_resistance: 10kOhm
  unit_of_measurement: "°C"
  device_class: temperature
  state_class: measurement
```

### WiFi 信号

```yaml
- platform: wifi_signal
  name: "Wifi Signal dB"
  id: wifi_signal_db
  update_interval: 60s

- platform: copy
  source_id: wifi_signal_db
  name: "Wifi Strength"
  id: wifi_strength
  filters:
    - lambda: return min(max(2 * (x + 100.0), 0.0), 100.0);
  unit_of_measurement: "%"
```

## 十七、MicroPython 初始化模板

```python
from machine import SPI, Pin, ADC, PWM, I2C

# ===== SPI2 总线 (TFT + SD 共享) =====
spi = SPI(2, baudrate=20000000, polarity=0, phase=0,
          sck=Pin(18), mosi=Pin(23), miso=Pin(19))

# ===== TFT 显示屏 (ST7735) =====
from st7735_buf import ST7735
dp = ST7735(width=160, height=128, spi=spi,
            cs=5, dc=4, res=19, rotate=1,
            bl=None, invert=False, rgb=True)

# ===== EasyDisplay 封装 =====
from easydisplay import EasyDisplay
ed = EasyDisplay(display=dp, font="/font/text_lite_16px_2312.v3.bmf",
                 show=True, color=0xFFFF, clear=True, color_type="RGB565")

# ===== 按键 (6键, PULL_UP) =====
key_up    = Pin(2,  Pin.IN, Pin.PULL_UP)
key_down  = Pin(13, Pin.IN, Pin.PULL_UP)
key_left  = Pin(27, Pin.IN, Pin.PULL_UP)
key_right = Pin(35, Pin.IN, Pin.PULL_UP)   # 仅输入, 无内部上拉
key_a     = Pin(34, Pin.IN, Pin.PULL_UP)   # 仅输入, 无内部上拉
key_b     = Pin(12, Pin.IN, Pin.PULL_UP)   # 启动敏感!

# ===== 光照传感器 (ADC1_CH0) =====
light = ADC(Pin(36))
light.atten(ADC.ATTN_11DB)   # 0~3.3V

# ===== 热敏电阻 (ADC1_CH3) =====
ntc = ADC(Pin(39))
ntc.atten(ADC.ATTN_11DB)     # 0~3.3V

# ===== 蜂鸣器 (PWM) =====
buzzer = PWM(Pin(14), freq=2000, duty=512)

# ===== I2C =====
i2c = I2C(0, scl=Pin(15), sda=Pin(21))
```

## 十八、备份/恢复命令参考

```bash
# 全片备份
python -m esptool --chip esp32 --port COM5 --baud 115200 read-flash 0 0x400000 esp32_wrover_backup.bin

# 全片恢复
python -m esptool --chip esp32 --port COM5 --baud 115200 write-flash 0 esp32_wrover_backup.bin

# 仅读取分区表
python -m esptool --chip esp32 --port COM5 --baud 115200 read-flash 0x8000 0xC00 partition_table.bin

# 查看 Flash 信息
python -m esptool --chip esp32 --port COM5 --baud 115200 flash-id

# 擦除全片
python -m esptool --chip esp32 --port COM5 --baud 115200 erase-flash
```

---

*文档生成时间：2026-06-27 15:00 GMT+8*
*数据来源：esptool v5.3.0 实测 + GitHub 仓库源码分析 (esphome/microptyhon/lvgl-mpy)*
*设备 MAC：70:b8:f6:1a:4d:18*
