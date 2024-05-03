#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>

#include "RenderWindow.hpp"
#include "Utilities.hpp"

const std::vector<unsigned char> halfMaskVals = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15};
unsigned char maskHalfTile(const bool &t, const bool &l, const bool &r, const bool &b) {return t + l * 2 + r * 4 + b * 8;}
char halfMask2Index(const unsigned char &maskVal) {return btils::binarySearch(halfMaskVals, maskVal, 0, halfMaskVals.size() - 1);}

const std::vector<unsigned char> fullMaskVals = {2, 8, 10, 11, 16, 18, 22, 24, 26, 27, 30, 31, 64, 66, 72, 74, 75, 80, 82, 86, 88, 90, 91, 94, 95, 104, 106, 107, 120, 122, 123, 126, 127, 208, 210, 214, 216, 218, 219, 222, 223, 248, 250, 251, 254, 255};
unsigned char maskFullTile(const bool &tl, const bool &t, const bool &tr, const bool &l, const bool &r, const bool &bl, const bool &b, const bool &br) {
    const bool topl = tl && t && l, topr = tr && t && r, botl = bl && b && l, botr = br && b && r;
    return topl + t * 2 + topr * 4 + l * 8 + r * 16 + botl * 32 + b * 64 + botr * 128;
}
char fullMask2Index(const unsigned char &maskVal) {return maskVal == 0 ? 47 : btils::binarySearch(fullMaskVals, maskVal, 0, fullMaskVals.size() - 1) + 1;}

double HireTime_Sec() {return SDL_GetTicks() * 0.01f;}
int main(int argc, char* args[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {std::cout << "Error initializing SDL2\nERROR: " << SDL_GetError() << "\n";}
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {std::cout << "Error initializing SDL2_image\nERROR: " << IMG_GetError() << "\n";}
    if (TTF_Init() == -1) {std::cout << "Error initializing SDL2_ttf\nERROR: " << TTF_GetError() << "\n";}

    RenderWindow Window("Tileset Bitmask", 1280, 720, SDL_WINDOW_SHOWN | SDL_WINDOW_UTILITY);
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

    int cellSize = 80;
    SDL_Point mousePosGrid = {0, 0}, prevMousePosGrid = mousePosGrid;
    bool madeChanges = true;

    std::vector<std::vector<bool>> grid;
    for (int i = 0; i < Window.getH() / cellSize; i++) {
        grid.emplace_back();
        for (int j = 0; j < Window.getW() / cellSize; j++) {
            grid[i].emplace_back(false);
        }
    }
    SDL_Texture *tileset = Window.loadTexture("../dev/png/optimalTilesetCompass.png");
    char textureIndex = 0;

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
                                // textureIndex = halfMask2Index(maskHalfTile(grid.at(0).at(1), grid.at(1).at(0), grid.at(1).at(2), grid.at(2).at(1)));
                                textureIndex = fullMask2Index(maskFullTile(grid.at(0).at(0), grid.at(0).at(1), grid.at(0).at(2), grid.at(1).at(0), grid.at(1).at(2), grid.at(2).at(0), grid.at(2).at(1), grid.at(2).at(2)));
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
                                if (MouseInfo.PosR.y / cellSize < (int)grid.size() && MouseInfo.PosR.x / cellSize < (int)grid.at(0).size()) {
                                    grid[MouseInfo.PosR.y / cellSize][MouseInfo.PosR.x / cellSize] = 1;
                                    madeChanges = true;
                                }
                                break;
                            case SDL_BUTTON_RIGHT:
                                if (MouseInfo.PosR.y / cellSize < (int)grid.size() && MouseInfo.PosR.x / cellSize < (int)grid.at(0).size()) {
                                    grid[MouseInfo.PosR.y / cellSize][MouseInfo.PosR.x / cellSize] = 0;
                                    madeChanges = true;
                                }
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
                    if (MouseInfo.PosR.y / cellSize < (int)grid.size() && MouseInfo.PosR.x / cellSize < (int)grid.at(0).size()) {
                        grid[MouseInfo.PosR.y / cellSize][MouseInfo.PosR.x / cellSize] = 1;
                        madeChanges = true;
                    }
                }
                if (MouseInfo.Pressed[SDL_BUTTON_RIGHT]) {
                    if (MouseInfo.PosR.y / cellSize < (int)grid.size() && MouseInfo.PosR.x / cellSize < (int)grid.at(0).size()) {
                        grid[MouseInfo.PosR.y / cellSize][MouseInfo.PosR.x / cellSize] = 0;
                        madeChanges = true;
                    }
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

            for (unsigned long int i = 0; i < grid.size(); i++) {
                for (unsigned long int j = 0; j < grid.at(i).size(); j++) {
                    Window.fillRectangle(-Window.getW_2() + j * cellSize, Window.getH_2() - i * cellSize, cellSize, cellSize, PresetColors[COLOR_WHITE]);
                    Window.drawRectangle(-Window.getW_2() + j * cellSize, Window.getH_2() - i * cellSize, cellSize, cellSize, PresetColors[COLOR_BLACK]);
                }
            }

            // Detect which cells had the changes made and only compute the new texture for that cell

            if (textureIndex >= 0 && textureIndex < 48) {
                const SDL_Rect frame = {textureIndex % 8 * 16, textureIndex / 8 * 16, 16, 16};
                Window.renderTexture(tileset, frame, {-Window.getW_2() + cellSize, Window.getH_2() - cellSize, cellSize, cellSize});
            }

            Window.show();
        }

        if ((frameTicks = SDL_GetTicks() - startTicks) < 1000 / Window.getRefreshRate()) {SDL_Delay(1000 / Window.getRefreshRate() - frameTicks);}
    }

    TTF_CloseFont(font);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0; 
}
