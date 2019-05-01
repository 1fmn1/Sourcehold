#include <Building.h>
#include <Entity.h>
#include <World.h>
#include <Config.h>

#include <Parsers/TgxFile.h>
#include <Parsers/Gm1File.h>

#include <Rendering/Font.h>

using namespace Sourcehold::Game;

/**
 * Indices into the texture atlas for every user interface tab.
 * Contains indices for:
 * 1. Not selected
 * 2. Mouse over
 * 3. Selected
 */
static uint16_t _ui_tabs_indices[6][3] = {
    { 7, 8, 9 },
    { 10, 11, 12 },
    { 13, 14, 15 },
    { 16, 17, 18 },
    { 19, 20, 21 },
    { 22, 23, 24 },
};

World::World(std::shared_ptr<GameManager> mgr) :
    GameMap(mgr),
    manager(mgr),
    EventConsumer<Keyboard>(mgr->GetHandler()),
    EventConsumer<Mouse>(mgr->GetHandler()),
    menubar(mgr),
    ui_disk(mgr),
    ui_info(mgr),
    ui_delete(mgr),
    ui_revert(mgr),
    ui_tabs { mgr, mgr, mgr, mgr, mgr, mgr },
    ui_compass(mgr),
    ui_magnify(mgr),
    ui_lower(mgr),
    ui_hide(mgr)
{
    /* The repeating, wooden background of the menu bar */
    tgx_bar_bg = mgr->GetTgx(mgr->GetDirectory() / "gfx/1280r.tgx").lock();
    /* The right side of the scribe/book */
    tgx_right = mgr->GetTgx(mgr->GetDirectory() / "gfx/edge1280r.tgx").lock();
    /* The scribe's facial animation */
    gm1_scribe = mgr->GetGm1(mgr->GetDirectory() / "gm/scribe.gm1").lock();
    /* The menu faces */
    gm1_face = mgr->GetGm1(mgr->GetDirectory() / "gm/face800-blank.gm1").lock();
    gm1_icons = mgr->GetGm1(mgr->GetDirectory() / "gm/interface_buttons.gm1").lock();
    gm1_floats = mgr->GetGm1(mgr->GetDirectory() / "gm/floats.gm1").lock();

    menubar.AllocNewTarget(240 + 800 + 240, 200);

    manager->SetBounds({ 15, 8, 160 * 30 - 15, 90 * 16 - 8});
    manager->TranslateCam(15, 8);
}

World::~World() {
}

int World::Play() {
    while(manager->Running()) {
        manager->Clear();
        manager->StartTimer();

        UpdateCamera();

        GameMap::Render();

        if(rmbHolding) {
            rmbHeld = SDL_GetTicks() - rmbPressed;
            if(rmbHeld > 90) {
                RenderQuickMenu();
            }
        }

        if(menubarShown) {
            RenderMenubar();
            UpdateMenubar();
        }

        RenderText(L"Sourcehold version " SOURCEHOLD_VERSION_STRING, 1, 1, 0.5, FONT_SMALL);

        manager->EndTimer();
        manager->Flush();
    }

    return 0;
}

void World::RenderQuickMenu() {
    auto atlas = gm1_floats->GetTextureAtlas().lock();

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
        [&]() -> Texture& {
            if(ui_compass.IsMouseOver()) {
                Camera::Rotation rota = manager->GetRotation();
            }
            atlas->SetRect(atlas->Get(37));
            return *atlas;
        });

    ui_hide.Render(
        [&]() -> Texture& {
            if(ui_hide.IsMouseOver()) {
                atlas->SetRect(atlas->Get(125));
                menubarShown = false;
            } else {
                atlas->SetRect(atlas->Get(124));
                menubarShown = true;
            }
            return *atlas;
        });

    ui_magnify.Render(
        [&]() -> Texture& {
            atlas->SetRect(atlas->Get(45));
            return *atlas;
        });

    ui_lower.Render(
        [&]() -> Texture& {
            if(ui_lower.IsMouseOver()) {
                atlas->SetRect(atlas->Get(36));
            }else atlas->SetRect(atlas->Get(35));
            return *atlas;
        });
}

