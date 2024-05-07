#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>

#include "RenderWindow.hpp"
#include "Utilities.hpp"

class AutoTiler {
    private:
        static const std::vector<unsigned char> Vals_8Bit;

        static unsigned char GetMask_4Bit(const bool &t, const bool &l, const bool &r, const bool &b) {return t + l * 2 + r * 4 + b * 8;}
        static unsigned char GetMask_8Bit(const bool &tl, const bool &t, const bool &tr, const bool &l, const bool &r, const bool &bl, const bool &b, const bool &br) {return (tl && t && l) + t * 2 + (tr && t && r) * 4 + l * 8 + r * 16 + (bl && b && l) * 32 + b * 64 + (br && b && r) * 128;}
        static char IndexVal_8Bit(const unsigned char &maskVal) {return maskVal == 0 ? 47 : btils::binarySearch(Vals_8Bit, maskVal, 0, Vals_8Bit.size() - 1) + 1;}

    public:
        static char half(std::vector<std::vector<char>> &grid, const unsigned long int &x, const unsigned long int &y, const bool &addTile = true) {
            if (y >= grid.size() || x >= grid.at(0).size()) {return -1;}
            grid[y][x] = addTile - 1;

            for (char i = -1; i < 2; i++) {
                for (char j = -1; j < 2; j++) {
                    if (std::abs(i) == 1 && std::abs(j) == 1) {continue;}
                    if (y + i < 0 || y + i >= grid.size() || x + j < 0 || x + j >= grid.at(0).size()) {continue;}
                    if (grid.at(y + i).at(x + j) < 0) {continue;}

                    const bool t = y + i > 0 ? (grid.at(y + i - 1).at(x + j) >= 0) : false;
                    const bool l = x + j > 0 ? (grid.at(y + i).at(x + j - 1) >= 0) : false;
                    const bool r = x + j < grid.at(0).size() - 1 ? (grid.at(y + i).at(x + j + 1) >= 0) : false;
                    const bool b = y + i < grid.size() - 1 ? (grid.at(y + i + 1).at(x + j) >= 0) : false;

                    grid[y + i][x + j] = GetMask_4Bit(t, l, r, b);
                }
            }
            return grid.at(y).at(x);
        }
        static char full(std::vector<std::vector<char>> &grid, const unsigned long int &x, const unsigned long int &y, const bool &addTile = true) {
            if (y >= grid.size() || x >= grid.at(0).size()) {return -1;}
            grid[y][x] = addTile - 1;

            for (char i = -1; i < 2; i++) {
                for (char j = -1; j < 2; j++) {
                    if (y + i < 0 || y + i >= grid.size() || x + j < 0 || x + j >= grid.at(0).size()) {continue;}
                    if (grid.at(y + i).at(x + j) < 0) {continue;}

                    const bool tl = y + i > 0 && x + j > 0 ? (grid.at(y + i - 1).at(x + j - 1) >= 0) : false;
                    const bool t = y + i > 0 ? (grid.at(y + i - 1).at(x + j) >= 0) : false;
                    const bool tr = y + i > 0 && x + j < grid.at(0).size() - 1 ? (grid.at(y + i - 1).at(x + j + 1) >= 0) : false;
                    const bool l = x + j > 0 ? (grid.at(y + i).at(x + j - 1) >= 0) : false;
                    const bool r = x + j < grid.at(0).size() - 1 ? (grid.at(y + i).at(x + j + 1) >= 0) : false;
                    const bool bl = y + i < grid.size() - 1 && x + j > 0 ? (grid.at(y + i + 1).at(x + j - 1) >= 0) : false;
                    const bool b = y + i < grid.size() - 1 ? (grid.at(y + i + 1).at(x + j) >= 0) : false;
                    const bool br = y + i < grid.size() - 1 && x + j < grid.at(0).size() - 1 ? (grid.at(y + i + 1).at(x + j + 1) >= 0) : false;

                    grid[y + i][x + j] = IndexVal_8Bit(GetMask_8Bit(tl, t, tr, l, r, bl, b, br));
                }
            }
            return grid.at(y).at(x);
        }

} AutoTile;
const std::vector<unsigned char> AutoTiler::Vals_8Bit = {2, 8, 10, 11, 16, 18, 22, 24, 26, 27, 30, 31, 64, 66, 72, 74, 75, 80, 82, 86, 88, 90, 91, 94, 95, 104, 106, 107, 120, 122, 123, 126, 127, 208, 210, 214, 216, 218, 219, 222, 223, 248, 250, 251, 254, 255};

