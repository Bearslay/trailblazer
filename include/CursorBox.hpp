#ifndef CURSORBOX
#define CURSORBOX

#include <SDL2/SDL_rect.h>

struct MouseState {
    SDL_Point PosC = {0, 0};
    SDL_Point PosR = {0, 0};
    SDL_Point Rel = {0, 0};
    double Sensitivity = 10;

    bool Motion = false;
    bool Released = true;
    bool Captured = false;

    bool Pressed[6] = {false, false, false, false, false, false};
};

class CursorBox {
    private:
        SDL_Rect BoxC = {0, 0, 0, 0};
        SDL_Rect BoxR = {0, 0, 0, 0};

    public:
        CursorBox(const SDL_Rect &box, const int &dimx, const int &dimy) {
            BoxC = box;
            BoxR = {box.x - dimx / 2, box.y - dimy / 2, box.w, box.h};
        }
};

#endif /* CURSORBOX */