void World::RenderMenubar() {
    manager->Render(menubar, menuX, menuY, menuW, menuH);
}

void World::UpdateMenubar() {
    manager->SetTarget(&menubar, menuX, menuY, menuW, menuH);

    int32_t width = menubar.GetWidth(), height = menubar.GetHeight();

    /* TODO: Less hardcoding */
    auto atlas = gm1_face->GetTextureAtlas().lock();
    SDL_Rect rect = atlas->Get(0);
    manager->Render(*atlas, width - 800 - 240, height - rect.h, &rect);

    /**
     * Render the menu background
     * TODO: Change depending on the window size, since Stronghold HD
     * supports multiple resolutions and does not scale menu elements at all,
     * and will instead fill the sides of the actual menu with the same, repeating
     * background image, but flipped every second time
     */
    atlas = gm1_scribe->GetTextureAtlas().lock();
    rect = atlas->Get(0);
    manager->Render(*atlas, width - 800 - 240 + 705, height - 200, &rect);
    manager->Render(*tgx_right, width - tgx_right->GetWidth(), height - tgx_right->GetHeight());
    manager->Render(*tgx_bar_bg, 0, height - tgx_bar_bg->GetHeight());

    atlas = gm1_icons->GetTextureAtlas().lock();

    /* Init menu buttons */
    rect = atlas->Get(25);
    ui_disk.Translate(753, 72);
    ui_disk.Scale(rect.w, rect.h);
    rect = atlas->Get(27);
    ui_info.Translate(753, 110);
    ui_info.Scale(rect.w, rect.h);
    rect = atlas->Get(29);
    ui_delete.Translate(753, 142);
    ui_delete.Scale(rect.w, rect.h);
    rect = atlas->Get(68);
    ui_revert.Translate(753, 168);
    ui_revert.Scale(rect.w, rect.h);

    ui_tabs[0].Translate(264, 165);
    ui_tabs[0].Scale(30, 35);
    ui_tabs[1].Translate(300, 165);
    ui_tabs[1].Scale(30, 35);
    ui_tabs[2].Translate(336, 165);
    ui_tabs[2].Scale(30, 35);
    ui_tabs[3].Translate(372, 165);
    ui_tabs[3].Scale(30, 35);
    ui_tabs[4].Translate(408, 165);
    ui_tabs[4].Scale(30, 35);
    ui_tabs[5].Translate(444, 165);
    ui_tabs[5].Scale(30, 35);

    /* Render the menu buttons */
    ui_disk.Render(
        [&]() -> Texture& {
            if(ui_disk.IsMouseOver()) atlas->SetRect(atlas->Get(26));
            else atlas->SetRect(atlas->Get(25));
            return *atlas;
        });

    ui_info.Render(
        [&]() -> Texture& {
            if(ui_info.IsMouseOver()) atlas->SetRect(atlas->Get(28));
            else atlas->SetRect(atlas->Get(27));
            return *atlas;
        });

    ui_delete.Render(
        [&]() -> Texture& {
            if(ui_delete.IsMouseOver()) atlas->SetRect(atlas->Get(30));
            else atlas->SetRect(atlas->Get(29));
            return *atlas;
        });

    ui_revert.Render(
        [&]() -> Texture& {
            if(ui_revert.IsMouseOver()) atlas->SetRect(atlas->Get(69));
            else atlas->SetRect(atlas->Get(68));
            return *atlas;
        });

    for(uint8_t i = 0; i < 6; i++) {
        ui_tabs[i].Render(
            [&]() -> Texture& {
                if(ui_tabs[i].IsMouseOver()) atlas->SetRect(atlas->Get(_ui_tabs_indices[i][1]));
                else atlas->SetRect(atlas->Get(_ui_tabs_indices[i][0]));
                if(ui_tabs[i].IsClicked()) {
                    currentTab = static_cast<MenuPage>(i);
                }
                /* Highlight the selected tab */
                if(currentTab == i) atlas->SetRect(atlas->Get(_ui_tabs_indices[i][2]));
                return *atlas;
            });
    }

    /* Render the current pages content, TODO: may change when different building selected, etc. */
    switch(currentTab) {
    case MENU_CASTLE: {
    }break;
    case MENU_INDUSTRY: {
    }break;
    case MENU_FARM: {
    }break;
    case MENU_TOWN: {
    }break;
    case MENU_WEAPONS: {
    }break;
    case MENU_FOOD_PROCESSING: {
    }break;
    default: break;
    }

    manager->ResetTarget();
}

