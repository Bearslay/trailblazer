#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>
#include <vector>

#include "RenderWindow.hpp"
#include "Utilities.hpp"

#include "Perlin.hpp"

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

    RenderWindow Window("Trailblazer", 1280, 720, SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_UTILITY);
    SDL_Event Event;
    const Uint8 *Keystate = SDL_GetKeyboardState(NULL);

    TTF_Font *font = TTF_OpenFont("dev/fonts/GNU-Unifont.ttf", 20);

    struct {
        SDL_Point Pos = {0, 0};
        SDL_Point Rel = {0, 0};
        double Sensitivity = 10;

        bool Motion = false;
        bool Released = true;
        bool Captured = false;
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

    std::vector<std::vector<Uint8>> Noise = getNoise(Window.getW(), Window.getH());

    Window.clear();
    for (unsigned long int i = 0; i < Noise.size(); i++) {
        for (unsigned long int j = 0; j < Noise[0].size(); j++) {
            Window.drawPixel(-Window.getW_2() + j, Window.getH_2() - i, {Noise[i][j], Noise[i][j], Noise[i][j], 255});
        }
    }
    Window.renderText(font, u"hola", {0, 0});
    Window.show();

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
                        SDL_GetMouseState(&MouseInfo.Pos.x, &MouseInfo.Pos.y);
                        MouseInfo.Pos.x = MouseInfo.Pos.x - Window.getW_2();
                        MouseInfo.Pos.y = Window.getH() - MouseInfo.Pos.y - Window.getH_2();

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
                                MouseInfo.Pos = {0, 0};
                            }
                        }
                        break;
                    case SDL_WINDOWEVENT:
                        Window.handleEvent(Event.window);
                        break;
                }
                if (!running) {break;}
            }
            if (!running || Keystate[Keybinds.Quit]) {
                running = false;
                break;
            }

            t += dt;
            accumulator -= dt;
            MouseInfo.Motion = false;
        }
        if (!running) {break;}

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
