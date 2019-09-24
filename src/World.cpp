#include "Building.h"
#include "Entity.h"
#include "World.h"

#include "Parsers/TgxFile.h"
#include "Parsers/Gm1File.h"

#include "Rendering/Font.h"
#include "Rendering/Camera.h"

#include "GUI/MenuUtils.h"

using namespace Sourcehold::Game;

/**
 * Contains:
 * 1. Not selected index
 * 2. Mouse over index
 * 3. Selected index
 * 4. Relative x pos
 */
static uint16_t lut_ui_tabs[6][5] = {
    {  7,  8,  9, 264 },
    { 10, 11, 12, 300 },
    { 13, 14, 15, 336 },
    { 16, 17, 18, 372 },
    { 19, 20, 21, 408 },
    { 22, 23, 24, 444 },
};

World::World() :
    EventConsumer<Keyboard>(),
    EventConsumer<Mouse>()
{
    /* The scribes facial animation */
    gm1_scribe = GetGm1("gm/scribe.gm1");
    gm1_icons = GetGm1("gm/interface_buttons.gm1");
    gm1_floats = GetGm1("gm/floats.gm1");

    LoadMenuAssets();

    menuX = (GetWidth() / 2) - (menubar.GetWidth() / 2);
    menuY = GetHeight() - menubar.GetHeight();
    menuW = menubar.GetWidth();
    menuH = menubar.GetHeight();

    SetTarget(&menubar, Rect<int>{ menuX, menuY, menuW, menuH });

    /* Init the buttons */
    auto atlas = gm1_floats->GetTextureAtlas();
    ui_compass.SetTexture(atlas.get());
    ui_hide.SetTexture(atlas.get());
    ui_magnify.SetTexture(atlas.get());
    ui_lower.SetTexture(atlas.get());

    atlas = gm1_icons->GetTextureAtlas();
    SDL_Rect rect = atlas->Get(25);
    ui_disk.Translate(753, 72);
    ui_disk.Scale(rect.w, rect.h);
    ui_disk.SetTexture(atlas.get());

    rect = atlas->Get(27);
    ui_info.Translate(753, 110);
    ui_info.Scale(rect.w, rect.h);
    ui_info.SetTexture(atlas.get());

    rect = atlas->Get(29);
    ui_delete.Translate(753, 142);
    ui_delete.Scale(rect.w, rect.h);
    ui_delete.SetTexture(atlas.get());

    rect = atlas->Get(68);
    ui_revert.Translate(753, 168);
    ui_revert.Scale(rect.w, rect.h);
    ui_revert.SetTexture(atlas.get());

    for (int i = 0; i < 6; i++) {
        ui_tabs[i].Translate(lut_ui_tabs[i][3], 165);
        ui_tabs[i].Scale(30, 15);
    }

    ResetTarget();

    SetBounds({ 15, 8, 160 * 30 - 15, 90 * 16 - 8});
    SetCamPos(15, 8);
}

World::~World()
{
}

UIState World::Play()
{
    while(Running()) {
        ClearDisplay();

        UpdateCamera();

        GameMap::Render();

        if(rmbHolding) {
            RenderQuickMenu();
        }

        if(menubarShown) {
            UpdateMenubar();
            RenderMenubar();
        }

        if(escMenu) {
            /*DialogResult res = EscMenu();
            if(res == BACK) escMenu = false;
            else if(res == QUIT) {
                return EXIT_GAME;
            }else if(res == QUIT_MISSION) {
                return MAIN_MENU;
                }*/
        }

        RenderText(L"Sourcehold version " SOURCEHOLD_VERSION_STRING, 1, 1, FONT_SMALL);

        FlushDisplay();
        SyncDisplay();
    }

    return EXIT_GAME;
}

