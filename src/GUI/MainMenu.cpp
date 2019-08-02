#include <map>
#include <boost/assign.hpp>

#include <GUI/MainMenu.h>
#include <Rendering/Font.h>
#include <System/Config.h>
#include <Assets.h>

using namespace Sourcehold::GUI;
using namespace Sourcehold::Rendering;

/* All of the menu buttons. TODO: replace double with int */
struct MenuButtonInfo {
    double x;
    double y;
    const wchar_t *text;
    bool hasGlare;
    uint8_t index;
    uint8_t glareIndex;
    uint8_t glareOrder;
    uint8_t atlasIndex;
};

const static MenuButtonInfo lut_buttons[] = {
    // MAIN_MENU //
    {  0.12, 0.67, L"Exit Stronghold", false, 68, 0, 0, 0 },
    { 0.423,0.774, L"Credits", false, 77, 0, 0, 1 },
    { 0.691,0.687, L"Tutorial", false, 72, 0, 0, 0 },
    { 0.183,0.276, L"Combat-based games", true, 15, 1, 0, 0 },
    { 0.336,0.276, L"Economic-based games", true, 32, 18, 1, 0 },
    { 0.492,0.276, L"Map Editor", true, 49, 35, 2, 0 },
    { 0.648,0.278, L"Load a Saved Game", true, 66, 52, 3, 0 },
    { 0.732, 0.54, L"Options", false, 88, 0, 0, 1 },
    // COMBAT_MENU //
    { 0.183,0.276, L"Play the Military Campaign", true, 15, 1, 0, 2 },
    { 0.336,0.276, L"Play a Siege", true, 32, 18, 1, 2 },
    { 0.492,0.276, L"Play an Invasion", true, 49, 35, 2, 2 },
    { 0.648,0.276, L"Play a Multiplayer Game", true, 66, 52, 3, 2 },
    {   0.1,  0.7, L"Back to Main Menu", false, 70, 0, 0, 0 },
    {   0.1,  0.7, L"", false, 68, 0, 0, 2 }, // back
    {  0.73,  0.7, L"", false, 70, 0, 0, 2 }, // next
    // ECONOMICS_MENU //
    { 0.262,0.276, L"Play the Economics Campaign", true, 15, 1, 0, 3 },
    { 0.416,0.276, L"Play an Economics Mission", true, 32, 18, 1, 3 },
    { 0.571,0.276, L"Free Build", true, 49, 35, 2, 3 },
    {   0.1,  0.7, L"Back to Main Menu", false, 70, 0, 0, 0 },
    {   0.1,  0.7, L"", false, 51, 0, 0, 3 }, // back
    {  0.73,  0.7, L"", false, 53, 0, 0, 3 }, // next
    // BUILDER_MENU //
    { 0.183,0.276, L"New Working Map", true, 15, 1, 0, 4 },
    { 0.336,0.276, L"New Stand-Alone Mission", true, 32, 18, 1, 4 },
    { 0.492,0.276, L"New 'Siege That' Mission", true, 49, 35, 2, 4 },
    { 0.648,0.276, L"New Multiplayer Mission", true, 66, 52, 3, 4 },
    {   0.1,  0.7, L"Back to Main Menu", false, 70, 0, 0, 4 }
};

/* Connects a button to a UIState */
const static std::map<MenuButton, UIState> actions = boost::assign::map_list_of
    // MAIN_MENU //
    (MAIN_EXIT, EXIT_GAME)
    (MAIN_FIREFLY, CREDITS)
    (MAIN_TUTORIAL, TUTORIAL)
    (MAIN_COMBAT, COMBAT_MENU)
    (MAIN_ECONOMIC, ECONOMICS_MENU)
    (MAIN_BUILDER, BUILDER_MENU)
    (MAIN_LOAD, LOAD_SAVED_MENU)
    (MAIN_SETTINGS, SETTINGS_MENU)
    // COMBAT_MENU //
    (COMBAT_CAMPAIGN, MILITARY_CAMPAIGN_MENU)
    (COMBAT_SIEGE, SIEGE_MENU)
    (COMBAT_INVASION, INVASION_MENU)
    (COMBAT_MULTIPLAYER, MULTIPLAYER_MENU)
    (COMBAT_BACK_TO_MAIN, MAIN_MENU)
    (COMBAT_CAMPAIGN_BACK, COMBAT_MENU)
    (COMBAT_CAMPAIGN_NEXT, MILITARY_CAMPAIGN_MISSION)
    // ECONOMICS_MENU //
    (ECO_CAMPAIGN, ECONOMICS_CAMPAIGN_MENU)
    (ECO_MISSION, ECONOMICS_MISSION_MENU)
    (ECO_FREEBUILD, FREE_BUILD_MENU)
    (ECO_BACK_TO_MAIN, MAIN_MENU)
    (ECO_CAMPAIGN_BACK, ECONOMICS_MENU)
    (ECO_CAMPAIGN_NEXT, ECONOMICS_CAMPAIGN_MENU) // todo
    // BUILDER_MENU //
    (BUILDER_WORKING_MAP, WORKING_MAP_MENU)
    (BUILDER_STANDALONE, STAND_ALONE_MISSION_MENU)
    (BUILDER_SIEGE, SIEGE_THAT_MENU)
    (BUILDER_MULTIPLAYER, MULTIPLAYER_MAP_MENU)
    (BUILDER_BACK_TO_MAIN, MAIN_MENU);

