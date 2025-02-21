// created: 10.02.2022
// updated: 20.12.2022

#pragma once

#define _SSID               "mySSID"                        // Your WiFi credentials here
#define _PW                 "myWiFiPassword"
#define TZName              "MST7MDT,M3.2.0,M11.1.0"    // Timezone (more TZNames in "rtime.cpp")
#define DECODER             1                               // (0)VS1053 , (1)MAX98357A PCM5102A... (2)AC101 (3)ES8388 (4)WM8978
#define TFT_CONTROLLER      4                               // (0)ILI9341, (1)HX8347D, (2)ILI9486a, (3)ILI9486b, (4)ILI9488
#define DISPLAY_INVERSION   0                               // (0) off (1) on
#define TFT_FREQUENCY       80000000                        // 27000000, 40000000, 80000000
#define TFT_ROTATION        3                               // 1 or 3 (landscape)
#define TP_VERSION          5                               // (0)ILI9341, (1)ILI9341RPI, (2)HX8347D, (3)ILI9486, (4)ILI9488, (5) I2C FT6236
#define TP_ROTATION         3                               // 1 or 3 (landscape)
#define AUDIOTASK_CORE      1                               // 0 or 1
#define AUDIOTASK_PRIO      2                               // 0 ... 24  Priority of the Task (0...configMAX_PRIORITIES -1)
#define SDMMC_FREQUENCY     20000000                        // 40000000, 2000000, 10000000, not every SD Card will run at 40MHz
#define FTP_USERNAME        "esp32"                         // user and pw in FTP Client
#define FTP_PASSWORD        "esp32"
#define GCORE_HW            1                               // 0 or 1 (gCore platform)

/**********************************************************************************************************************/

#include <Arduino.h>
#include <Preferences.h>
#include <Ticker.h>
#include <SPI.h>
#include <SD_MMC.h>
#include <FS.h>
#include <Wire.h>
#include <WiFiClient.h>
#include <WiFiMulti.h>
#include "index.h"
#include "websrv.h"
#include "rtime.h"
#include "IR.h"
#include "tft.h"
#include "ESP32FtpServer.h"
#include "AC101.h"
#include "ES8388.h"
#include "WM8978.h"
#include "SoapESP32.h"
#include "Arduino_JSON.h"

#ifdef CONFIG_IDF_TARGET_ESP32
    // Digital I/O used
        #define TFT_CS         5
        #define TFT_DC        27
        #define TFT_BL        32  // at -1 the brightness menu is not displayed (ignored for TFT_CONTROLLER=5)
        #define TP_IRQ        -1  // VN
        #define TP_CS         -1  // Set to -1 for no SPI TP
        #define SD_MMC_D0      2  // cannot be changed
        #define SD_MMC_CLK    14  // cannot be changed
        #define SD_MMC_CMD    15  // cannot be changed
        #define IR_PIN        -1  // Set to -1 for no IR
        #define TFT_MOSI      23  // TFT and SPI TP (VSPI)
        #define TFT_MISO      -1  // TFT and SPI TP (VSPI)
        #define TFT_SCK       18  // TFT and SPI TP (VSPI)
    #if DECODER == 0
        #define VS1053_CS     33
        #define VS1053_DCS     4
        #define VS1053_DREQ   36
        #define VS1053_MOSI   13  // VS1053     (HSPI)
        #define VS1053_MISO   34  // VS1053     (HSPI)
        #define VS1053_SCK    12  // VS1053     (HSPI) (sometimes we need a 1k resistor against ground)
    #else
        #define I2S_DOUT      25
        #define I2S_DIN       -1  // pin not used
        #define I2S_BCLK      32
        #define I2S_LRC       26
        #define I2S_MCLK       0  // mostly not used
    #endif
        #define I2C_DATA      21  // some DACs or touchscreen controllers are controlled via I2C
        #define I2C_CLK       22
        #define SD_DETECT     -1  // some pins on special boards: Lyra, Olimex, A1S ...
        #define HP_DETECT     -1
        #define AMP_ENABLED   -1
#endif

#ifdef CONFIG_IDF_TARGET_ESP32S3
    // Digital I/O used
        #define TFT_CS         8
        #define TFT_DC        12
        #define TFT_BL        10 // at -1 the brightness menu is not displayed
        #define TP_IRQ        39
        #define TP_CS         15
        #define SD_MMC_D0     11
        #define SD_MMC_CLK    13
        #define SD_MMC_CMD    14
        #define IR_PIN        33
        #define TFT_MOSI      18  // TFT and TP (FSPI)
        #define TFT_MISO       2  // TFT and TP (FSPI)
        #define TFT_SCK       17  // TFT and TP (FSPI)
    #if DECODER == 0
        #define VS1053_CS     48
        #define VS1053_DCS    16
        #define VS1053_DREQ   38
        #define VS1053_MOSI   20  // VS1053     (HSPI)
        #define VS1053_MISO   19  // VS1053     (HSPI)
        #define VS1053_SCK    21  // VS1053     (HSPI) (sometimes we need a 1k resistor against ground)
    #else
        #define I2S_DOUT       9
        #define I2S_DIN       -1
        #define I2S_BCLK       3
        #define I2S_LRC        1
        #define I2S_MCLK      -1
    #endif
        #define I2C_DATA      -1  // some DACs are controlled via I2C
        #define I2C_CLK       -1
        #define SD_DETECT     -1  // some pins on special boards: Lyra, Olimex, A1S ...
        #define HP_DETECT     -1
        #define AMP_ENABLED   -1