void World::RenderQuickMenu()
{
    auto atlas = gm1_floats->GetTextureAtlas();

    SDL_Rect rect = atlas->Get(37);
    ui_compass.Translate(int(mouseX - (rect.w / 2)), int(mouseY - rect.h - 25));
    ui_compass.Scale(rect.w, rect.h);

    rect = atlas->Get(124);
    ui_hide.Translate(int(mouseX - rect.w - 45), int(mouseY - (rect.h / 2)));
    ui_hide.Scale(rect.w, rect.h);

    rect = atlas->Get(45);
    ui_magnify.Translate(int(mouseX + 45), int(mouseY - (rect.h / 2)));
    ui_magnify.Scale(rect.w, rect.h);

    rect = atlas->Get(35);
    ui_lower.Translate(int(mouseX - (rect.w/2)), int(mouseY + 25));
    ui_lower.Scale(rect.w, rect.h);

    ui_compass.Render(
    [&]() -> SDL_Rect {
        if(ui_compass.IsMouseOver()) {
            Rotation rota = GetRotation();
        }
        return atlas->Get(37);
    });

    ui_hide.Render(
    [&]() -> SDL_Rect {
        if(ui_hide.IsMouseOver())
        {
            menubarShown = false;
            return atlas->Get(125);
        }
        else
        {
            menubarShown = true;
            return atlas->Get(124);
        }
    });

    ui_magnify.Render(
    [&]() -> SDL_Rect {
        return atlas->Get(45);
    });

    ui_lower.Render(
    [&]() -> SDL_Rect {
        if(ui_lower.IsMouseOver())
        {
            return atlas->Get(36);
        }
        else return atlas->Get(35);
    });
}

void World::RenderMenubar()
{
    ::Render(menubar, menuX, menuY);
}

void World::UpdateMenubar()
{
    SetTarget(&menubar, Rect<int>{ menuX, menuY, menuW, menuH });

    int32_t width = menubar.GetWidth(), height = menubar.GetHeight();

    auto atlas = gm1_face->GetTextureAtlas();
    SDL_Rect rect = atlas->Get(0);
    Rendering::Render(*atlas, width - 800 - tgx_bar_bg->GetWidth(), height - rect.h, &rect);

    atlas = gm1_scribe->GetTextureAtlas();
    rect = atlas->Get(0);
    Rendering::Render(*atlas, width - 800 - tgx_bar_bg->GetWidth() + 705, height - 200, &rect);
    Rendering::Render(*tgx_right, 800 + tgx_right->GetWidth(), height - tgx_right->GetHeight());
    Rendering::Render(*tgx_bar_bg, 0, height - tgx_bar_bg->GetHeight());

    atlas = gm1_icons->GetTextureAtlas();

    /* Render the menu buttons */
    ui_disk.Render(
        [&]() -> SDL_Rect {
            if (ui_disk.IsClicked() && !escMenu) {
                escMenu = true;
            }
            if (ui_disk.IsMouseOver() && !escMenu) return atlas->Get(26);
            else return atlas->Get(25);
        });

    ui_info.Render(
        [&]() -> SDL_Rect {
            if (ui_info.IsMouseOver() && !escMenu) return atlas->Get(28);
            else return atlas->Get(27);
        });

    ui_delete.Render(
        [&]() -> SDL_Rect {
            if (ui_delete.IsMouseOver() && !escMenu) return atlas->Get(30);
            else return atlas->Get(29);
        });

    ui_revert.Render(
        [&]() -> SDL_Rect {
            if (ui_revert.IsMouseOver() && !escMenu) return atlas->Get(69);
            else return atlas->Get(68);
        });

    for (uint8_t i = 0; i < 6; i++) {
        ui_tabs[i].Render(
            [&]() -> SDL_Rect {
                if (ui_tabs[i].IsClicked() && !escMenu) {
                    currentTab = static_cast<MenuPage>(i);
                }
                else {
                    if (ui_tabs[i].IsMouseOver() && !escMenu)return atlas->Get(lut_ui_tabs[i][1]);
                    else return atlas->Get(lut_ui_tabs[i][0]);
                }
                /* Highlight the selected tab */
                if (currentTab == i) return atlas->Get(lut_ui_tabs[i][2]);
                return { 0, 0, 0, 0 };
            });
    }

    /* Render the current pages content, TODO: may change when different building selected, etc. */
    switch (currentTab) {
    case MENU_CASTLE: {
    } break;
    case MENU_INDUSTRY: {
    } break;
    case MENU_FARM: {
    } break;
    case MENU_TOWN: {
    } break;
    case MENU_WEAPONS: {
    } break;
    case MENU_FOOD_PROCESSING: {
    } break;
    default:
        break;
    }

    ResetTarget();
}

void World::UpdateCamera()
{
    if(scroll.left) MoveLeft();
    if(scroll.right) MoveRight();
    if(scroll.up) MoveUp();
    if(scroll.down) MoveDown();
}

static const char* _res_to_edge[][2] = {
    { "edge1280", "edge_military_1280" },
    { "edge1024", "edge_military_1024" },
    { "edge1280", "edge_military_1280" },
    { "edge1280", "edge_military_1280" },
    { "edge1360", "edge_military_1360" },
    { "edge1366", "edge_military_1366" },
    { "edge1440", "edge_military_1440" },
    { "edge1600", "edge_military_1600" },
    { "edge1600", "edge_military_1600" },
    { "edge1680", "edge_military_1680" },
    { "edge1920", "edge_military_1920" }
};

