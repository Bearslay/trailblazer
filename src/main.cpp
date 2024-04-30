#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>

#include "RenderWindow.hpp"
#include "Utilities.hpp"

#include "Perlin.hpp"
#include "AStar.hpp"

std::vector<std::vector<Uint8>> getNoise(int w, int h, int o = 8, double bias = 2.0, double scale = 350.0) {
    std::vector<std::vector<Uint8>> output;

    for (int i = 0; i < h; i++) {
        output.emplace_back();
        for (int j = 0; j < w; j++) {
            double val = 0.0, freq = 1.0, amp = 1.0;

            for (int k = 0; k < o; k++) {
                val += perlin(i * freq / scale, j * freq / scale) * amp;

                freq *= bias;
                amp /= bias;
            }

            if (val > 1.0) {val = 1.0;}
            else if (val < -1.0) {val = -1.0;}

            output[i].emplace_back((Uint8)((val * 0.5 + 0.5) * 255));
        }
    }

    return output;
}

// SDL_Texture* getNoiseTexture(int w, int h, int o = 8, double bias = 2.0, double scale = 350.0) {
//     std::vector<std::vector<Uint8>> noise = getNoise(w, h, o, bias, scale);
//     SDL_Surface* surface = SDL_CreateRGBSurface(0, w, h, 32, 0, 0, 0, 0);
//     SDL_LockSurface(surface);
//     Uint8* pixels = (Uint8*)surface->pixels;
//     for (unsigned long int i = 0; i < noise.size(); i++) {
//         for (unsigned long int j = 0; j < noise[0].size(); j++) {
//             const unsigned char val = (unsigned char)((noise[i][j] * 0.5 + 0.5) * 255.0);
//             pixels[i * surface->pitch + j * surface->format->BytesPerPixel + 0] = val;
//             pixels[i * surface->pitch + j * surface->format->BytesPerPixel + 1] = val;
//             pixels[i * surface->pitch + j * surface->format->BytesPerPixel + 2] = val;
//         }
//     }
//     SDL_UnlockSurface(surface);
//     SDL_SaveBMP(surface, "dev/map.bmp");
//     return nullptr;
// }

