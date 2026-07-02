//                            USER DEFINED SETTINGS
//   Set driver type, fonts to be loaded, pins used and SPI control method etc
//
//   XiaoMiao (学而思小喵掌机) — ESP32-D0WD, ST7735 128x160 (landscape 160x128)
//   Physical: 128 wide x 160 tall (portrait), RGB565, RGB order, no inversion.
//   Mirrors User_Setup_marauder_mini.h but tuned for this hardware.
//
//   Pins per doc/ESP32_Info.md + retro-go-for-xueersi-xiaomiao port:
//     SPI2 shared: SCK=18 MOSI=23 MISO=19
//     TFT: CS=5  DC=4  (NO hardware RST)   SD: CS=22
//     GPIO19 is MISO ONLY. There is no hardware reset line wired to a GPIO:
//     TFT_eSPI uses software reset (TFT_RST=-1). This mirrors the NES-emulator
//     retro-go port, which keeps GPIO19 as a dedicated MISO so the SD card can
//     share the SPI2 bus. Defining TFT_RST=19 makes TFT_eSPI drive GPIO19 as an
//     OUTPUT (reset) and corrupts MISO, so SD reads fail / the 3.x core panics.

// ##################################################################################
// Section 1. Driver + options
// ##################################################################################

#define ST7735_DRIVER

// Colour order: device is RGB (not BGR) per doc
#define TFT_RGB_ORDER  TFT_RGB

// Portrait pixel geometry (physical panel)
#define TFT_WIDTH  128
#define TFT_HEIGHT 160

// ST7735 tab type: 128x160, offset 0,0, RGB, no inversion -> BLACKTAB
// (matches doc MADCTL table: rot0 = 0x00, offsets 0/0; invert=False)
#define ST7735_BLACKTAB
// (do NOT define ST7735_INITB / GREENTAB* / REDTAB — they add offsets/invert)

// No colour inversion on this panel
// #define TFT_INVERSION_ON
// #define TFT_INVERSION_OFF

#define TFT_BACKLIGHT_ON HIGH

// ##################################################################################
// Section 2. Pins (ESP32, hardware SPI on VSPI/SPI2)
// ##################################################################################

#define TFT_MISO 19
#define TFT_MOSI 23
#define TFT_SCLK 18
#define TFT_CS   5
#define TFT_DC   4
#define TFT_RST  -1   // NO hardware RST. ST7735 is software-reset (TFT_SWRST).
                      // GPIO19 must stay as MISO (input) for the shared-SPI SD.
#define TOUCH_CS -1
// TFT_BL defined in configs.h as -1 (no backlight control pin)

// ##################################################################################
// Section 3. Fonts (same set as Marauder Mini)
// ##################################################################################

#define LOAD_GLCD
#define LOAD_FONT2
#define LOAD_FONT4
#define LOAD_FONT6
#define LOAD_FONT7
#define LOAD_FONT8
#define LOAD_GFXFF
#define SMOOTH_FONT

// ##################################################################################
// Section 4. SPI frequency
// ##################################################################################

// ST7735: doc shows 20MHz stable (MicroPython); 40MHz only with LVGL DMA. Use 20MHz.
#define SPI_FREQUENCY       20000000
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY 2500000
