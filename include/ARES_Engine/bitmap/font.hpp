// I used FreeType library for vector text rendering
// sudo apt install libfreetype6-dev
// Space needed: ~6364 kB

#pragma once

#include <string>

#include <ft2build.h>
#include FT_FREETYPE_H

namespace ARES_Engine::Bitmap
{
    class Font
    {
    public:
        Font(const std::string &path);
        ~Font();

    private:
        FT_Library m_library; /* handle to library     */
        FT_Face m_face;       /* handle to face object */
    };
}