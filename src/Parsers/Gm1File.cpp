#include <Parsers/Gm1File.h>

using namespace Sourcehold::Parsers;
using namespace Sourcehold::Rendering;
using namespace Sourcehold::System;

struct Gm1File::ImageHeader {
    /* Image dimensions */
    uint16_t width;
    uint16_t height;
    /* Image offsets */
    uint16_t offsetX;
    uint16_t offsetY;
    /* Image part */
    uint16_t part;
    uint16_t parts;
    /* Left/Right/Center */
    enum Direction : uint8_t {
        DIR_NONE =  0,
        DIR_DOWN =  1,
        DIR_RIGHT = 2,
        DIR_LEFT =  3,
    } direction;
    /* Horizontal offset */
    uint8_t horizOffset;
    /* Width of building part */
    uint8_t partWidth;
    /* Color */
    uint8_t color;
};

struct Gm1File::Gm1Entry {
    ImageHeader header;
    uint32_t size;
    uint32_t offset;
};

Gm1File::Gm1File(std::shared_ptr<Renderer> rend) : Parser(), TextureAtlas(rend), Tileset(rend)
{
    this->renderer = rend;
}

Gm1File::Gm1File(std::shared_ptr<Renderer> rend, const std::string &path) : Parser(), TextureAtlas(rend), Tileset(rend)
{
    this->renderer = rend;
    this->LoadFromDisk(path, false);
}

Gm1File::~Gm1File() {

}

bool Gm1File::LoadFromDisk(const std::string &path, bool threaded) {
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

    /* Reserve size in vectors to fit all entries */
    entries.resize(header.num);

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
    numCollections = 0;
    for(n = 0; n < header.num; n++) {
        Parser::GetData(&entries[n].header, sizeof(ImageHeader));
        if(entries[n].header.part == 0) numCollections++;
    }

    /* Get offset of data start */
    offData = Parser::Tell();

    /* Read compressed images into buffer */
    uint32_t buflen = Parser::GetLength() - offData;
    imgdata = new char[buflen];
    Parser::GetData(imgdata, buflen);

    /* Close file */
    Parser::Close();

    /* Allocate images */
    if(header.type == Gm1Header::TYPE_TILE) {
        /* One collection -> one texture */
        //TextureAtlas::Resize(header.num);
        //Tileset::Allocate(header.num);
        //Tileset::LockTexture();
        //GetCollections();
        //Tileset::UnlockTexture();
    }else {
        /* One entry -> one texture */
        TextureAtlas::Allocate(entries.size(), entries[0].header.width, entries[0].header.height);
        for(n = 0; n < entries.size(); n++) {
            //tex.LockTexture();
            //GetImage(n);
            //tex.UnlockTexture();
        }
    }

    delete [] imgdata;
    entries.clear();

    return true;
}

void Gm1File::DumpInformation() {
    Logger::message("PARSERS") << "Gm1 file:\nNum: " << header.num << "\nType: " << header.type << "\nLen: " << header.len << std::endl;
}

void Gm1File::Free() {
    entries.clear();
    TextureAtlas::Clear();
    Tileset::Clear();
}

bool Gm1File::GetCollections() {
    for(uint32_t n = 0; n < entries.size(); n++) {
        GetImage(n);
    }
    return true;
}

bool Gm1File::GetImage(uint32_t index) {    
    /* Seek to position */
    char *position = imgdata + entries[index].offset;

    switch(header.type) {
        case Gm1Header::TYPE_INTERFACE: case Gm1Header::TYPE_FONT: case Gm1Header::TYPE_CONSTSIZE: {
            //TgxFile::ReadTgx(static_cast<Texture>(this->TextureAtlas), position, entries[index].size, 0, 0, nullptr);
        }break;
        case Gm1Header::TYPE_ANIMATION: {
            //TgxFile::ReadTgx(TextureAtlas::Get(index), position, entries[index].size, 0, 0, palette);
        }break;
        case Gm1Header::TYPE_TILE: {
            /* Read tile */
            SDL_Rect tile = Tileset::GetTile(index);

            /* Extract pixel data */
            const static uint8_t lines[16] = {
                2, 6, 10, 14, 18, 22, 26, 30,
                30, 26, 22, 18, 14, 10, 6, 2
            };

            /* Read every line */
            for(uint8_t l = 0; l < 16; l++) {
                /* Read every pixel in a line */
                for(uint8_t i = 0; i < lines[l]; i++) {
                    uint16_t pixel = *reinterpret_cast<uint16_t*>(position);
                    position += 2;

                    /* Read RGB */
                    uint8_t r, g, b;
                    TgxFile::ReadPixel(pixel, r, g, b);

                    /* Add to tileset texture */
                    Tileset::SetPixel(
                        tile.x + (15 - lines[l] / 2 + i),
                        tile.y + l,
                        r, g, b, 0xFF
                    );
                }
            }
        }break;
        case Gm1Header::TYPE_MISC1: case Gm1Header::TYPE_MISC2: {
//            Texture &target = TextureAtlas::Get(index);
//
//            for(uint32_t x = 0; x < entries[index].header.width; x++) {
//                for(uint32_t y = 0; y < entries[index].header.height; y++) {
//                    uint16_t pixel = *reinterpret_cast<uint16_t*>(position);
//                    position += 2;
//
//                    /* Read RGB */
//                    uint8_t r, g, b;
//                    TgxFile::ReadPixel(pixel, r, g, b);
//            
//                    /* Add to texture */
//                    target.SetPixel(x, y, r, g, b, 0xFF);
//                }
//            }
        }break;
        default: {
            Logger::error("PARSERS") << "Unknown filetype stored in Gm1 '" << path << "': " << header.type << std::endl;
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