double HireTime_Sec() {return SDL_GetTicks() * 0.01f;}
int main(int argc, char* args[]) {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {std::cout << "Error initializing SDL2\nERROR: " << SDL_GetError() << "\n";}
    if (IMG_Init(IMG_INIT_PNG) != IMG_INIT_PNG) {std::cout << "Error initializing SDL2_image\nERROR: " << SDL_GetError() << "\n";}
    if (TTF_Init() == -1) {std::cout << "Error initializing SDL2_ttf\nERROR: " << SDL_GetError() << "\n";}

    RenderWindow Window("Trailblazer", 1280, 720, SDL_WINDOW_SHOWN | SDL_WINDOW_UTILITY);
    SDL_Event Event;
    const Uint8 *Keystate = SDL_GetKeyboardState(NULL);

    TTF_Font *font = TTF_OpenFont("dev/fonts/GNU-Unifont.ttf", 20);

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
        int AStar_Cardinal = SDL_SCANCODE_1;
        int AStar_Compass = SDL_SCANCODE_2;
        int AStar_General = SDL_SCANCODE_3;
    } Keybinds;

    long double t = 0.0;
    double dt = 0.01;

    int startTicks = 0, frameTicks = 0;
    double currentTime = HireTime_Sec();
    long double newTime = 0.0;
    long double frameTime = 0.0;
    double accumulator = 0.0;

    // std::vector<std::vector<Uint8>> Noise = getNoise(Window.getW(), Window.getH());

    // Window.clear();
    // for (unsigned long int i = 0; i < Noise.size(); i++) {
    //     for (unsigned long int j = 0; j < Noise[0].size(); j++) {
    //         Window.drawPixel(-Window.getW_2() + j, Window.getH_2() - i, {Noise[i][j], Noise[i][j], Noise[i][j], 255});
    //     }
    // }
    // Window.renderText(font, u"hola", {0, 0});
    // Window.show();

    int cellSize = 16;
    std::vector<std::vector<double>> grid;
    std::pair<unsigned long int, unsigned long int> start = std::make_pair(0, 0), goal = std::make_pair(Window.getH() / cellSize - 1, Window.getW() / cellSize - 1);
    std::vector<std::pair<int, int>> pathNodes;
    bool madeChanges = true;

    for (int i = 0; i < Window.getH() / cellSize; i++) {
        grid.emplace_back();
        for (int j = 0; j < Window.getW() / cellSize; j++) {
            grid[i].emplace_back(0.0);
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
                        break;
                    case SDL_KEYDOWN:
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
                        if (Keystate[Keybinds.AStar_Cardinal]) {
                            madeChanges = true;
                            pathNodes = aStar_CardinalGrid(grid, start, goal);
                        }
                        if (Keystate[Keybinds.AStar_Compass]) {
                            madeChanges = true;
                            pathNodes = aStar_CompassGrid(grid, start, goal);
                        }
                        if (Keystate[Keybinds.AStar_General]) {
                            madeChanges = true;
                            pathNodes = aStar_GeneralGrid(grid, start, goal);
                        }
                        break;
                    case SDL_WINDOWEVENT:
                        Window.handleEvent(Event.window);
                        break;
                    case SDL_MOUSEBUTTONDOWN:
                        MouseInfo.Pressed[Event.button.button] = true;
                        switch (Event.button.button) {
                            case SDL_BUTTON_MIDDLE:
                                for (unsigned long int i = 0; i < grid.size(); i++) {
                                    for (unsigned long int j = 0; j < grid.at(i).size(); j++) {
                                        grid[i][j] = 0.0;
                                    }
                                }
                                madeChanges = true;
                                break;
                            case SDL_BUTTON_X1:
                                start = std::make_pair(MouseInfo.PosR.y / cellSize, MouseInfo.PosR.x / cellSize);
                                madeChanges = true;
                                break;
                            case SDL_BUTTON_X2:
                                goal = std::make_pair(MouseInfo.PosR.y / cellSize, MouseInfo.PosR.x / cellSize);
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

            if (MouseInfo.Pressed[SDL_BUTTON_LEFT]) {
                grid[MouseInfo.PosR.y / cellSize][MouseInfo.PosR.x / cellSize] = 1.0;
                madeChanges = true;
            } else if (MouseInfo.Pressed[SDL_BUTTON_RIGHT]) {
                grid[MouseInfo.PosR.y / cellSize][MouseInfo.PosR.x / cellSize] = 0.0;
                madeChanges = true;
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
                for (unsigned long int j = 0; j < grid[i].size(); j++) {
                    const unsigned char shade = 255 - (unsigned char)(grid[i][j] * 255.0);
                    Window.fillRectangle(-Window.getW_2() + j * cellSize, Window.getH_2() - i * cellSize, cellSize, cellSize, {shade, shade, shade, 255});
                    if (grid[i][j] <= 0.0) {
                        Window.drawRectangle(-Window.getW_2() + j * cellSize, Window.getH_2() - i * cellSize, cellSize, cellSize, PresetColors[COLOR_LIGHT_GRAY]);
                    }
                }
            }
            Window.fillRectangle(-Window.getW_2() + start.second * cellSize, Window.getH_2() - start.first * cellSize, cellSize, cellSize, PresetColors[COLOR_TEAL]);
            Window.fillRectangle(-Window.getW_2() +  goal.second * cellSize, Window.getH_2() -  goal.first * cellSize, cellSize, cellSize, PresetColors[COLOR_MAROON]);

            for (unsigned long int i = 1; i < pathNodes.size(); i++) {
                Window.drawLine(-Window.getW_2() + cellSize / 2 + pathNodes.at(i - 1).second * cellSize, Window.getH_2() - cellSize / 2 - pathNodes.at(i - 1).first  * cellSize, -Window.getW_2() + cellSize / 2 + pathNodes.at(i).second * cellSize, Window.getH_2() - cellSize / 2 - pathNodes.at(i).first * cellSize, PresetColors[COLOR_GREEN]);
            }

            Window.show();
        }

        frameTicks = SDL_GetTicks() - startTicks;
        if (frameTicks < 1000 / Window.getRefreshRate()) {
            SDL_Delay(1000 / Window.getRefreshRate() - frameTicks);
        }
    }

    TTF_CloseFont(font);

    TTF_Quit();
    IMG_Quit();
    SDL_Quit();
    return 0; 
}
