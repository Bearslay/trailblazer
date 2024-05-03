#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>

#include "RenderWindow.hpp"
#include "Utilities.hpp"

unsigned char autotile_mask4Bit(const bool &t, const bool &l, const bool &r, const bool &b) {return t + l * 2 + r * 4 + b * 8;}
char autotile4Bit(std::vector<std::vector<char>> &grid, const unsigned long int &x, const unsigned long int &y, const bool &addTile = true) {
    if (y >= grid.size() || x >= grid.at(0).size()) {return -1;}

    // Assign the target tile a dummy value
    grid[y][x] = addTile - 1;

    // Loop through the 3x3 grid of tiles that will probably be affected by the change
    for (char i = -1; i < 2; i++) {
        for (char j = -1; j < 2; j++) {
            // Skip the corners
            if (std::abs(i) == 1 && std::abs(j) == 1) {continue;}
            // Make sure the targeted tile actually exists
            if (y + i < 0 || y + i >= grid.size() || x + j < 0 || x + j >= grid.at(0).size()) {continue;}
            // If the targeted tile isn't filled, then its value doesn't need to be updated
            if (grid.at(y + i).at(x + j) < 0) {continue;}

            const bool t = y + i > 0 ? (grid.at(y + i - 1).at(x + j) >= 0) : false;
            const bool l = x + j > 0 ? (grid.at(y + i).at(x + j - 1) >= 0) : false;
            const bool r = x + j < grid.at(0).size() - 1 ? (grid.at(y + i).at(x + j + 1) >= 0) : false;
            const bool b = y + i < grid.size() - 1 ? (grid.at(y + i + 1).at(x + j) >= 0) : false;

            grid[y + i][x + j] = autotile_mask4Bit(t, l, r, b);
        }
    }
    return grid.at(y).at(x);
}

