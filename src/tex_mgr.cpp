#include "freesolitaire/stdafx.h"
#include "freesolitaire/tex_mgr.h"

using FreeSolitaire::TextureMgr;
using std::cerr;
using std::endl;
using std::wstring;

TextureMgr::TextureMgr() : lastId(0) {}
void TextureMgr::loadTexture(const wchar_t* path, int filterMode) {
    unsigned int id;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterMode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterMode);
    size_t bufSz = wcslen(path) + 1;
    char* utf8 = new char[bufSz];
    stbi_convert_wchar_to_utf8(utf8, bufSz, path);
    int w, h, c;
    stbi_uc* data = stbi_load(utf8, &w, &h, &c, STBI_default);
    delete[] utf8;
    utf8 = nullptr;
    bool failed = false;
    if (data == nullptr) {
        failed = true;
        cerr << "Failed to load image " << path << ", reason: " << stbi_failure_reason() << endl;
        stbi_uc tmp[] = {
            0xFF, 0x00, 0xFF, 0xFF, 0xFF, 0xFF,
            0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0xFF,
        };
        data = tmp;
        w = 2;
        h = 2;
        c = STBI_rgb;
    }
    gluBuild2DMipmaps(GL_TEXTURE_2D,
        c,
        w,
        h,
        c == STBI_rgb ? GL_RGB : GL_RGBA,
        GL_UNSIGNED_BYTE,
        data);
    if (!failed)
        stbi_image_free(data);
    loadedIds[path] = id;
}
void TextureMgr::bindTexture(unsigned int id) {
    if (lastId != id) {
        lastId = id;
        glBindTexture(GL_TEXTURE_2D, id);
    }
}
void TextureMgr::bindTexture(const wchar_t* path) {
    bindTexture(loadedIds[path]);
}
TextureMgr::~TextureMgr() {
    for (auto& p : loadedIds) {
        glDeleteTextures(1, &p.second);
    }
}