void World::LoadMenuAssets()
{
    Resolution res = GetResolution();
    std::string base(_res_to_edge[res][0]);

    tgx_right = GetTgx(std::string("gfx/" + base + "r.tgx"));
    tgx_bar_bg = GetTgx(std::string("gfx/" + base + "l.tgx"));

    menubar.AllocNewTarget(tgx_bar_bg->GetWidth() + 800 + tgx_right->GetWidth(), 200);

    gm1_face = GetGm1("gm/face800-blank.gm1");
}

void World::onEventReceive(Keyboard &keyEvent)
{
    if(keyEvent.GetType() == KEYBOARD_KEYDOWN) {
        switch(keyEvent.Key().sym) {
        case SDLK_LEFT:
            scroll.left.shouldScroll = true;
            scroll.left.setByKeyboard = true;
            break;
        case SDLK_RIGHT:
            scroll.right.shouldScroll = true;
            scroll.right.setByKeyboard = true;
            break;
        case SDLK_UP:
            scroll.up.shouldScroll = true;
            scroll.up.setByKeyboard = true;
            break;
        case SDLK_DOWN:
            scroll.down.shouldScroll = true;
            scroll.down.setByKeyboard = true;
            break;
        case SDLK_SPACE:
            if(GetZoomLevel() == ZOOM_NEAR) ZoomOut();
            else ZoomIn();
            break;
        case SDLK_TAB:
            menubarShown = !menubarShown;
            break;
        case SDLK_ESCAPE:
            escMenu = !escMenu;
            break;
        default:
            break;
        }
    }
    else if(keyEvent.GetType() == KEYBOARD_KEYUP) {
        switch(keyEvent.Key().sym) {
        case SDLK_LEFT:
            scroll.left.shouldScroll = false;
            scroll.left.setByKeyboard = false;
            break;
        case SDLK_RIGHT:
            scroll.right.shouldScroll = false;
            scroll.right.setByKeyboard = false;
            break;
        case SDLK_UP:
            scroll.up.shouldScroll = false;
            scroll.up.setByKeyboard = false;
            break;
        case SDLK_DOWN:
            scroll.down.shouldScroll = false;
            scroll.down.setByKeyboard = false;
            break;
        default:
            break;
        }

        ResetMomentum();
    }
}

void World::onEventReceive(Mouse &mouseEvent)
{
    if(mouseEvent.GetType() == MOUSE_BUTTONDOWN) {
        if(mouseEvent.RmbDown()) {
            rmbHolding = true;
            mouseX = mouseEvent.GetPosX();
            mouseY = mouseEvent.GetPosY();
        }

        if(mouseEvent.LmbDown()) {
            GrabMouse();
        }
    }
    else if(mouseEvent.GetType() == MOUSE_BUTTONUP) {
        if(mouseEvent.RmbUp()) {
            rmbHolding = false;
        }
    }
    else if(mouseEvent.GetType() == MOUSE_MOTION) {
        const int scrollThreshold = 2;

        int x = mouseEvent.GetPosX();
        int y = mouseEvent.GetPosY();

        bool shouldReset = false;

        if(x < scrollThreshold) {
            scroll.left.shouldScroll = true;
            scroll.left.setByMouse = true;
        }
        else if(scroll.left.setByMouse) {
            scroll.left.shouldScroll = false;
            scroll.left.setByMouse = false;
            shouldReset = true;
        }

        if(x > GetWidth() - scrollThreshold) {
            scroll.right.shouldScroll = true;
            scroll.right.setByMouse = true;
        }
        else if(scroll.right.setByMouse) {
            scroll.right.shouldScroll = false;
            scroll.right.setByMouse = false;
            shouldReset = true;
        }

        if(y < scrollThreshold) {
            scroll.up.shouldScroll = true;
            scroll.up.setByMouse = true;
        }
        else if(scroll.up.setByMouse) {
            scroll.up.shouldScroll = false;
            scroll.up.setByMouse = false;
            shouldReset = true;
        }

        if(y > GetHeight() - scrollThreshold) {
            scroll.down.shouldScroll = true;
            scroll.down.setByMouse = true;
        }
        else if(scroll.down.setByMouse) {
            scroll.down.shouldScroll = false;
            scroll.down.setByMouse = false;
            shouldReset = true;
        }

        if(shouldReset) {
            ResetMomentum();
        }
    }
}
