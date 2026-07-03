#include "SDInterface.h"
#include "lang_var.h"
#ifdef MARAUDER_XIAOMIAO
  #include "SdFat.h"
  SdFat SD;
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

    // XiaoMiao: SD shares SPI2 with the TFT (SCK=18, MOSI=23, MISO=19, SD CS=22).
    // There is NO hardware TFT reset line — TFT_RST is -1, so GPIO19 is a dedicated
    // MISO and the two devices time-share the bus via CS only. This mirrors the
    // retro-go-for-xueersi-xiaomiao port, which reads this SD cleanly.
    #ifdef MARAUDER_XIAOMIAO
      // SdFat 2.2.0 + SHARED_SPI works on arduino-esp32 2.x (the 3.x core's SPIClass
      // crashes SdFat on this shared bus, so this path only runs on the 2.x build).
      // Deselect both SPI devices before bus init so neither holds MISO low.
      pinMode(TFT_CS, OUTPUT);
      digitalWrite(TFT_CS, HIGH);
      pinMode(SD_CS, OUTPUT);
      digitalWrite(SD_CS, HIGH);
      delay(5);
      SPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI, SD_CS);
      delay(10);
      // Try the full bus speed first; on failure fall back to a slow init speed
      // (mirrors retro-go rg_storage.c: retry at SDMMC_FREQ_PROBING on TIMEOUT/CRC).
      Serial.println(F("XiaoMiao SD: SdFat SHARED_SPI init @20MHz..."));
      bool sd_ok = SD.begin(SdSpiConfig(SD_CS, SHARED_SPI, SD_SCK_MHZ(20), &SPI));
      if (!sd_ok) {
        Serial.println(F("XiaoMiao SD: 20MHz failed, retry @400kHz..."));
        delay(10);
        sd_ok = SD.begin(SdSpiConfig(SD_CS, SHARED_SPI, SD_SCK_MHZ(0.4), &SPI));
      }
      if (!sd_ok) {
        Serial.println(F("XiaoMiao SD: SdFat init FAILED"));
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
      SPI.begin(TFT_SCLK, TFT_MISO, TFT_MOSI, SD_CS);
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
        this->cardType = SD.card()->type();
        this->cardSizeMB = (SD.card()->sectorCount() * 512) / (1024 * 1024);
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
      char namebuf[64];
      entry.getName(namebuf, sizeof(namebuf));
      String file_name = String(namebuf);
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
      char namebuf[64];
      entry.getName(namebuf, sizeof(namebuf));
      Serial.print(namebuf);
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
        display_obj.tft.println(LT2(0));
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
        display_obj.tft.println(LT2(1));
      #endif
      Serial.println(F("Starting update over SD. Please wait..."));
      this->performUpdate(updateBin, updateSize);
    }
    else {
      #ifdef HAS_SCREEN
        display_obj.tft.setTextColor(TFT_RED);
        display_obj.tft.println(LT2(2));
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
      display_obj.tft.println(LT2(3));
    #endif
    const esp_partition_t *running = esp_ota_get_running_partition();

    const esp_partition_t *next = esp_ota_get_next_update_partition(NULL);

    esp_err_t result = esp_ota_set_boot_partition(next);
     
    ESP.restart();
  }
  else {
    #ifdef HAS_SCREEN
      display_obj.tft.setTextColor(TFT_RED);
      display_obj.tft.println(LT2(4));
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
      display_obj.tft.println(LT2(5) + String(updateSize));
      display_obj.tft.println(LT2(6));
    #endif
    size_t written = Update.writeStream(updateSource);
    if (written == updateSize) {
      #ifdef HAS_SCREEN
        display_obj.tft.println(LT2(7) + String(written) + LT2(10));
      #endif
      Serial.print(F("Written : "));
      Serial.print(written);
      Serial.println(F(" successfully"));
    }
    else {
      #ifdef HAS_SCREEN
        display_obj.tft.println(LT2(8) + String(written) + "/" + String(updateSize) + LT2(9));
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
          display_obj.tft.println(LT2(12));
        #endif
        Serial.println(F("Update not finished? Something went wrong!"));
        #ifdef HAS_SCREEN
          display_obj.tft.setTextColor(TFT_WHITE);
        #endif
      }
    }
    else {
      #ifdef HAS_SCREEN
        display_obj.tft.println(LT2(13) + String(Update.getError()));
      #endif
      Serial.print(F("Error Occurred. Error #: "));
      Serial.println(Update.getError());
    }

  }
  else
  {
    #ifdef HAS_SCREEN
      display_obj.tft.println(LT2(14));
    #endif
    Serial.println(F("Not enough space to begin OTA"));
  }
}
