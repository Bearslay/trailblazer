#include "RenderWindow.hpp"

RenderWindow::RenderWindow(const char* title, const int &w, const int &h, Uint32 flags) : Window(NULL), Renderer(NULL), W(w), H(h), W_2(w / 2), H_2(h / 2) {
    if ((Window = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, w, h, flags)) == NULL) {std::cout << "Window \"" << title << "\" failed to initialize\nERROR: " << SDL_GetError() << "\n";}
    if ((Renderer = SDL_CreateRenderer(Window, -1, SDL_RENDERER_ACCELERATED)) == NULL) {std::cout << "Renderer for \"" << title << "\" failed to initialize\nERROR: " << SDL_GetError() << "\n";}
}
RenderWindow::~RenderWindow() {
    SDL_DestroyRenderer(Renderer);
    SDL_DestroyWindow(Window);
}

int RenderWindow::getRefreshRate() const {
    SDL_DisplayMode mode;
    SDL_GetDisplayMode(SDL_GetWindowDisplayIndex(Window), 0, &mode);
    return mode.refresh_rate;
}
Uint32 RenderWindow::getWindowFlags() {return SDL_GetWindowFlags(Window);}

int RenderWindow::getW() const {return W;}
int RenderWindow::setW(const int &w) {
    const int output = W;
    W = w;
    W_2 = W / 2;
    SDL_SetWindowSize(Window, W, H);
    return output;
}
int RenderWindow::adjustW(const int &amount) {
    const int output = W;
    W += amount;
    W_2 = W / 2;
    SDL_SetWindowSize(Window, W, H);
    return output;
}
int RenderWindow::getH() const {return H;}
int RenderWindow::setH(const int &h) {
    const int output = H;
    H = h;
    H_2 = H / 2;
    SDL_SetWindowSize(Window, W, H);
    return output;
}
int RenderWindow::adjustH(const int &amount) {
    const int output = W;
    H += amount;
    H_2 = H / 2;
    SDL_SetWindowSize(Window, W, H);
    return output;
}
SDL_Point RenderWindow::getDims() const {return {W, H};}
SDL_Point RenderWindow::setDims(const int &w, const int &h) {
    const SDL_Point output = {W, H};
    W = w;
    W_2 = W / 2;
    H = h;
    H_2 = H / 2;
    SDL_SetWindowSize(Window, W, H);
    return output;
}
SDL_Point RenderWindow::adjustDims(const int &w, const int &h) {
    const SDL_Point output = {W, H};
    W += w;
    W_2 = W / 2;
    H += h;
    H_2 = H / 2;
    SDL_SetWindowSize(Window, W, H);
    return output;
}
SDL_Point RenderWindow::updateDims() {
    const SDL_Point output = {W, H};
    SDL_GetWindowSize(Window, &W, &H);
    W_2 = W / 2;
    H_2 = H / 2;
    return output;
}
int RenderWindow::getW_2() const {return W_2;}
int RenderWindow::getH_2() const {return H_2;}

const char* RenderWindow::getTitle() const {return SDL_GetWindowTitle(Window);}
const char* RenderWindow::setTitle(const char* title) {
    const char* output = getTitle();
    SDL_SetWindowTitle(Window, title);
    return output;
}

void RenderWindow::clear(const SDL_Color &color) {
    SDL_SetRenderDrawColor(Renderer, color.r, color.g, color.b, color.a);
    SDL_RenderClear(Renderer);
}
void RenderWindow::show() {SDL_RenderPresent(Renderer);}

bool RenderWindow::toggleFullscreen(const bool &trueFullscreen) {
    const bool output = IsFullscreen;
    if   (!IsFullscreen) {SDL_SetWindowFullscreen(Window, trueFullscreen ? SDL_WINDOW_FULLSCREEN : SDL_WINDOW_FULLSCREEN_DESKTOP);}
    else                 {SDL_SetWindowFullscreen(Window, SDL_FALSE);}
    IsFullscreen = !IsFullscreen;
    updateDims();
    return output;
}
void RenderWindow::centerMouse() {SDL_WarpMouseInWindow(Window, W_2, H_2);}

