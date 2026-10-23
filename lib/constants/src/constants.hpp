#ifndef constants_DEFINED
#define constants_DEFINED

// Module usage...

// Serial (terminal) colors...
#define RESET "\033[0m"
#define RED "\033[31m"     /* Red */
#define GREEN "\033[32m"   /* Green */
#define YELLOW "\033[33m"  /* Yellow */
#define BLUE "\033[34m"    /* Blue */
#define MAGENTA "\033[35m" /* Magenta */
#define CYAN "\033[36m"    /* Cyan */
#define BOLD "\033[1m"     /* Bold style */

// TFT Screen settings

#define TPos uint16_t

#define TFT_IS_ROTATED (bool(TFT_ROTATION % 2))
#define TFT_SCREEN_WIDTH (TFT_IS_ROTATED ? TFT_HEIGHT : TFT_WIDTH)
#define TFT_SCREEN_HEIGHT (!TFT_IS_ROTATED ? TFT_HEIGHT : TFT_WIDTH)

// Extra TFT colors
#define TFT_GREY 0x5AEB  // New color
// #define TFT_BROWN 0x38E0
// Default colors should be redefined too: theere is a mess (see notations in
// the `serial-colors.css`)

// TFT Fonts

#define TFont uint8_t
#define TColor uint32_t

// Rows count depends on line height, see below, in range 12-16

#define FONT_XS 1  // 26x16
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

#endif  // constants_DEFINED
