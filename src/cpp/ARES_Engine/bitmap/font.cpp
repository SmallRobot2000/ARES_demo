#include <ARES_Engine/bitmap/font.hpp>
#include <stdexcept>

namespace ARES_Engine::Bitmap
{
    Font::Font(const std::string &path)
    {
        auto error = FT_Init_FreeType(&this->m_library);
        if (error)
        {
            throw std::runtime_error("Error");
        }

        error = FT_New_Face(this->m_library,
                            path.c_str(),
                            0,
                            &this->m_face);
        if (error == FT_Err_Unknown_File_Format)
        {
            throw std::runtime_error("The font file could be opened and read, but it appears that its font format is unsupported");
        }
        else if (error)
        {
            throw std::runtime_error("The font file could not be opened or read");
        }
    }

}