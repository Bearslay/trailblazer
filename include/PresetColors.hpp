#ifndef PRESETCOLORS
#define PRESETCOLORS

#include <SDL2/SDL_pixels.h>

const SDL_Color PresetColors[16] = {
    {  0,   0,   0, 255},
    {255, 255, 255, 255},
    {170, 170, 170, 255},
    { 85,  85,  85, 255},
    {117,  60,  19, 255},
    {255,   0,   0, 255},
    {115,   0,   0, 255},
    {255, 115,   0, 255},
    {255, 255,   0, 255},
    {  0, 255,   0, 255},
    {  0, 115,   0, 255},
    {  0, 255, 255, 255},
    {  0, 115, 115, 255},
    {  0,   0, 255, 255},
    {255,   0, 255, 255},
    {115,   0, 115, 255}
};
typedef enum {
    COLOR_BLACK,
    COLOR_WHITE,
    COLOR_LIGHT_GRAY,
    COLOR_DARK_GRAY,
    COLOR_BROWN,
    COLOR_RED,
    COLOR_MAROON,
    COLOR_ORANGE,
    COLOR_YELLOW,
    COLOR_LIME,
    COLOR_GREEN ,
    COLOR_CYAN ,
    COLOR_TEAL ,
    COLOR_BLUE ,
    COLOR_MAGENTA,
    COLOR_PURPLE
} PresetColorsNames;

#endif /* PRESETCOLORS */
