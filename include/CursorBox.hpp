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
        SDL_Rect Box = {0, 0, 0, 0};

    public:
        CursorBox(const SDL_Rect &box) : Box(box) {}

        bool check(const MouseState &mstate) {
            return mstate.PosR.x >= Box.x && mstate.PosR.y >= Box.y && mstate.PosR.x <= Box.x + Box.w && mstate.PosR.y <= Box.y + Box.h;
        }
};

#endif /* CURSORBOX */
