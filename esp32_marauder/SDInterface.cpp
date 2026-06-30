#include "SDInterface.h"
#include "lang_var.h"

#ifdef MARAUDER_XIAOMIAO
  #include "driver/gpio.h"
#endif

#ifdef HAS_C5_SD
  SDInterface::SDInterface(SPIClass* spi, int cs)
    : _spi(spi), _cs(cs) {}
#endif

bool SDInterface::initSD() {
  #ifdef HAS_SD
    String display_string = "";

    #ifdef KIT
      pinMode(SD_DET, INPUT);
      if (digitalRead(SD_DET) != LOW) {
        this->supported = false;
        return false;
      }
    #endif

    pinMode(SD_CS, OUTPUT);

    // XiaoMiao: GPIO19 is shared between TFT RST and SD MISO. TFT_eSPI configures
    // GPIO19 as OUTPUT (RST) during tft.init() and keeps it as OUTPUT through every
    // subsequent draw operation. Display::init() never runs during setup(), so by the
    // time initSD() is called GPIO19 is still configured as OUTPUT (RST), which makes
    // SD reads return garbage. The Arduino SD library's internal SPI object is already
    // initialized (by TFT_eSPI), so SPI.begin() returns early WITHOUT re-attaching the
    // pins. We must (a) force GPIO19 back to its input state via the ESP-IDF GPIO
    // driver, and (b) force the global SPI bus to re-attach with the correct pins by
    // calling end()/begin() — this re-points the SPI2 MISO input at GPIO19.
    #ifdef MARAUDER_XIAOMIAO
      Serial.println(F("XiaoMiao SD: releasing GPIO19 from TFT RST -> SD MISO"));
      // Force GPIO19 (shared RST/MISO) out of TFT_eSPI's OUTPUT state. gpio_reset_pin
      // disables the output driver and returns the pad to high-impedance input.
      gpio_reset_pin(GPIO_NUM_19);
      delay(5);

      // Hold SD_CS idle (high) while we (re)start the bus.
      pinMode(SD_CS, OUTPUT);
      digitalWrite(SD_CS, HIGH);
      pinMode(TFT_SCLK, OUTPUT);
      pinMode(TFT_MOSI, OUTPUT);
      delay(5);

      // Force the shared SPI2 bus to re-attach the pins (end() tears the bus down so
      // the next begin() actually re-programs the GPIO matrix with MISO=GPIO19).
      SPI.end();
      delay(2);
      SPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI, SD_CS);
      delay(10);

      // ---- Full manual SD SPI init diagnostic ----
      // Walk through CMD0 -> CMD8 -> ACMD41 the way the SD library does internally,
      // printing each response so we can see exactly where the card stops answering.
      {
        auto sendCmd = [](uint8_t cmd, uint32_t arg, uint8_t crc) {
          digitalWrite(SD_CS, LOW);
          SPI.transfer(0x40 | cmd);
          SPI.transfer((arg >> 24) & 0xFF);
          SPI.transfer((arg >> 16) & 0xFF);
          SPI.transfer((arg >> 8) & 0xFF);
          SPI.transfer(arg & 0xFF);
          SPI.transfer(crc);
          uint8_t r1 = 0xFF;
          for (int i = 0; i < 10 && (r1 & 0x80); i++) r1 = SPI.transfer(0xFF);
          return r1;
        };

        // >= 74 dummy clocks with CS high to enter native SPI mode
        digitalWrite(SD_CS, HIGH);
        for (int i = 0; i < 12; i++) SPI.transfer(0xFF);
        delay(2);

        // CMD0 GO_IDLE_STATE -> expect R1=0x01 (idle)
        uint8_t r1 = sendCmd(0, 0, 0x95);
        digitalWrite(SD_CS, HIGH);
        Serial.printf("SD diag: CMD0 R1=0x%02X (expect 0x01)\n", r1);

        // CMD8 SEND_IF_COND (SD v2 check): arg=0x000001AA -> R7 echoes 00 00 01 AA
        uint8_t cmd8[4] = {0};
        digitalWrite(SD_CS, LOW);
        r1 = sendCmd(8, 0x000001AA, 0x87);
        if (!(r1 & 0x80)) for (int i = 0; i < 4; i++) cmd8[i] = SPI.transfer(0xFF);
        digitalWrite(SD_CS, HIGH);
        Serial.printf("SD diag: CMD8 R1=0x%02X R7=%02X %02X %02X %02X (last=AA ok)\n",
                      r1, cmd8[0], cmd8[1], cmd8[2], cmd8[3]);

        // ACMD41 init loop (CMD55 + ACMD41). Card clears idle (R1=0x00) when ready.
        // IMPORTANT: CMD55 and ACMD41 must be in the SAME CS-low window with no CS
        // toggle between them, or the card returns illegal-command (0x05).
        bool inited = false;
        for (int i = 0; i < 50; i++) {
          digitalWrite(SD_CS, LOW);
          uint8_t r55 = sendCmd(55, 0, 0x00);
          SPI.transfer(0xFF);   // 8 dummy clocks between CMD55 and ACMD41 (CS stays low)
          uint8_t r41 = sendCmd(41, 0x40000000, 0x00);  // HCS=1 (host supports SDHC)
          digitalWrite(SD_CS, HIGH);
          if (r41 == 0x00) { inited = true; Serial.printf("SD diag: ACMD41 READY after %d tries\n", i + 1); break; }
          if (i == 0 || i == 4 || i == 9 || i == 24 || i == 49)
            Serial.printf("SD diag: ACMD41 try %d R55=0x%02X R41=0x%02X\n", i + 1, r55, r41);
          delay(20);
        }
        if (!inited) Serial.println(F("SD diag: ACMD41 never cleared idle (NOT initialized)"));
        SPI.transfer(0xFF);  // 8 clocks to release bus
      }

      // SD.begin() retries with a flag so we keep a single shared error/else branch.
      // The manual CMD0 above succeeded (R1=0x01), so the card and MISO are alive.
      // High-speed begin often fails on the shared GPIO19 RST/MISO trace, so try a
      // moderate speed first, then a conservative 1 MHz.
      bool sd_ok = SD.begin(SD_CS, SPI, 8000000);
      if (!sd_ok) {
        Serial.println(F("XiaoMiao SD: SD.begin failed @ 8MHz, retrying @ 1MHz"));
        SPI.end();
        delay(2);
        SPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI, SD_CS);
        delay(10);
        sd_ok = SD.begin(SD_CS, SPI, 1000000);
        if (!sd_ok) Serial.println(F("XiaoMiao SD: SD.begin failed @ 1MHz"));
      }
      if (!sd_ok) {
    #else
    delay(10);
    #if (defined(MARAUDER_M5STICKC)) || (defined(HAS_CYD_TOUCH)) || (defined(MARAUDER_CARDPUTER)) || (defined(MARAUDER_CARDPUTER_ADV)) || (defined(HAS_SEPARATE_SD))
      /* Set up SPI SD Card using external pin header
      StickCPlus Header - SPI SD Card Reader
                  3v3   -   3v3
                  GND   -   GND
                   G0   -   CLK
              G36/G25   -   MISO
                  G26   -   MOSI
                        -   CS (jumper to SD Card GND Pin)
      */
      #if defined(MARAUDER_M5STICKC)
        enum { SPI_SCK = 0, SPI_MISO = 36, SPI_MOSI = 26 };
      #elif defined(HAS_CYD_TOUCH) || defined(MARAUDER_CARDPUTER) || defined(MARAUDER_CARDPUTER_ADV) || defined(HAS_SEPARATE_SD)
        enum { SPI_SCK = SD_SCK, SPI_MISO = SD_MISO, SPI_MOSI = SD_MOSI };
      #else
        enum { SPI_SCK = 0, SPI_MISO = 36, SPI_MOSI = 26 };
      #endif
      #if !defined(MARAUDER_CARDPUTER) && !defined(MARAUDER_CARDPUTER_ADV)
        this->spiExt = new SPIClass();
      #else
        this->spiExt = new SPIClass(FSPI);
      #endif
      Serial.println(F("Using external SPI configuration..."));
      SPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI);
      if (!SD.begin(SD_CS, *(&SPI))) {
    #elif defined(HAS_C5_SD)
      if (!SD.begin(SD_CS, *_spi)) {
    #else
      if (!SD.begin(SD_CS)) {
    #endif
    #endif
      Serial.println(F("Failed to mount SD Card"));
      this->supported = false;
      return false;
    }
    else {
      this->supported = true;
      #ifdef MARAUDER_XIAOMIAO
        this->cardType = SD.cardType();
        this->cardSizeMB = SD.cardSize() / (1024 * 1024);
      #else
        this->cardType = SD.cardType();
        this->cardSizeMB = SD.cardSize() / (1024 * 1024);
      #endif
    
      if (this->supported) {
        const int NUM_DIGITS = log10(this->cardSizeMB) + 1;

        char sz[NUM_DIGITS + 1];

        sz[NUM_DIGITS] =  0;
        for ( size_t i = NUM_DIGITS; i--; this->cardSizeMB /= 10)
        {
            sz[i] = '0' + (this->cardSizeMB % 10);
            display_string.concat((String)sz[i]);
        }
  
        this->card_sz = sz;
      }

      if (!SD.exists("/SCRIPTS")) {

        SD.mkdir("/SCRIPTS");
      }

      this->sd_files = new LinkedList<String>();
    
      return true;
  }

  #else
    return false;
  #endif
}

File SDInterface::getFile(String path) {
  if (this->supported) {
    File file = SD.open(path, FILE_READ);
    return file;
  }
  return File();
}

bool SDInterface::removeFile(String file_path) {
  if (SD.remove(file_path))
    return true;
  else
    return false;
}

void SDInterface::listDirToLinkedList(LinkedList<String>* file_names, String str_dir, String ext) {
  if (this->supported) {
    File dir = SD.open(str_dir);
    while (true)
    {
      File entry = dir.openNextFile();
      if (!entry)
        break;
      if (entry.isDirectory())
        continue;
      
      
      String file_name = entry.name();
      if (ext != "") {
        if (file_name.endsWith(ext))
          file_names->add(file_name);
      } else
        file_names->add(file_name);
    }
  }
}

void SDInterface::listDir(String str_dir){
  if (this->supported) {
    File dir = SD.open(str_dir);
    while (true)
    {
      File entry = dir.openNextFile();
      if (!entry)
        break;
      
      
      Serial.print(entry.name());
      Serial.print("\t");
      Serial.println(entry.size());
      entry.close();
    }
  }
}

void SDInterface::runUpdate(String file_name) {
  if (file_name == "")
    file_name = "/update.bin";

  #ifdef HAS_SCREEN
    display_obj.tft.setTextWrap(false);
    display_obj.tft.setFreeFont(NULL);
    display_obj.tft.setCursor(0, SCREEN_HEIGHT / 3);
    display_obj.tft.setTextSize(1);
    display_obj.tft.setTextColor(TFT_WHITE);
  
    display_obj.tft.println("Opening " + file_name + "...");
  #endif

  File updateBin = SD.open(file_name);

  if (updateBin) {
    if(updateBin.isDirectory()){
      #ifdef HAS_SCREEN
        display_obj.tft.setTextColor(TFT_RED);
        display_obj.tft.println(F(text_table2[0]));
      #endif
      Serial.print(F("Error, could not find \""));
      Serial.print(file_name);
      Serial.println(F("\""));
      #ifdef HAS_SCREEN
        display_obj.tft.setTextColor(TFT_WHITE);
      #endif
      updateBin.close();
      return;
    }

    size_t updateSize = updateBin.size();

    if (updateSize > 0) {
      #ifdef HAS_SCREEN
        display_obj.tft.println(F(text_table2[1]));
      #endif
      Serial.println(F("Starting update over SD. Please wait..."));
      this->performUpdate(updateBin, updateSize);
    }
    else {
      #ifdef HAS_SCREEN
        display_obj.tft.setTextColor(TFT_RED);
        display_obj.tft.println(F(text_table2[2]));
      #endif
      Serial.println(F("Error, file is empty"));
      #ifdef HAS_SCREEN
        display_obj.tft.setTextColor(TFT_WHITE);
      #endif
      return;
    }

    updateBin.close();
    
      // whe finished remove the binary from sd card to indicate end of the process
    #ifdef HAS_SCREEN
      display_obj.tft.println(F(text_table2[3]));
    #endif
    const esp_partition_t *running = esp_ota_get_running_partition();

    const esp_partition_t *next = esp_ota_get_next_update_partition(NULL);

    esp_err_t result = esp_ota_set_boot_partition(next);
     
    ESP.restart();
  }
  else {
    #ifdef HAS_SCREEN
      display_obj.tft.setTextColor(TFT_RED);
      display_obj.tft.println(F(text_table2[4]));
    #endif
    Serial.println(F("Could not load update.bin from sd root"));
    #ifdef HAS_SCREEN
      display_obj.tft.setTextColor(TFT_WHITE);
    #endif
  }
}

void SDInterface::performUpdate(Stream &updateSource, size_t updateSize) {
  if (Update.begin(updateSize)) {   
    #ifdef HAS_SCREEN
      display_obj.tft.println(text_table2[5] + String(updateSize));
      display_obj.tft.println(F(text_table2[6]));
    #endif
    size_t written = Update.writeStream(updateSource);
    if (written == updateSize) {
      #ifdef HAS_SCREEN
        display_obj.tft.println(text_table2[7] + String(written) + text_table2[10]);
      #endif
      Serial.print(F("Written : "));
      Serial.print(written);
      Serial.println(F(" successfully"));
    }
    else {
      #ifdef HAS_SCREEN
        display_obj.tft.println(text_table2[8] + String(written) + "/" + String(updateSize) + text_table2[9]);
      #endif
      Serial.print(F("Written only : "));
      Serial.print(written);
      Serial.print(F("/"));
      Serial.print(updateSize);
      Serial.println(F(". Retry?"));
    }
    if (Update.end()) {
      if (Update.isFinished()) {

      }
      else {
        #ifdef HAS_SCREEN
          display_obj.tft.setTextColor(TFT_RED);
          display_obj.tft.println(text_table2[12]);
        #endif
        Serial.println(F("Update not finished? Something went wrong!"));
        #ifdef HAS_SCREEN
          display_obj.tft.setTextColor(TFT_WHITE);
        #endif
      }
    }
    else {
      #ifdef HAS_SCREEN
        display_obj.tft.println(text_table2[13] + String(Update.getError()));
      #endif
      Serial.print(F("Error Occurred. Error #: "));
      Serial.println(Update.getError());
    }

  }
  else
  {
    #ifdef HAS_SCREEN
      display_obj.tft.println(text_table2[14]);
    #endif
    Serial.println(F("Not enough space to begin OTA"));
  }
}