#endif

/**********************************************************************************************************************/
// output on serial terminal
#define ANSI_ESC_BLACK      "\033[30m"
#define ANSI_ESC_RED        "\033[31m"
#define ANSI_ESC_GREEN      "\033[32m"
#define ANSI_ESC_YELLOW     "\033[33m"
#define ANSI_ESC_BLUE       "\033[34m"
#define ANSI_ESC_MAGENTA    "\033[35m"
#define ANSI_ESC_CYAN       "\033[36m"
#define ANSI_ESC_WHITE      "\033[37m"
#define ANSI_ESC_RESET      "\033[0m"
#define ANSI_ESC_BROWN      "\033[38;5;130m"
#define ANSI_ESC_ORANGE     "\033[38;5;214m"

#define SerialPrintfln(...) {xSemaphoreTake(mutex_rtc, portMAX_DELAY); \
                            Serial.printf("%s ", rtc.gettime_s()); \
                            Serial.printf(__VA_ARGS__); \
                            Serial.printf("\033[0m"); \
                            Serial.println(""); \
                            xSemaphoreGive(mutex_rtc);}

/**********************************************************************************************************************/

// //prototypes (main.cpp)
boolean defaultsettings();
boolean saveStationsToNVS();
void setTFTbrightness(uint8_t duty);
const char* UTF8toASCII(const char* str);
const char* ASCIItoUTF8(const char* str);
void showHeadlineVolume();
void showHeadlineTime(bool complete = true);
void showHeadlineItem(uint8_t idx);
void showFooterIPaddr();
void showFooterStaNr();
void showFooterRSSI();
void updateSleepTime(boolean noDecrement = false);
void showVolumeBar();
void showBrightnessBar();
void showFooter();
void display_info(const char *str, int xPos, int yPos, uint16_t color, uint16_t indent, uint16_t winHeight);
void showStreamTitle(const char* streamTitle);
void showLogoAndStationName();
void showFileName(const char* fname);
void display_time(boolean showall = false);
void display_alarmDays(uint8_t ad, boolean showall=false);
void display_alarmtime(int8_t xy = 0, int8_t ud = 0, boolean showall = false);
void display_sleeptime(int8_t ud = 0);
boolean drawImage(const char* path, uint16_t posX, uint16_t posY, uint16_t maxWidth = 0 , uint16_t maxHeigth = 0);
bool setAudioFolder(const char* audioDir);
const char* listAudioFile();
bool sendAudioList2Web(const char* audioDir);
bool connectToWiFi();
const char* byte_to_binary(int8_t x);
bool startsWith (const char* base, const char* str);
bool endsWith (const char* base, const char* str);
int indexOf (const char* base, const char* str, int startIndex);
boolean strCompare(char* str1, char* str2);
boolean strCompare(const char* str1, char* str2);
void SerialPrintflnCut(const char* item, const char* color, const char* str);
const char* scaleImage(const char* path);
void setVolume(uint8_t vol);
inline uint8_t getvolume();
uint8_t downvolume();
uint8_t upvolume();
void setStation(uint16_t sta);
void nextStation();
void prevStation();
void StationsItems();
void setStationViaURL(const char* url);
void changeBtn_pressed(uint8_t btnNr);
void changeBtn_released(uint8_t btnNr);
void savefile(const char* fileName, uint32_t contentLength);
String setTone();
String setI2STone();
void audiotrack(const char* fileName, uint32_t resumeFilePos = 0);
void processPlaylist(boolean first = false);
void changeState(int state);
void connecttohost(const char* host);
void connecttoFS(const char* filename, uint32_t resumeFilePos = 0);
void stopSong();
void IRAM_ATTR headphoneDetect();

//prototypes (audiotask.cpp)
void audioInit();
void audioSetVolume(uint8_t vol);
uint8_t audioGetVolume();
boolean audioConnecttohost(const char* host, const char* user = "", const char* pwd = "");
boolean audioConnecttoFS(const char* filename, uint32_t resumeFilePos = 0);
uint32_t audioStopSong();
void audioSetTone(int8_t param0, int8_t param1, int8_t param2, int8_t param3 = 0);
uint32_t audioInbuffFilled();
uint32_t audioInbuffFree();
boolean audioIsRunning();
uint32_t audioGetStackHighWatermark();