const std::vector<unsigned char> autotile_8BitVals = {2, 8, 10, 11, 16, 18, 22, 24, 26, 27, 30, 31, 64, 66, 72, 74, 75, 80, 82, 86, 88, 90, 91, 94, 95, 104, 106, 107, 120, 122, 123, 126, 127, 208, 210, 214, 216, 218, 219, 222, 223, 248, 250, 251, 254, 255};
unsigned char autotile_mask8Bit(const bool &tl, const bool &t, const bool &tr, const bool &l, const bool &r, const bool &bl, const bool &b, const bool &br) {return (tl && t && l) + t * 2 + (tr && t && r) * 4 + l * 8 + r * 16 + (bl && b && l) * 32 + b * 64 + (br && b && r) * 128;}
char autotile_index8BitMask(const unsigned char &maskVal) {return maskVal == 0 ? 47 : btils::binarySearch(autotile_8BitVals, maskVal, 0, autotile_8BitVals.size() - 1) + 1;}
char autotile8Bit(std::vector<std::vector<char>> &grid, const unsigned long int &x, const unsigned long int &y, const bool &addTile = true) {
    if (y >= grid.size() || x >= grid.at(0).size()) {return -1;}

    // Assign the target tile a dummy value
    grid[y][x] = addTile - 1;

    // Loop through the 3x3 grid of tiles that will probably be affected by the change
    for (char i = -1; i < 2; i++) {
        for (char j = -1; j < 2; j++) {
            // Make sure the targeted tile actually exists
            if (y + i < 0 || y + i >= grid.size() || x + j < 0 || x + j >= grid.at(0).size()) {continue;}
            // If the targeted tile isn't filled, then its value doesn't need to be updated
            if (grid.at(y + i).at(x + j) < 0) {continue;}

            const bool tl = y + i > 0 && x + j > 0 ? (grid.at(y + i - 1).at(x + j - 1) >= 0) : false;
            const bool t = y + i > 0 ? (grid.at(y + i - 1).at(x + j) >= 0) : false;
            const bool tr = y + i > 0 && x + j < grid.at(0).size() - 1 ? (grid.at(y + i - 1).at(x + j + 1) >= 0) : false;
            const bool l = x + j > 0 ? (grid.at(y + i).at(x + j - 1) >= 0) : false;
            const bool r = x + j < grid.at(0).size() - 1 ? (grid.at(y + i).at(x + j + 1) >= 0) : false;
            const bool bl = y + i < grid.size() - 1 && x + j > 0 ? (grid.at(y + i + 1).at(x + j - 1) >= 0) : false;
            const bool b = y + i < grid.size() - 1 ? (grid.at(y + i + 1).at(x + j) >= 0) : false;
            const bool br = y + i < grid.size() - 1 && x + j < grid.at(0).size() - 1 ? (grid.at(y + i + 1).at(x + j + 1) >= 0) : false;

            grid[y + i][x + j] = autotile_index8BitMask(autotile_mask8Bit(tl, t, tr, l, r, bl, b, br));
        }
    }
    return grid.at(y).at(x);
}

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
    bool use8Bit = true;

    std::vector<std::vector<char>> grid8Bit, grid4Bit;
    for (int i = 0; i < Window.getH() / cellSize; i++) {
        grid8Bit.emplace_back();
        grid4Bit.emplace_back();
        for (int j = 0; j < Window.getW() / cellSize; j++) {
            grid8Bit[i].emplace_back(-1);
            grid4Bit[i].emplace_back(-1);
        }
    }
    SDL_Texture *tileset8Bit = Window.loadTexture("../dev/png/optimalTilesetCompass.png");
    SDL_Texture *tileset4Bit = Window.loadTexture("../dev/png/optimalTilesetCardinal.png");

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
                                use8Bit = !use8Bit;
                            }
                            if (Keystate[SDL_SCANCODE_C]) {
                                for (unsigned long int i = 0; i < grid8Bit.size(); i++) {
                                    for (unsigned long int j = 0; j < grid8Bit.at(0).size(); j++) {
                                        if (use8Bit) {grid8Bit[i][j] = -1;}
                                        else {grid4Bit[i][j] = -1;}
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
                                if (use8Bit) {
                                    if (autotile8Bit(grid8Bit, mousePosGrid.x, mousePosGrid.y, true) != -1) {madeChanges = true;}
                                } else {
                                    if (autotile4Bit(grid4Bit, mousePosGrid.x, mousePosGrid.y, true) != -1) {madeChanges = true;}
                                }
                                break;
                            case SDL_BUTTON_RIGHT:
                                if (use8Bit) {autotile8Bit(grid8Bit, mousePosGrid.x, mousePosGrid.y, false);}
                                else {autotile4Bit(grid4Bit, mousePosGrid.x, mousePosGrid.y, false);}
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
                    if (use8Bit) {
                        if (autotile8Bit(grid8Bit, mousePosGrid.x, mousePosGrid.y, true) != -1) {madeChanges = true;}
                    } else {
                        if (autotile4Bit(grid4Bit, mousePosGrid.x, mousePosGrid.y, true) != -1) {madeChanges = true;}
                    }
                }
                if (MouseInfo.Pressed[SDL_BUTTON_RIGHT]) {
                    if (use8Bit) {autotile8Bit(grid8Bit, mousePosGrid.x, mousePosGrid.y, false);}
                    else {autotile4Bit(grid4Bit, mousePosGrid.x, mousePosGrid.y, false);}
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

            for (unsigned long int i = 0; i < grid8Bit.size(); i++) {
                for (unsigned long int j = 0; j < grid8Bit.at(i).size(); j++) {
                    Window.fillRectangle(-Window.getW_2() + j * cellSize, Window.getH_2() - i * cellSize, cellSize, cellSize, PresetColors[COLOR_WHITE]);
                    Window.drawRectangle(-Window.getW_2() + j * cellSize, Window.getH_2() - i * cellSize, cellSize, cellSize, PresetColors[COLOR_BLACK]);

                    if (grid8Bit.at(i).at(j) >= 0) {
                        const SDL_Rect frame = {grid8Bit.at(i).at(j) % 8 * 16, grid8Bit.at(i).at(j) / 8 * 16, 16, 16};
                        Window.renderTexture(tileset8Bit, frame, {-Window.getW_2() + (int)j * cellSize, Window.getH_2() - (int)i * cellSize, cellSize, cellSize});
                    }
                    if (grid4Bit.at(i).at(j) >= 0) {
                        const SDL_Rect frame = {grid4Bit.at(i).at(j) % 4 * 16, grid4Bit.at(i).at(j) / 4 * 16, 16, 16};
                        Window.renderTexture(tileset4Bit, frame, {-Window.getW_2() + (int)j * cellSize, Window.getH_2() - (int)i * cellSize, cellSize, cellSize});
                    }
                }
            }

            Window.show();
        }

        if ((frameTicks = SDL_GetTicks() - startTicks) < 1000 / Window.getRefreshRate()) {SDL_Delay(1000 / Window.getRefreshRate() - frameTicks);}
    }

    TTF_CloseFont(font);
    SDL_DestroyTexture(tileset8Bit);
    SDL_DestroyTexture(tileset4Bit);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0; 
}
