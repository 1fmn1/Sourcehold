#include <Parsers/Gm1File.h>

using namespace Sourcehold::Parsers;
using namespace Sourcehold::Rendering;
using namespace Sourcehold::System;

Gm1File::Gm1File() :
    Parser()
{ }

Gm1File::~Gm1File() {

}

bool Gm1File::LoadFromDisk(const std::string &path, Rendering::TextureAtlas &atlas) {
    this->path = path;

    if(!Parser::Open(path, std::fstream::in | std::ios::binary)) {
        Logger::error("PARSERS")  << "Unable to open Gm1 file '" << path << "'!" << std::endl;
        return false;
    }

    if(!Parser::GetData(&header, sizeof(Gm1Header))) {
        Logger::error("PARSERS") << "Unable to load Gm1 file header from '" << path << "'!" << std::endl;
        Parser::Close();
        return false;
    }

    /* Boundary check */
    if(header.num > max_num) {
        Logger::error("PARSERS") << "Gm1 file header from '" << path << "' contains too many images!" << std::endl;
        Parser::Close();
        return false;
    }

    /* Reserve size in vector to fit all entries */
    entries.resize(header.num);
    atlas.Resize(header.num);

    /* Read Gm1 palette */
    ReadPalette();

    uint32_t n;

    /* Read offsets */
    for(n = 0; n < header.num; n++) {
        Parser::GetData(&entries[n].offset, sizeof(uint32_t));
    }

    /* Read sizes */
    for(n = 0; n < header.num; n++) {
        Parser::GetData(&entries[n].size, sizeof(uint32_t));
    }

    /* Read image headers */
    for(n = 0; n < header.num; n++) {
        Parser::GetData(&entries[n].header, sizeof(ImageHeader));
    }

    /* Get offset of data start */
    offData = Parser::Tell();

    for(n = 0; n < header.num; n++) {
        GetImage(atlas.Get(n), n);
    }

    /* Close file */
    Parser::Close();

    return true;
}

void Gm1File::DumpInformation() {
    Logger::message("PARSERS") << "Gm1 file:\nNum: " << header.num << "\nType: " << header.type << "\nLen: " << header.len << std::endl;
}

std::vector<Gm1File::Gm1Entry>& Gm1File::GetEntries() {
    return entries;
}

Gm1File::Gm1Entry &Gm1File::GetEntry(uint32_t index) {
    return entries[index];
}

bool Gm1File::GetImage(Texture &tex, uint32_t index) {
    if(index >= header.num) return false;

    /* Seek to position */
    Parser::Seek(entries[index].offset + offData);

    /* Error handling */
    if(!Parser::Ok()) {
        Logger::error("PARSERS") << "Error while parsing entry " << index << " from Gm1 '" << path << "'!" << std::endl;
        Parser::Close();
        return false;
    }

    switch(header.type) {
        case Gm1Header::TYPE_INTERFACE: case Gm1Header::TYPE_FONT: case Gm1Header::TYPE_CONSTSIZE: {
            tex.AllocNew(entries[index].header.width, entries[index].header.height, SDL_PIXELFORMAT_RGBA8888);
            TgxFile::ReadTgx(*this, tex, entries[index].size, entries[index].header.width, entries[index].header.height, NULL);
            tex.UpdateTexture();
        }break;
        case Gm1Header::TYPE_ANIMATION: {
            tex.AllocNew(entries[index].header.width, entries[index].header.height, SDL_PIXELFORMAT_RGBA8888);
            TgxFile::ReadTgx(*this, tex, entries[index].size, entries[index].header.width, entries[index].header.height, palette);
            tex.UpdateTexture();
        }break;
        case Gm1Header::TYPE_TILE: {
            /* Create texture */
            tex.AllocNew(30, 16, SDL_PIXELFORMAT_RGBA8888);
                
            /* Extract pixel data */
            const uint8_t lines[16] = {
                2, 6, 10, 14, 18, 22, 26, 30,
                30, 26, 22, 18, 14, 10, 6, 2
            };

            /* Read every line */
            for(uint8_t l = 0; l < 16; l++) {
                /* Read every pixel in a line */
                for(uint8_t i = 0; i < lines[l]; i++) {
                    uint16_t pixel = Parser::GetWord();
            
                    /* Read RGB */
                    uint8_t r, g, b;
                    TgxFile::ReadPixel(pixel, r, g, b);
            
                    /* Add to texture */
                    tex.SetPixel(i, l, r, g, b, 0xFF);
                }
            }

            /* Write pixels to texture */
            tex.UpdateTexture();
        }break;
        case Gm1Header::TYPE_MISC1: case Gm1Header::TYPE_MISC2: {
            /* Read miscellaneous entry */
            tex.AllocNew(entries[index].header.width, entries[index].header.height, SDL_PIXELFORMAT_RGBA8888);
            for(uint32_t x = 0; x < entries[index].header.width; x++) {
                for(uint32_t y = 0; y < entries[index].header.height; y++) {
                    uint16_t pixel = Parser::GetWord();

                    /* Read RGB */
                    uint8_t r, g, b;
                    TgxFile::ReadPixel(pixel, r, g, b);
            
                    /* Add to texture */
                    tex.SetPixel(x, y, r, g, b, 0xFF);
                }
            }
            
            /* Write pixels to texture */
            tex.UpdateTexture();
        }break;
        default: {
            Logger::error("PARSERS") << "Unknown filetype stored in Gm1 '" << path << "': " << header.type << std::endl;
            Parser::Close();
            return false;
        }
    }

    return true;
}

void Gm1File::ReadPalette() {
    if(!Parser::GetData(palette, sizeof(palette))) {
        Logger::error("PARSERS") << "Unable to read palette from Gm1!" << std::endl;
        Parser::Close();
    }
}