void RenderWindow::handleEvent(const SDL_WindowEvent &event) {
    switch (event.event) {
        case SDL_WINDOWEVENT_RESIZED:
        case SDL_WINDOWEVENT_SIZE_CHANGED:
            updateDims();
            break;
    }
}

void RenderWindow::drawPixel(const int &x, const int &y, const SDL_Color &color) {
    SDL_SetRenderDrawColor(Renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawPoint(Renderer, W_2 + x, H_2 - y);
}
void RenderWindow::drawLine(const int &x1, const int &y1, const int &x2, const int &y2, const SDL_Color &color) {
    SDL_SetRenderDrawColor(Renderer, color.r, color.g, color.b, color.a);
    SDL_RenderDrawLine(Renderer, W_2 + x1, H_2 - y1, W_2 + x2, H_2 - y2);
}
void RenderWindow::drawThickLine(const int &x1, const int &y1, const int &x2, const int &y2, const int &thickness, const SDL_Color &color = PresetColors[COLOR_WHITE]) {
    SDL_SetRenderDrawColor(Renderer, color.r, color.g, color.b, color.a);

    // Code and stuff (see below)
}
void RenderWindow::drawRectangle(const int &x, const int &y, const int &w, const int &h, const SDL_Color &color) {
    SDL_SetRenderDrawColor(Renderer, color.r, color.g, color.b, color.a);
    SDL_Rect dst = {W_2 + x, H_2 - y, w, h};
    SDL_RenderDrawRect(Renderer, &dst);
}
void RenderWindow::fillRectangle(const int &x, const int &y, const int &w, const int &h, const SDL_Color &color) {
    SDL_SetRenderDrawColor(Renderer, color.r, color.g, color.b, color.a);
    SDL_Rect dst = {W_2 + x, H_2 - y, w, h};
    SDL_RenderFillRect(Renderer, &dst);
}
void RenderWindow::drawCircle(const int &x, const int &y, const int &r, const SDL_Color &color) {
    SDL_SetRenderDrawColor(Renderer, color.r, color.g, color.b, color.a);
    const int diameter = r * 2;
    int ox    = r - 1;    int oy = 0;
    int tx    = 1;        int ty = 1;
    int error = tx - diameter;
    while (ox >= oy) {
        SDL_RenderDrawPoint(Renderer, W_2 + x + ox, H_2 - y - oy);
        SDL_RenderDrawPoint(Renderer, W_2 + x + ox, H_2 - y + oy);
        SDL_RenderDrawPoint(Renderer, W_2 + x - ox, H_2 - y - oy);
        SDL_RenderDrawPoint(Renderer, W_2 + x - ox, H_2 - y + oy);
        SDL_RenderDrawPoint(Renderer, W_2 + x + oy, H_2 - y - ox);
        SDL_RenderDrawPoint(Renderer, W_2 + x + oy, H_2 - y + ox);
        SDL_RenderDrawPoint(Renderer, W_2 + x - oy, H_2 - y - ox);
        SDL_RenderDrawPoint(Renderer, W_2 + x - oy, H_2 - y + ox);
        if (error <= 0) {
            oy++;
            error += ty;
            ty    += 2;
        } else if (error > 0) {
            ox--;
            tx    += 2;
            error += tx - diameter;
        }
    }
}
void RenderWindow::fillCircle(const int &x, const int &y, const int &r, const SDL_Color &color) {
    SDL_SetRenderDrawColor(Renderer, color.r, color.g, color.b, color.a);
    int ox    = 0;    int oy = r;
    int error = r - 1;
    while (oy >= ox) {
        SDL_RenderDrawLine(Renderer, W_2 + x - oy, H_2 - y + ox, W_2 + x + oy, H_2 - y + ox);
        SDL_RenderDrawLine(Renderer, W_2 + x - ox, H_2 - y + oy, W_2 + x + ox, H_2 - y + oy);
        SDL_RenderDrawLine(Renderer, W_2 + x - ox, H_2 - y - oy, W_2 + x + ox, H_2 - y - oy);
        SDL_RenderDrawLine(Renderer, W_2 + x - oy, H_2 - y - ox, W_2 + x + oy, H_2 - y - ox);
        if (error >= ox * 2) {
            error -= ox * 2 + 1;
            ox++;
        } else if (error < 2 * (r - oy)) {
            error += oy * 2 - 1;
            oy--;;
        } else {
            error += 2 * (oy - ox - 1);
            oy--;
            ox++;
        }
    }
}

SDL_Texture* RenderWindow::loadTexture(const std::string &path) {
    SDL_Texture* output = NULL;
    if ((output = IMG_LoadTexture(Renderer, path.c_str())) == NULL) {std::cout << "Failed to load texture\nERROR: " << SDL_GetError() << "\n";}
    return output;
}

void RenderWindow::renderTexture(SDL_Texture* texture, const SDL_Rect &src, const SDL_Rect &dst) {
    const SDL_Rect destination = {W_2 + dst.x, H_2 - dst.y, dst.w, dst.h};
    SDL_RenderCopy(Renderer, texture, &src, &destination);
}
void RenderWindow::renderTexture(SDL_Texture* texture, const SDL_Rect &src, const SDL_Rect &dst, const double &angle, const SDL_Point &center, const SDL_RendererFlip &flip) {
    const SDL_Rect destination = {W_2 + dst.x, H_2 - dst.y, dst.w, dst.h};
    SDL_RenderCopyEx(Renderer, texture, &src, &destination, angle, &center, flip);
}
void RenderWindow::renderTexture(const Texture &texture, const SDL_Rect &dst) {
    const SDL_Rect source = texture.getFrame();
    const SDL_Rect destination = {W_2 + dst.x, H_2 - dst.y, dst.w, dst.h};
    const SDL_Point center = texture.getCenter();
    SDL_RenderCopyEx(Renderer, texture.getTexture(), &source, &destination, -texture.getAngle() * 180 / M_PI, &center, texture.getFlip());
}
void RenderWindow::renderTexture(const Texture &texture, const SDL_Point &pos) {
    const SDL_Rect src = texture.getFrame();
    const SDL_Rect dst = {W_2 + pos.x, H_2 - pos.y, texture.getFrame().w, texture.getFrame().h};
    const SDL_Point center = texture.getCenter();
    SDL_RenderCopyEx(Renderer, texture.getTexture(), &src, &dst, -texture.getAngle() * 180 / M_PI, &center, texture.getFlip());
}
void RenderWindow::renderTexture(const Texture &texture, const int &x, const int &y) {
    const SDL_Point pos = {x, y};
    renderTexture(texture, pos);
}

void RenderWindow::renderText(TTF_Font *font, const char16_t* text, const SDL_Point &pos, const Uint32 wrapWidth, const SDL_Color &color) {
    SDL_Surface *surface = TTF_RenderUNICODE_Blended_Wrapped(font, (Uint16*)text, color, wrapWidth);
    renderTexture(SDL_CreateTextureFromSurface(Renderer, surface), {0, 0, surface->w, surface->h}, {pos.x - surface->w / 2, pos.y + surface->h / 2, surface->w, surface->h}, 0, {0, 0}, SDL_FLIP_NONE);
    SDL_FreeSurface(surface);
}

// #define LINE_OVERLAP_NONE 0 	// No line overlap, like in standard Bresenham
// #define LINE_OVERLAP_MAJOR 0x01 // Overlap - first go major then minor direction. Pixel is drawn as extension after actual line
// #define LINE_OVERLAP_MINOR 0x02 // Overlap - first go minor then major direction. Pixel is drawn as extension before next line
// #define LINE_OVERLAP_BOTH 0x03  // Overlap - both

// #define LINE_THICKNESS_MIDDLE 0                 // Start point is on the line at center of the thick line
// #define LINE_THICKNESS_DRAW_CLOCKWISE 1         // Start point is on the counter clockwise border line
// #define LINE_THICKNESS_DRAW_COUNTERCLOCKWISE 2  // Start point is on the clockwise border line

// /**
//  * Draws a line from aXStart/aYStart to aXEnd/aYEnd including both ends
//  * @param aOverlap One of LINE_OVERLAP_NONE, LINE_OVERLAP_MAJOR, LINE_OVERLAP_MINOR, LINE_OVERLAP_BOTH
//  */
// void drawLineOverlap(unsigned int aXStart, unsigned int aYStart, unsigned int aXEnd, unsigned int aYEnd, uint8_t aOverlap, uint16_t aColor) {
//     int16_t tDeltaX, tDeltaY, tDeltaXTimes2, tDeltaYTimes2, tError, tStepX, tStepY;

//     /*
//      * Clip to display size
//      */
//     if (aXStart >= LOCAL_DISPLAY_WIDTH) {
//         aXStart = LOCAL_DISPLAY_WIDTH - 1;
//     }

//     if (aXEnd >= LOCAL_DISPLAY_WIDTH) {
//         aXEnd = LOCAL_DISPLAY_WIDTH - 1;
//     }

//     if (aYStart >= LOCAL_DISPLAY_HEIGHT) {
//         aYStart = LOCAL_DISPLAY_HEIGHT - 1;
//     }

//     if (aYEnd >= LOCAL_DISPLAY_HEIGHT) {
//         aYEnd = LOCAL_DISPLAY_HEIGHT - 1;
//     }

//     if ((aXStart == aXEnd) || (aYStart == aYEnd)) {
//         // horizontal or vertical line -> fillRect() is faster than drawLine()
//         fillRect(aXStart, aYStart, aXEnd, aYEnd, aColor); // you can remove the check and this line if you have no fillRect() or drawLine() available.
//     } else {
//         // calculate direction
//         tDeltaX = aXEnd - aXStart;
//         tDeltaY = aYEnd - aYStart;
//         if (tDeltaX < 0) {
//             tDeltaX = -tDeltaX;
//             tStepX = -1;
//         } else {
//             tStepX = +1;
//         }
//         if (tDeltaY < 0) {
//             tDeltaY = -tDeltaY;
//             tStepY = -1;
//         } else {
//             tStepY = +1;
//         }
//         tDeltaXTimes2 = tDeltaX << 1;
//         tDeltaYTimes2 = tDeltaY << 1;
//         // draw start pixel
//         drawPixel(aXStart, aYStart, aColor);
//         if (tDeltaX > tDeltaY) {
//             // start value represents a half step in Y direction
//             tError = tDeltaYTimes2 - tDeltaX;
//             while (aXStart != aXEnd) {
//                 // step in main direction
//                 aXStart += tStepX;
//                 if (tError >= 0) {
//                     if (aOverlap & LINE_OVERLAP_MAJOR) {
//                         // draw pixel in main direction before changing
//                         drawPixel(aXStart, aYStart, aColor);
//                     }
//                     // change Y
//                     aYStart += tStepY;
//                     if (aOverlap & LINE_OVERLAP_MINOR) {
//                         // draw pixel in minor direction before changing
//                         drawPixel(aXStart - tStepX, aYStart, aColor);
//                     }
//                     tError -= tDeltaXTimes2;
//                 }
//                 tError += tDeltaYTimes2;
//                 drawPixel(aXStart, aYStart, aColor);
//             }
//         } else {
//             tError = tDeltaXTimes2 - tDeltaY;
//             while (aYStart != aYEnd) {
//                 aYStart += tStepY;
//                 if (tError >= 0) {
//                     if (aOverlap & LINE_OVERLAP_MAJOR) {
//                         // draw pixel in main direction before changing
//                         drawPixel(aXStart, aYStart, aColor);
//                     }
//                     aXStart += tStepX;
//                     if (aOverlap & LINE_OVERLAP_MINOR) {
//                         // draw pixel in minor direction before changing
//                         drawPixel(aXStart, aYStart - tStepY, aColor);
//                     }
//                     tError -= tDeltaYTimes2;
//                 }
//                 tError += tDeltaXTimes2;
//                 drawPixel(aXStart, aYStart, aColor);
//             }
//         }
//     }
// }

// /**
//  * Bresenham with thickness
//  * No pixel missed and every pixel only drawn once!
//  * The code is bigger and more complicated than drawThickLineSimple() but it tends to be faster, since drawing a pixel is often a slow operation.
//  * aThicknessMode can be one of LINE_THICKNESS_MIDDLE, LINE_THICKNESS_DRAW_CLOCKWISE, LINE_THICKNESS_DRAW_COUNTERCLOCKWISE
//  */
// void drawThickLine(unsigned int aXStart, unsigned int aYStart, unsigned int aXEnd, unsigned int aYEnd, unsigned int aThickness,
//         uint8_t aThicknessMode, uint16_t aColor) {
//     int16_t i, tDeltaX, tDeltaY, tDeltaXTimes2, tDeltaYTimes2, tError, tStepX, tStepY;

//     if (aThickness <= 1) {
//         drawLineOverlap(aXStart, aYStart, aXEnd, aYEnd, LINE_OVERLAP_NONE, aColor);
//     }
//     /*
//      * Clip to display size
//      */
//     if (aXStart >= LOCAL_DISPLAY_WIDTH) {
//         aXStart = LOCAL_DISPLAY_WIDTH - 1;
//     }

//     if (aXEnd >= LOCAL_DISPLAY_WIDTH) {
//         aXEnd = LOCAL_DISPLAY_WIDTH - 1;
//     }

//     if (aYStart >= LOCAL_DISPLAY_HEIGHT) {
//         aYStart = LOCAL_DISPLAY_HEIGHT - 1;
//     }

//     if (aYEnd >= LOCAL_DISPLAY_HEIGHT) {
//         aYEnd = LOCAL_DISPLAY_HEIGHT - 1;
//     }

//     /**
//      * For coordinate system with 0.0 top left
//      * Swap X and Y delta and calculate clockwise (new delta X inverted)
//      * or counterclockwise (new delta Y inverted) rectangular direction.
//      * The right rectangular direction for LINE_OVERLAP_MAJOR toggles with each octant
//      */
//     tDeltaY = aXEnd - aXStart;
//     tDeltaX = aYEnd - aYStart;
//     // mirror 4 quadrants to one and adjust deltas and stepping direction
//     bool tSwap = true; // count effective mirroring
//     if (tDeltaX < 0) {
//         tDeltaX = -tDeltaX;
//         tStepX = -1;
//         tSwap = !tSwap;
//     } else {
//         tStepX = +1;
//     }
//     if (tDeltaY < 0) {
//         tDeltaY = -tDeltaY;
//         tStepY = -1;
//         tSwap = !tSwap;
//     } else {
//         tStepY = +1;
//     }
//     tDeltaXTimes2 = tDeltaX << 1;
//     tDeltaYTimes2 = tDeltaY << 1;
//     bool tOverlap;
//     // adjust for right direction of thickness from line origin
//     int tDrawStartAdjustCount = aThickness / 2;
//     if (aThicknessMode == LINE_THICKNESS_DRAW_COUNTERCLOCKWISE) {
//         tDrawStartAdjustCount = aThickness - 1;
//     } else if (aThicknessMode == LINE_THICKNESS_DRAW_CLOCKWISE) {
//         tDrawStartAdjustCount = 0;
//     }

//     /*
//      * Now tDelta* are positive and tStep* define the direction
//      * tSwap is false if we mirrored only once
//      */
//     // which octant are we now
//     if (tDeltaX >= tDeltaY) {
//         // Octant 1, 3, 5, 7 (between 0 and 45, 90 and 135, ... degree)
//         if (tSwap) {
//             tDrawStartAdjustCount = (aThickness - 1) - tDrawStartAdjustCount;
//             tStepY = -tStepY;
//         } else {
//             tStepX = -tStepX;
//         }
//         /*
//          * Vector for draw direction of the starting points of lines is rectangular and counterclockwise to main line direction
//          * Therefore no pixel will be missed if LINE_OVERLAP_MAJOR is used on change in minor rectangular direction
//          */
//         // adjust draw start point
//         tError = tDeltaYTimes2 - tDeltaX;
//         for (i = tDrawStartAdjustCount; i > 0; i--) {
//             // change X (main direction here)
//             aXStart -= tStepX;
//             aXEnd -= tStepX;
//             if (tError >= 0) {
//                 // change Y
//                 aYStart -= tStepY;
//                 aYEnd -= tStepY;
//                 tError -= tDeltaXTimes2;
//             }
//             tError += tDeltaYTimes2;
//         }
//         // draw start line. We can alternatively use drawLineOverlap(aXStart, aYStart, aXEnd, aYEnd, LINE_OVERLAP_NONE, aColor) here.
//         drawLine(aXStart, aYStart, aXEnd, aYEnd, aColor);
//         // draw aThickness number of lines
//         tError = tDeltaYTimes2 - tDeltaX;
//         for (i = aThickness; i > 1; i--) {
//             // change X (main direction here)
//             aXStart += tStepX;
//             aXEnd += tStepX;
//             tOverlap = LINE_OVERLAP_NONE;
//             if (tError >= 0) {
//                 // change Y
//                 aYStart += tStepY;
//                 aYEnd += tStepY;
//                 tError -= tDeltaXTimes2;
//                 /*
//                  * Change minor direction reverse to line (main) direction
//                  * because of choosing the right (counter)clockwise draw vector
//                  * Use LINE_OVERLAP_MAJOR to fill all pixel
//                  *
//                  * EXAMPLE:
//                  * 1,2 = Pixel of first 2 lines
//                  * 3 = Pixel of third line in normal line mode
//                  * - = Pixel which will additionally be drawn in LINE_OVERLAP_MAJOR mode
//                  *           33
//                  *       3333-22
//                  *   3333-222211
//                  * 33-22221111
//                  *  221111                     /\
//                  *  11                          Main direction of start of lines draw vector
//                  *  -> Line main direction
//                  *  <- Minor direction of counterclockwise of start of lines draw vector
//                  */
//                 tOverlap = LINE_OVERLAP_MAJOR;
//             }
//             tError += tDeltaYTimes2;
//             drawLineOverlap(aXStart, aYStart, aXEnd, aYEnd, tOverlap, aColor);
//         }
//     } else {
//         // the other octant 2, 4, 6, 8 (between 45 and 90, 135 and 180, ... degree)
//         if (tSwap) {
//             tStepX = -tStepX;
//         } else {
//             tDrawStartAdjustCount = (aThickness - 1) - tDrawStartAdjustCount;
//             tStepY = -tStepY;
//         }
//         // adjust draw start point
//         tError = tDeltaXTimes2 - tDeltaY;
//         for (i = tDrawStartAdjustCount; i > 0; i--) {
//             aYStart -= tStepY;
//             aYEnd -= tStepY;
//             if (tError >= 0) {
//                 aXStart -= tStepX;
//                 aXEnd -= tStepX;
//                 tError -= tDeltaYTimes2;
//             }
//             tError += tDeltaXTimes2;
//         }
//         //draw start line
//         drawLine(aXStart, aYStart, aXEnd, aYEnd, aColor);
//         // draw aThickness number of lines
//         tError = tDeltaXTimes2 - tDeltaY;
//         for (i = aThickness; i > 1; i--) {
//             aYStart += tStepY;
//             aYEnd += tStepY;
//             tOverlap = LINE_OVERLAP_NONE;
//             if (tError >= 0) {
//                 aXStart += tStepX;
//                 aXEnd += tStepX;
//                 tError -= tDeltaYTimes2;
//                 tOverlap = LINE_OVERLAP_MAJOR;
//             }
//             tError += tDeltaXTimes2;
//             drawLineOverlap(aXStart, aYStart, aXEnd, aYEnd, tOverlap, aColor);
//         }
//     }
// }
