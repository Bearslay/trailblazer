#ifndef RENDERWINDoW
#define RENDERWINDoW

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <iostream>

#include "PresetColors.hpp"
#include "Texture.hpp"

class RenderWindow {
    private:
        SDL_Window* Window;
        SDL_Renderer* Renderer;
        int W;
        int H;
        int W_2;
        int H_2;
        bool IsFullscreen = false;

    public:
        RenderWindow(const char* title, const int &w, const int &h, Uint32 flags = SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE);
        ~RenderWindow();

        int getRefreshRate() const;
        Uint32 getWindowFlags();

        int getW() const;
        int setW(const int &w);
        int adjustW(const int &amount);
        int getH() const;
        int setH(const int &h);
        int adjustH(const int &amount);
        SDL_Point getDims() const;
        SDL_Point setDims(const int &w, const int &h);
        SDL_Point adjustDims(const int &w, const int &h);
        SDL_Point updateDims();
        int getW_2() const;
        int getH_2() const;

        const char* getTitle() const;
        const char* setTitle(const char* title);

        void clear(const SDL_Color &color = PresetColors[COLOR_BLACK]);
        void show();

        bool toggleFullscreen(const bool &trueFullscreen = true);
        void centerMouse();

        void handleEvent(const SDL_WindowEvent &event);

        void drawPixel(const int &x, const int &y, const SDL_Color &color = PresetColors[COLOR_WHITE]);
        void drawLine(const int &x1, const int &y1, const int &x2, const int &y2, const SDL_Color &color = PresetColors[COLOR_WHITE]);
        void drawRectangle(const int &x, const int &y, const int &w, const int &h, const SDL_Color &color = PresetColors[COLOR_WHITE]);
        void fillRectangle(const int &x, const int &y, const int &w, const int &h, const SDL_Color &color = PresetColors[COLOR_WHITE]);
        void drawCircle(const int &x, const int &y, const int &r, const SDL_Color &color = PresetColors[COLOR_WHITE]);
        void fillCircle(const int &x, const int &y, const int &r, const SDL_Color &color = PresetColors[COLOR_WHITE]);

        SDL_Texture* loadTexture(const std::string &path);

        void renderTexture(SDL_Texture* texture, const SDL_Rect &src, const SDL_Rect &dst, const double &angle, const SDL_Point &center, const SDL_RendererFlip &flip);
        void renderTexture(const Texture &texture, const SDL_Rect &dst);
        void renderTexture(const Texture &texture, const SDL_Point &pos);

        void renderText(TTF_Font *font, const char16_t* text, const SDL_Point &pos, const Uint32 wrapWidth = 0, const SDL_Color &color = PresetColors[COLOR_WHITE]);
};

#endif /* RENDERWINDOW */
