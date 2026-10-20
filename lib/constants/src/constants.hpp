#ifndef constants_DEFINED
#define constants_DEFINED

// Serial colors...
#define RESET "\033[0m"
#define RED "\033[31m"     /* Red */
#define GREEN "\033[32m"   /* Green */
#define YELLOW "\033[33m"  /* Yellow */
#define BLUE "\033[34m"    /* Blue */
#define MAGENTA "\033[35m" /* Magenta */
#define CYAN "\033[36m"    /* Cyan */
#define BOLD "\033[1m"     /* Bold style */

// TFT Screen settings

#define TFT_IS_ROTATED (bool(TFT_ROTATION % 2))
#define TFT_SCREEN_WIDTH (TFT_IS_ROTATED ? TFT_HEIGHT : TFT_WIDTH)
#define TFT_SCREEN_HEIGHT (!TFT_IS_ROTATED ? TFT_HEIGHT : TFT_WIDTH)

// Extra TFT colors
#define TFT_GREY 0x5AEB  // New color
// #define TFT_BROWN 0x38E0

#endif  // constants_DEFINED
