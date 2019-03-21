#include <Rendering/Tileset.h>
#include <System/Logger.h>

#include <cmath>
#include <utility>

using namespace Sourcehold::Rendering;
using namespace Sourcehold::System;

Tileset::Tileset(std::shared_ptr<Renderer> rend) :
    renderer(rend),
    Texture(rend)
{
}

Tileset::~Tileset()  {

}

void Tileset::Allocate(uint32_t num) {
    this->num = num;

    numRows = (uint32_t)(std::sqrt((float)num) + 0.5f);
    Texture::AllocNew(30 * numRows, 16 * numRows, SDL_PIXELFORMAT_RGBA8888);
}

void Tileset::SetTile(Texture &image, uint32_t index) {
    auto coords = IndexToCoords(index);

    image.LockTexture();
    Texture::LockTexture();
    Texture::Copy(image, coords.first, coords.second);
    image.UnlockTexture();
    Texture::UnlockTexture();
}

SDL_Rect Tileset::GetTile(uint32_t index) {
    auto coords = IndexToCoords(index);

    SDL_Rect rect;
    rect.x = coords.first;
    rect.y = coords.second;
    rect.w = 30;
    rect.h = 16;

    return rect;
}

void Tileset::Clear() {
    Texture::Destroy();
}

std::pair<uint32_t, uint32_t> Tileset::IndexToCoords(uint32_t index) {
    return std::pair<uint32_t, uint32_t>(30 * (index % numRows), 16 * (index / numRows));
}