MainMenu::MainMenu()
{
    edition = GetEdition();

    mx = (GetWidth() - 1024) / 2;
    my = (GetHeight() - 768) / 2;

    if (edition == STRONGHOLD_HD) {
        screen.AllocNewTarget(1024, 768);
        SetTarget(&screen, Rect<int>{ mx, my, 1024, 768 });
    }

    bool success = true;
    success &= aud_chantloop.Load(GetDirectory() / "fx/music/chantloop1.raw", true);
    success &= aud_greetings.Load(GetGreetingsSound(), false);
    success &= aud_exit.Load(GetDirectory() / "fx/speech/General_Quitgame.wav", false);
    if(!success) {
        Logger::error(Subsystem::GUI) << "Unable to initialize main menu: failed to load SFX!" << std::endl;
        return;
    }

    gm1_icons_main = GetGm1(GetDirectory() / "gm/icons_front_end.gm1");
    gm1_icons_additional = GetGm1(GetDirectory() / "gm/interface_buttons.gm1");
    gm1_icons_combat = GetGm1(GetDirectory() / "gm/icons_front_end_combat.gm1");
    gm1_icons_economic = GetGm1(GetDirectory() / "gm/icons_front_end_economics.gm1");
    gm1_icons_builder = GetGm1(GetDirectory() / "gm/icons_front_end_builder.gm1");

    tgx_bg_main = GetTgx(GetDirectory() / "gfx/frontend_main.tgx");
    tgx_bg_main2 = GetTgx(GetDirectory() / "gfx/frontend_main2.tgx");
    tgx_bg_combat = GetTgx(GetDirectory() / "gfx/frontend_combat.tgx");
    tgx_bg_combat2 = GetTgx(GetDirectory() / "gfx/frontend_combat2.tgx");
    tgx_bg_economic = GetTgx(GetDirectory() / "gfx/frontend_economics.tgx");
    tgx_bg_economic2 = GetTgx(GetDirectory() / "gfx/frontend_economics2.tgx");
    tgx_bg_builder = GetTgx(GetDirectory() / "gfx/frontend_builder.tgx");

    /* Get textures */
    ui_tex.resize(5);
    ui_tex[0] = gm1_icons_main->GetTextureAtlas();
    ui_tex[1] = gm1_icons_additional->GetTextureAtlas();
    ui_tex[2] = gm1_icons_combat->GetTextureAtlas();
    ui_tex[3] = gm1_icons_economic->GetTextureAtlas();
    ui_tex[4] = gm1_icons_builder->GetTextureAtlas();

    /* Allocate buttons */
    ui_elems.resize(BUTTON_END);
    for(size_t i = 0; i < BUTTON_END; i++) {
        const MenuButtonInfo *inf = &lut_buttons[i];
        auto atlas = ui_tex[inf->atlasIndex];
        auto rect = atlas->Get(inf->index);

        ui_elems[i].Translate(inf->x, inf->y);
        ui_elems[i].Scale(rect.w, rect.w);
        ui_elems[i].SetTexture(atlas.get());
    }
    ResetTarget();
}

MainMenu::~MainMenu()
{
}

