#pragma once
#include <map>
#include "tex_mgr.h"

namespace FreeSolitaire {
    struct Sprite {
        int x, y, w, h;
        float u0, v0, u1, v1;
        std::string_view name;

        Sprite(int __x = 0, int __y = 0, int __w = 0, int __h = 0,
            float __u0 = 0, float __v0 = 0, float __u1 = 0, float __v1 = 0,
            std::string_view __name = "");
    };

    class SpriteAtlasTexture {
    private:
        std::map<std::string_view, Sprite> _sprites;
        unsigned int _glId;
    public:
        SpriteAtlasTexture(Sprite* __sprites, size_t __numSprites, unsigned int __glId);
        Sprite& getSprite(std::string_view name);
        unsigned int getGlId();
        void bind(TextureMgr& __mgr);
        ~SpriteAtlasTexture();
    };
}
