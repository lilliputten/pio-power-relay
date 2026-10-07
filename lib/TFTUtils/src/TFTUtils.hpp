#ifndef TFTUtils_DEFINED
#define TFTUtils_DEFINED

#include <Arduino.h>
#include <SPI.h>
#include <TFT_eSPI.h>  // Hardware-specific library, see configuration in `User_Setup.h`

#define FONT_XS \
  1  // 26x10-12 (rows count depends on line height, see below, in range 12-10)
#define FONT_XS_SIZE 11
#define FONT_SM 2  // 20x8
#define FONT_SM_SIZE 16
#define FONT_MD 4  // 11x5
#define FONT_MD_SIZE 26
#define FONT_LG 6        // 6x3 3 lines per screen
#define FONT_LG_SIZE 40  // 3 lines per screen
#define FONT_LCD 7       // 5x2
#define FONT_LCD_SIZE 60
#define FONT_XL 8  // 3x1
#define FONT_XL_SIZE 80

class TFTUtils {
 public:
  TFT_eSPI tft = TFT_eSPI();

  TFT_eSPI& getTFT();
  void initTFT();

  TFTUtils() {
    // Constructor
    // Serial.println("TFTUtils constructor");
    this->initTFT();
  }
};

#endif