UIState MainMenu::EnterMenu()
{
    currentState = MAIN_MENU;

    aud_chantloop.Play();
    //aud_greetings.Play();

    while (Running()) {
        ClearDisplay();

        if(edition == STRONGHOLD_HD) {
            RenderMenuBorder();
            SetTarget(&screen, Rect<int>{ mx, my, 1024, 768 });
        }

        /* Render the current menu on top of the background */
        HideAll();

        /**
         * Handle the current UIState.
         * buttonEnd and buttonStart define the range of
         * buttons to be rendered for the menu page.
         */
        selected = BUTTON_END;
        int buttonEnd = BUTTON_END, buttonStart = MAIN_EXIT;
        switch(currentState) {
        default:
        case MAIN_MENU: {
            Render(*tgx_bg_main);
            buttonEnd = COMBAT_CAMPAIGN;
        } break;
        case COMBAT_MENU: {
            Render(*tgx_bg_combat);
            buttonEnd = COMBAT_BACK_TO_MAIN+1;
            buttonStart = COMBAT_CAMPAIGN;
        } break;
        case MILITARY_CAMPAIGN_MENU: {
            Render(*tgx_bg_combat2);

            DialogResult res = CombatMenuDialog();
            if(res == LOAD) {
                // Double-click on mission -> start mission
                return MILITARY_CAMPAIGN_MISSION;
            }

            buttonEnd = COMBAT_CAMPAIGN_NEXT+1;
            buttonStart = COMBAT_CAMPAIGN_BACK;
        } break;
        case MILITARY_CAMPAIGN_MISSION: {
            // TODO
            if(GetMilitaryCampaignIndex() >= 0) return MILITARY_CAMPAIGN_MISSION;
            else currentState = MILITARY_CAMPAIGN_MENU;
        }
        case SIEGE_MENU: {
            Render(*tgx_bg_combat2);

            DialogResult res = SiegeMenuDialog();
            buttonStart = buttonEnd = MAIN_EXIT;
        } break;
        case ECONOMICS_MENU: {
            Render(*tgx_bg_economic);
            buttonEnd = ECO_BACK_TO_MAIN+1;
            buttonStart = ECO_CAMPAIGN;
        } break;
        case ECONOMICS_CAMPAIGN_MENU: {
            Render(*tgx_bg_economic2);

            DialogResult res = EconomicsMenuDialog();
            if(res == LOAD) {
                return ECONOMICS_CAMPAIGN_MISSION;
            }

            buttonEnd = ECO_CAMPAIGN_NEXT+1;
            buttonStart = ECO_CAMPAIGN_BACK;
        } break;
        case BUILDER_MENU: {
            Render(*tgx_bg_builder);
            buttonEnd = BUILDER_BACK_TO_MAIN+1;
            buttonStart = BUILDER_WORKING_MAP;
        } break;
        case LOAD_SAVED_MENU: {
            Render(*tgx_bg_main2);
            std::string file;
            DialogResult res = LoadDialog(file);
            if(res == LOAD && !file.empty()) {
                // TODO
            } else if(res == BACK) {
                currentState = MAIN_MENU;
            }
            buttonStart = buttonEnd = MAIN_EXIT;
        } break;
        case EXIT_GAME: {
            Render(*tgx_bg_main2);
            DialogResult res = QuitDialog();
            if(res == QUIT) {
                aud_chantloop.Stop();
                return EXIT_GAME;
            } else if(res == BACK) {
                currentState = MAIN_MENU;
            }
            buttonStart = buttonEnd = MAIN_EXIT;
        } break;
        case SETTINGS_MENU: {
            Render(*tgx_bg_main2);
            DialogResult res = SettingsDialog();
            if(res == BACK) {
                currentState = MAIN_MENU;
            }
            buttonStart = buttonEnd = MAIN_EXIT;
        } break;
        }

        int glareTicks = (int)(GetTime() * 10.0);
        glareCounter = (glareTicks / 14) % 4;

        for(int i = buttonStart; i < buttonEnd; i++) {
            ui_elems[i].Show();
            ui_elems[i].Render(
            [&]() -> SDL_Rect {
                MenuButtonInfo inf = lut_buttons[i];
                auto tex = ui_tex[inf.atlasIndex];

                if(ui_elems[i].IsClicked()) selected = (MenuButton)i;
                if (ui_elems[i].IsMouseOver()) {
                    RenderMenuText(inf.text);
                    return tex->Get(inf.index + 1);
                } else {
                    if(inf.hasGlare && inf.glareOrder == glareCounter) {
                        return tex->Get(inf.glareIndex + (glareTicks % 14));
                    }
                    else {
                        return tex->Get(inf.index);
                    }
                }
            });
        }

        if(selected != BUTTON_END) {
            try {
                currentState = actions.at(selected);
            } catch(std::out_of_range & ex) {
                currentState = MAIN_MENU;
            }
        }

        RenderText(L"V." SOURCEHOLD_VERSION_STRING, 6, 4, FONT_SMALL);
        ResetTarget();

        if(edition == STRONGHOLD_HD) {
            Render(screen, mx, my);
        }

        //aud_greetings.Update();

        /* Reset target first, then play credits and leave again */
        if(currentState == CREDITS) {
            aud_chantloop.Stop();
            /* Start credits loop */
            cred.Play(false, true, true);
            aud_chantloop.SetGain(1.0);
            aud_chantloop.Resume();

            currentState = MAIN_MENU;
        }

        FlushDisplay();
        SyncDisplay();
    }

    aud_chantloop.Stop();
    return EXIT_GAME;
}

boost::filesystem::path MainMenu::GetGreetingsSound()
{
    boost::filesystem::path snd = GetDirectory() / "fx/speech/";

    int index = GetUsernameIndex();
    if(index == -1) {
        snd /= "General_Startgame.wav";
    }
    else {
        snd /= "name" + std::to_string(index + 1) + ".wav";
    }

    return snd;
}

void MainMenu::HideAll()
{
    for(auto & e : ui_elems) {
        e.Hide();
    }
}


