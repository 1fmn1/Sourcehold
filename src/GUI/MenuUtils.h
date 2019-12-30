#pragma once

#include <string>

#include "System/ModCampaign.h"

#include "GUI/Widgets/Table.h"
#include "GUI/Widgets/LineEdit.h"

#include "Events/Event.h"
#include "Events/Mouse.h"

namespace Sourcehold {
    namespace GUI {
        using namespace Events;

        void InitMenuUtils();
        std::shared_ptr<TextureAtlas> GetInterfaceIcons();

        /**
         * Renders the given string to fit the main menu
         * rectangle.
         */
        void RenderMenuText(const std::wstring &text);

        /**
         * Renders the border around the screen. This asset
         * only exists for the HD edition to prevent scaling
         * the interface to fit the screen.
         */
        void RenderMenuBorder();

        /**
         * Given the mouse position relative to the button,
         * check if the mouse collides with the actual
         * image and not the transparent background
         */
        bool CheckButtonCollision(uint32_t rx, uint32_t ry);

        /* Ingame dialog windows */
        enum DialogResult {
            IDLE,
            LOAD,
            SAVE,
            BACK,
            QUIT,
            QUIT_MISSION
        };

        enum class DialogType {
            // Original game //
            QUIT,
            LOAD,
            COMBAT_MENU,
            SIEGE_MENU,
            ECO_MENU,
            SETTINGS,
            ESC_MENU,
            // Sourcehold //
            CAMPAIGN_SELECT_MILITARY,
            CAMPAIGN_SELECT_ECO
        };

        // TODO: names?
        enum DialogButton : uint8_t {
            BUTTON_1 = 0,
            BUTTON_2,
            BUTTON_3,
            BUTTON_4,
            BUTTON_5,
            BUTTON_6,
            BUTTON_7,
            BUTTON_8,
            BUTTON_9
        };

        enum class Deco {
            LARGE,
            SMALL
        };

        /* Rendering functions for common dialog window elements */
        void RenderDialogBorder(int x, int y, int nx, int ny);
        void RenderDeco(Deco type, int x, int y);
        void RenderDialogTextBox(int x, int y, int w, int h, const std::wstring& text, bool deco);
     }
}
