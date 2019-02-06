#include <Rendering/Renderer.h>

using namespace Sourcehold::Rendering;
using namespace Sourcehold::System;

Renderer::Renderer() {

}

Renderer::~Renderer() {
    
}

void Renderer::Flush() {
    SDL_RenderPresent(renderer);
}

void Renderer::RenderTextureXY(Texture &texture, int x, int y) {    
    SDL_Rect rect = {
        x, y,
        texture.GetWidth(),
        texture.GetHeight()
    };
    SDL_RenderCopy(
        renderer,
        texture.GetTexture(),
        NULL,
        &rect
    );
}

void Renderer::RenderTextureFullscreen(Texture &texture) {
    SDL_RenderCopy(
        renderer,
        texture.GetTexture(),
        NULL,
        NULL
    );
}

Context Renderer::CreateContext() {
    Context ctx;
    ctx.window = window;
    ctx.renderer = renderer;
    return ctx;
}