void World::UpdateCamera() {
    if(scroll.left) manager->MoveLeft();
    if(scroll.right) manager->MoveRight();
    if(scroll.up) manager->MoveUp();
    if(scroll.down) manager->MoveDown();
}

void World::onEventReceive(Keyboard &keyEvent) {
    if(keyEvent.GetType() == KEYBOARD_KEYDOWN) {
        switch(keyEvent.Key().sym) {
        case SDLK_LEFT: scroll.left.shouldScroll = true; scroll.left.setByKeyboard = true; break;
        case SDLK_RIGHT: scroll.right.shouldScroll = true; scroll.right.setByKeyboard = true; break;
        case SDLK_UP: scroll.up.shouldScroll = true; scroll.up.setByKeyboard = true; break;
        case SDLK_DOWN: scroll.down.shouldScroll = true; scroll.down.setByKeyboard = true; break;
        case SDLK_SPACE:
            if(manager->GetZoomLevel() == Camera::ZOOM_NEAR) manager->ZoomOut();
            else manager->ZoomIn();
            break;
        case SDLK_TAB: menubarShown = !menubarShown; break;
        case SDLK_ESCAPE: manager->ReleaseMouse(); break;
        default: break;
        }
    }else if(keyEvent.GetType() == KEYBOARD_KEYUP) {
        switch(keyEvent.Key().sym) {
        case SDLK_LEFT: scroll.left.shouldScroll = false; scroll.left.setByKeyboard = false; break;
        case SDLK_RIGHT: scroll.right.shouldScroll = false; scroll.right.setByKeyboard = false;  break;
        case SDLK_UP: scroll.up.shouldScroll = false; scroll.up.setByKeyboard = false;  break;
        case SDLK_DOWN: scroll.down.shouldScroll = false; scroll.down.setByKeyboard = false;  break;
        default: break;
        }

        manager->ResetMomentum();
    }
}

void World::onEventReceive(Mouse &mouseEvent) {
    if(mouseEvent.GetType() == MOUSE_BUTTONDOWN) {
        int64_t px = (mouseEvent.GetPosX() + manager->CamX()) / 30;
        int64_t py = (mouseEvent.GetPosY() + manager->CamY()) / 16;

        if(mouseEvent.RmbDown()) {
            rmbHolding = true;
            rmbPressed = SDL_GetTicks();
            mouseX = mouseEvent.GetPosX();
            mouseY = mouseEvent.GetPosY();
        }

        if(mouseEvent.LmbDown()) {
            manager->GrabMouse();
        }
    }else if(mouseEvent.GetType() == MOUSE_BUTTONUP) {
        if(mouseEvent.RmbUp()) {
            rmbHolding = false;
            rmbHeld = 0;
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

        if(x > manager->GetWidth() - scrollThreshold) {
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

        if(y > manager->GetHeight() - scrollThreshold) {
            scroll.down.shouldScroll = true;
            scroll.down.setByMouse = true;
        }
        else if(scroll.down.setByMouse) {
            scroll.down.shouldScroll = false;
            scroll.down.setByMouse = false;
            shouldReset = true;
        }

        if(shouldReset) {
            manager->ResetMomentum();
        }
    }
}
