#pragma once

#include <cinttypes>
#include <memory>
#include <vector>

#include <GameManager.h>

#include <Rendering/Renderer.h>

#include <Parsers/TgxFile.h>
#include <Parsers/Gm1File.h>

#include <Events/Keyboard.h>
#include <Events/Mouse.h>

namespace Sourcehold
{
    namespace Game
    {
        using namespace Rendering;
        using namespace Parsers;
        using namespace Events;

        struct MapTile {
            uint16_t tileset;
        };

        class GameMap : protected EventConsumer<Keyboard>, protected EventConsumer<Mouse>
        {
                int mult = 1;
            public:
                GameMap(std::shared_ptr<GameManager> man);
                GameMap(const GameMap&) = delete;
                ~GameMap() = default;

                void Render();
            protected:
                void onEventReceive(Keyboard &keyEvent) override;
                void onEventReceive(Mouse &mouseEvent) override;

                uint16_t maypole;
                std::shared_ptr<Gm1File> gm1_tile, gm1_maypole, gm1_churches;
                std::shared_ptr<GameManager> manager;
                std::shared_ptr<Tileset> tileset;
                std::vector<SDL_Rect> tiles;
        };
    }
}
