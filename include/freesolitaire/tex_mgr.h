// This is the texture manager.
#pragma once
#include <map>
#include <string>

namespace FreeSolitaire {
    class TextureMgr {
    private:
        std::map<std::wstring_view, unsigned int> _loadedIds;
        unsigned int _lastId;
    public:
        TextureMgr();
        /// <summary>
        /// Load a texture from specified path.
        /// <para>
        /// The memory of std::wstring is auto allocated and freed by program (using function free).
        /// </para>
        /// </summary>
        /// <param name="__path">The texture path. Can be relative or absolute.</param>
        /// <param name="__filterMode">The texture min and mag filter used by GL.</param>
        void loadTexture(const wchar_t* __path, int __filterMode);
        void bindTexture(const wchar_t* __path);
        void bindTexture(unsigned int __id);
        unsigned int getTexture(const wchar_t* __path);
        ~TextureMgr();
    };
}
