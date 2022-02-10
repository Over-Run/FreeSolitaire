#include "freesolitaire/stdafx.h"
#include "freesolitaire/atlas.h"

using FreeSolitaire::Sprite;
using FreeSolitaire::SpriteAtlasTexture;

Sprite::Sprite(int __x, int __y, int __w, int __h,
    float __u0, float __v0, float __u1, float __v1,
    std::string_view __name) :
    x(__x),
    y(__y),
    w(__w),
    h(__h),
    u0(__u0),
    v0(__v0),
    u1(__u1),
    v1(__v1),
    name(__name) {}

SpriteAtlasTexture::SpriteAtlasTexture(Sprite* __sprites,
    size_t __numSprites,
    unsigned int __glId)
    : _glId(__glId) {
    for (size_t i = 0; i < __numSprites; ++i) {
        Sprite s = __sprites[i];
        _sprites[s.name] = s;
    }
}
Sprite& SpriteAtlasTexture::getSprite(std::string_view name) {
    return _sprites[name];
}
unsigned int SpriteAtlasTexture::getGlId() {
    return _glId;
}
void SpriteAtlasTexture::bind(TextureMgr& __mgr) {
    __mgr.bindTexture(_glId);
}
SpriteAtlasTexture::~SpriteAtlasTexture() {
    glDeleteTextures(1, &_glId);
}
