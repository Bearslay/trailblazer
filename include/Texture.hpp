#ifndef TEXTURE
#define TEXTURE

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

class Texture {
    private:
        SDL_Texture* Source = NULL;
        SDL_Point Center;
        SDL_Rect Frame;
        double Angle = 0;
        SDL_RendererFlip Flip = SDL_FLIP_NONE;

        SDL_BlendMode BlendMode = SDL_BLENDMODE_NONE;
        SDL_Color Mods = {255, 255, 255, SDL_ALPHA_OPAQUE};

    public:
        Texture();
        Texture(SDL_Texture* texture);
        Texture(SDL_Texture* texture, SDL_Point center, SDL_Rect frame);
        Texture(SDL_Texture* texture, SDL_Rect frame);
        ~Texture() {SDL_DestroyTexture(Source);}

        SDL_Texture* getTexture() const;
        SDL_Point getCenter() const;
        SDL_Rect getFrame() const;
        double getAngle() const;
        SDL_RendererFlip getFlip() const;
        
        SDL_BlendMode getBlending() const;
        SDL_Color getColorMod() const;
        Uint8 getOpacity() const;
        Uint8 getRedMod() const;
        Uint8 getGreenMod() const;
        Uint8 getBlueMod() const;

        SDL_Texture* setTexture(SDL_Texture* texture);
        SDL_Point setCenter(SDL_Point center);
        SDL_Rect setFrame(SDL_Rect frame);
        double setAngle(double angle);
        SDL_RendererFlip setFlip(SDL_RendererFlip flip);

        SDL_BlendMode setBlending(SDL_BlendMode blendMode);
        SDL_Color setMods(SDL_Color mods);
        Uint8 setOpacity(Uint8 opacity);
        Uint8 setRedMod(Uint8 redMod);
        Uint8 setGreenMod(Uint8 greenMod);
        Uint8 setBlueMod(Uint8 blueMod);
};

#endif /* TEXTURE */
