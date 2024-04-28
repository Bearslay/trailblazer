#include "Texture.hpp"

Texture::Texture() : Source(NULL), Center({}), Frame({}) {}
Texture::Texture(SDL_Texture* texture) : Source(texture), Center({}), Frame({}) {}
Texture::Texture(SDL_Texture* texture, SDL_Point center, SDL_Rect frame) : Source(texture), Center(center), Frame(frame) {}
Texture::Texture(SDL_Texture* texture, SDL_Rect frame) : Source(texture), Center({frame.w / 2, frame.h / 2}), Frame(frame) {}

SDL_Texture* Texture::getTexture() const {return Source;}
SDL_Point Texture::getCenter() const {return Center;}
SDL_Rect Texture::getFrame() const {return Frame;}
double Texture::getAngle() const {return Angle;}
SDL_RendererFlip Texture::getFlip() const {return Flip;}

SDL_BlendMode Texture::getBlending() const {return BlendMode;}
SDL_Color Texture::getColorMod() const {return Mods;}
Uint8 Texture::getOpacity() const {return Mods.a;}
Uint8 Texture::getRedMod() const {return Mods.r;}
Uint8 Texture::getGreenMod() const {return Mods.g;}
Uint8 Texture::getBlueMod() const {return Mods.b;}

SDL_Texture* Texture::setTexture(SDL_Texture* texture) {
    SDL_Texture* output = Source;
    Source = texture;
    SDL_SetTextureBlendMode(Source, BlendMode);
    SDL_SetTextureAlphaMod(Source, Mods.a);
    SDL_SetTextureColorMod(Source, Mods.r, Mods.g, Mods.b);
    return output;
}
SDL_Point Texture::setCenter(SDL_Point center) {
    SDL_Point output = Center;
    Center = center;
    return output;
}
SDL_Rect Texture::setFrame(SDL_Rect frame) {
    SDL_Rect output = Frame;
    Frame = frame;
    return output;
}
double Texture::setAngle(double angle) {
    double output = Angle;
    Angle = angle;
    return output;
}
SDL_RendererFlip Texture::setFlip(SDL_RendererFlip flip) {
    SDL_RendererFlip output = Flip;
    Flip = flip;
    return output;
}

SDL_BlendMode Texture::setBlending(SDL_BlendMode blendMode) {
    SDL_BlendMode output = BlendMode;
    BlendMode = blendMode;
    SDL_SetTextureBlendMode(Source, BlendMode);
    return output;
}
SDL_Color Texture::setMods(SDL_Color mods) {
    SDL_Color output = Mods;
    Mods = mods;
    SDL_SetTextureBlendMode(Source, BlendMode);
    SDL_SetTextureAlphaMod(Source, Mods.a);
    SDL_SetTextureColorMod(Source, Mods.r, Mods.g, Mods.b);
    return output;
}
Uint8 Texture::setOpacity(Uint8 opacity) {
    Uint8 output = Mods.a;
    Mods.a = opacity;
    SDL_SetTextureAlphaMod(Source, Mods.a);
    return output;
}
Uint8 Texture::setRedMod(Uint8 redMod) {
    Uint8 output = Mods.r;
    Mods.r = redMod;
    SDL_SetTextureColorMod(Source, Mods.r, Mods.g, Mods.b);
    return output;
}
Uint8 Texture::setGreenMod(Uint8 greenMod) {
    Uint8 output = Mods.g;
    Mods.g = greenMod;
    SDL_SetTextureColorMod(Source, Mods.r, Mods.g, Mods.b);
    return output;
}
Uint8 Texture::setBlueMod(Uint8 blueMod) {
    Uint8 output = Mods.b;
    Mods.b = blueMod;
    SDL_SetTextureColorMod(Source, Mods.r, Mods.g, Mods.b);
    return output;
}
