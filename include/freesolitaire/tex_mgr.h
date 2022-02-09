#pragma once
#include <map>
#include <string>

namespace FreeSolitaire {
    class TextureMgr {
    private:
        std::map<std::wstring_view, unsigned int> loadedIds;
        unsigned int lastId;
    public:
        TextureMgr();
        /// <summary>
        /// Load a texture from specified path.
        /// <para>
        /// The memory of std::wstring is auto allocated and freed by program (using function free).
        /// </para>
        /// </summary>
        /// <param name="path">The texture path. Can be relative or absolute.</param>
        /// <param name="filterMode">The texture min and mag filter used by GL.</param>
        void loadTexture(const wchar_t* path, int filterMode);
        void bindTexture(const wchar_t* path);
        void bindTexture(unsigned int id);
        ~TextureMgr();
    };
}