double HireTime_Sec() {return SDL_GetTicks() * 0.01f;}
int main(int argc, char* args[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {std::cout << "Error initializing SDL2\nERROR: " << SDL_GetError() << "\n";}
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {std::cout << "Error initializing SDL2_image\nERROR: " << IMG_GetError() << "\n";}
    if (TTF_Init() == -1) {std::cout << "Error initializing SDL2_ttf\nERROR: " << TTF_GetError() << "\n";}

    RenderWindow Window("Autotile", 1280, 720, SDL_WINDOW_SHOWN | SDL_WINDOW_UTILITY);
    SDL_Event Event;
    const Uint8 *Keystate = SDL_GetKeyboardState(NULL);

    TTF_Font *font = TTF_OpenFont("../dev/fonts/GNU-Unifont.ttf", 20);

    struct {
        SDL_Point PosC = {0, 0};
        SDL_Point PosR = {0, 0};
        SDL_Point Rel = {0, 0};
        double Sensitivity = 10;

        bool Motion = false;
        bool Released = true;
        bool Captured = false;

        bool Pressed[5] = {false, false, false, false, false};
    } MouseInfo;

    struct {
        int Quit = SDL_SCANCODE_ESCAPE;
        int ToggleCapture = SDL_SCANCODE_F1;
    } Keybinds;

    long double t = 0.0;
    double dt = 0.01;

    int startTicks = 0, frameTicks = 0;
    double currentTime = HireTime_Sec();
    long double newTime = 0.0;
    long double frameTime = 0.0;
    double accumulator = 0.0;

    int cellSize = 40;
    SDL_Point mousePosGrid = {0, 0}, prevMousePosGrid = mousePosGrid;
    bool madeChanges = true;
    unsigned char setNumber = 0;

    std::vector<SDL_Texture*> spriteSheets = {
        Window.loadTexture("../dev/png/imperialPath/sheet4bit.png"),
        Window.loadTexture("../dev/png/imperialPath/sheet8bit.png"),
        Window.loadTexture("../dev/png/ironFence/sheet4bit.png"),
        Window.loadTexture("../dev/png/ironFence/sheet8bit.png")
    };
    std::vector<std::vector<std::vector<char>>> grid;
    for (unsigned char i = 0; i < spriteSheets.size(); i++) {
        grid.emplace_back();
        for (int j = 0; j < Window.getH() / cellSize; j++) {
            grid[i].emplace_back();
            for (int k = 0; k < Window.getW() / cellSize; k++) {
                grid[i][j].emplace_back(-1);
            }
        }
    }

    bool running = true;
    while (running) {
        startTicks = SDL_GetTicks();
        newTime = HireTime_Sec();
        frameTime = newTime - currentTime;
        currentTime = newTime;
        accumulator += frameTime;

        while (accumulator >= dt) {
            while (SDL_PollEvent(&Event)) {
                switch (Event.type) {
                    case SDL_QUIT:
                        running = false;
                        break;
                    case SDL_MOUSEMOTION:
                        MouseInfo.Motion = true;
                        SDL_GetMouseState(&MouseInfo.PosR.x, &MouseInfo.PosR.y);
                        MouseInfo.PosC.x = MouseInfo.PosR.x - Window.getW_2();
                        MouseInfo.PosC.y = Window.getH() - MouseInfo.PosR.y - Window.getH_2();

                        if (!MouseInfo.Released) {
                            MouseInfo.Rel.x = Event.motion.xrel;
                            MouseInfo.Rel.y = -Event.motion.yrel;
                        } else {
                            MouseInfo.Released = false;
                            MouseInfo.Rel = {0, 0};
                        }

                        mousePosGrid = {MouseInfo.PosR.x / cellSize, MouseInfo.PosR.y / cellSize};
                        break;
                    case SDL_KEYDOWN:
                        if (!Event.key.repeat) {
                            if (Keystate[Keybinds.ToggleCapture]) {
                                MouseInfo.Captured = !MouseInfo.Captured;
                                if (MouseInfo.Captured) {
                                    SDL_SetRelativeMouseMode(SDL_TRUE);
                                    MouseInfo.Released = true;
                                    MouseInfo.Rel = {0, 0};
                                } else {
                                    SDL_SetRelativeMouseMode(SDL_FALSE);
                                    Window.centerMouse();
                                    MouseInfo.PosC = {0, 0};
                                    MouseInfo.PosR = {0, 0};
                                }
                            }
                            if (Keystate[SDL_SCANCODE_SPACE]) {
                                setNumber++;
                                setNumber %= spriteSheets.size();
                            }
                            if (Keystate[SDL_SCANCODE_C]) {
                                for (unsigned long int i = 0; i < grid.at(0).size(); i++) {
                                    for (unsigned long int j = 0; j < grid.at(0).at(0).size(); j++) {
                                        grid[setNumber][i][j] = -1;
                                    }
                                }
                                madeChanges = true;
                            }
                        }
                        break;
                    case SDL_WINDOWEVENT:
                        Window.handleEvent(Event.window);
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        MouseInfo.Pressed[Event.button.button] = true;
                        switch (Event.button.button) {
                            case SDL_BUTTON_LEFT:
                                if (setNumber % 2 == 0) {
                                    if (AutoTile.half(grid[setNumber], mousePosGrid.x, mousePosGrid.y, true) != -1) {madeChanges = true;}
                                } else {
                                    if (AutoTile.full(grid[setNumber], mousePosGrid.x, mousePosGrid.y, true) != -1) {madeChanges = true;}
                                }
                                break;
                            case SDL_BUTTON_RIGHT:
                                if (setNumber % 2 == 0) {AutoTile.half(grid[setNumber], mousePosGrid.x, mousePosGrid.y, false);}
                                else {AutoTile.full(grid[setNumber], mousePosGrid.x, mousePosGrid.y, false);}
                                madeChanges = true;
                                break;
                        }
                        break;
                    case SDL_MOUSEBUTTONUP:
                        MouseInfo.Pressed[Event.button.button] = false;
                        break;
                }
                if (!running) {break;}
            }
            if (!running || Keystate[Keybinds.Quit]) {
                running = false;
                break;
            }

            if ((mousePosGrid.x != prevMousePosGrid.x) || (mousePosGrid.y != prevMousePosGrid.y)) {
                prevMousePosGrid = mousePosGrid;

                if (MouseInfo.Pressed[SDL_BUTTON_LEFT]) {
                    if (setNumber % 2 == 0) {
                        if (AutoTile.half(grid[setNumber], mousePosGrid.x, mousePosGrid.y, true) != -1) {madeChanges = true;}
                    } else {
                        if (AutoTile.full(grid[setNumber], mousePosGrid.x, mousePosGrid.y, true) != -1) {madeChanges = true;}
                    }
                }
                if (MouseInfo.Pressed[SDL_BUTTON_RIGHT]) {
                    if (setNumber % 2 == 0) {AutoTile.half(grid[setNumber], mousePosGrid.x, mousePosGrid.y, false);}
                    else {AutoTile.full(grid[setNumber], mousePosGrid.x, mousePosGrid.y, false);}
                    madeChanges = true;
                }
            }

            t += dt;
            accumulator -= dt;
            MouseInfo.Motion = false;
        }
        if (!running) {break;}

        if (madeChanges) {
            madeChanges = false;
            Window.clear();

            for (unsigned long int i = 0; i < grid.at(0).size(); i++) {
                for (unsigned long int j = 0; j < grid.at(0).at(i).size(); j++) {
                    Window.fillRectangle(-Window.getW_2() + j * cellSize, Window.getH_2() - i * cellSize, cellSize, cellSize, PresetColors[COLOR_WHITE]);
                    Window.drawRectangle(-Window.getW_2() + j * cellSize, Window.getH_2() - i * cellSize, cellSize, cellSize, PresetColors[COLOR_BLACK]);
                }
            }

            for (unsigned char i = 0; i < spriteSheets.size(); i++) {
                for (unsigned long int j = 0; j < grid.at(i).size(); j++) {
                    for (unsigned long int k = 0; k < grid.at(i).at(j).size(); k++) {
                        if (grid.at(i).at(j).at(k) >= 0) {
                            const SDL_Rect frame = {grid.at(i).at(j).at(k) % (i % 2 == 0 ? 4 : 8) * 16, grid.at(i).at(j).at(k) / (i % 2 == 0 ? 4 : 8) * 16, 16, 16};
                            Window.renderTexture(spriteSheets[i], frame, {-Window.getW_2() + (int)k * cellSize, Window.getH_2() - (int)j * cellSize, cellSize, cellSize});
                        }
                    }
                }
            }

            Window.show();
        }

        if ((frameTicks = SDL_GetTicks() - startTicks) < 1000 / Window.getRefreshRate()) {SDL_Delay(1000 / Window.getRefreshRate() - frameTicks);}
    }

    TTF_CloseFont(font);
    for (unsigned long int i = spriteSheets.size(); i > 0; i--) {
        SDL_DestroyTexture(spriteSheets[i - 1]);
        spriteSheets.erase(spriteSheets.end());
    }

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0; 
}
