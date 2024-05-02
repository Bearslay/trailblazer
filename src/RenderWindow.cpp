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

void RenderWindow::renderText(TTF_Font *font, const char16_t* text, const SDL_Point &pos, const Uint32 wrapWidth, const SDL_Color &color) {
    SDL_Surface *surface = TTF_RenderUNICODE_Blended_Wrapped(font, (Uint16*)text, color, wrapWidth);
    renderTexture(SDL_CreateTextureFromSurface(Renderer, surface), {0, 0, surface->w, surface->h}, {pos.x - surface->w / 2, pos.y + surface->h / 2, surface->w, surface->h}, 0, {0, 0}, SDL_FLIP_NONE);
    SDL_FreeSurface(surface);
}
