#pragma once

#ifndef SDInterface_h
#define SDInterface_h

#include "configs.h"

#include "settings.h"
#ifdef HAS_C5_SD
  #include "FS.h"
#endif
#ifdef MARAUDER_XIAOMIAO
  #include "SdFat.h"
  extern SdFat SD;
  // Alias so all Marauder code using 'File' / 'FILE_READ' / 'FILE_WRITE' compiles
  // unchanged with SdFat. FsFile is SdFat's file type; O_READ/O_WRITE are its flags.
  #define File FsFile
  #define FILE_READ O_READ
  #define FILE_WRITE (O_RDWR | O_CREAT | O_TRUNC)
  #define FILE_APPEND (O_WRITE | O_APPEND | O_CREAT)
#else
  #include "SD.h"
#endif
#ifdef HAS_C5_SD
  #include "SPI.h"
#endif
#include "Buffer.h"
#ifdef HAS_SCREEN
  #include "Display.h"
#endif
#include <Update.h>

#include "esp_ota_ops.h"
#include "esp_partition.h"
#include "esp_err.h"

extern Buffer buffer_obj;
extern Settings settings_obj;
#ifdef HAS_SCREEN
  extern Display display_obj;
#endif

#ifdef KIT
  #define SD_DET 4
#endif

class SDInterface {

  private:
  #if (defined(MARAUDER_M5STICKC) || defined(HAS_CYD_TOUCH) || defined(MARAUDER_CARDPUTER) || defined(MARAUDER_CARDPUTER_ADV) || defined(HAS_SEPARATE_SD) || defined(MARAUDER_XIAOMIAO))
    SPIClass *spiExt;
  #elif defined(HAS_C5_SD)
    SPIClass* _spi;
    int _cs;
  #endif

  public:
    #ifdef HAS_C5_SD
      SDInterface(SPIClass* spi, int cs);
    #endif

    uint8_t cardType;
    //uint64_t cardSizeBT;
    //uint64_t cardSizeKB;
    uint64_t cardSizeMB;
    //uint64_t cardSizeGB;
    bool supported = false;

    String card_sz;
  
    bool initSD();

    LinkedList<String>* sd_files;

    void listDir(String str_dir);
    void listDirToLinkedList(LinkedList<String>* file_names, String str_dir = "/", String ext = "");
    File getFile(String path);
    void runUpdate(String file_name = "");
    void performUpdate(Stream &updateSource, size_t updateSize);
    bool removeFile(String file_path);
};

#endif
