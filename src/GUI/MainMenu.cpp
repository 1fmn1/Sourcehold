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
    {  0.12, 0.67, L"Exit Stronghold", false, 68, 0, 0, 0 },
    { 0.423,0.774, L"Credits", false, 77, 0, 0, 1 },
    { 0.691,0.687, L"Tutorial", false, 72, 0, 0, 0 },
    { 0.183,0.276, L"Combat-based games", true, 15, 1, 0, 0 },
    { 0.336,0.276, L"Economic-based games", true, 32, 18, 1, 0 },
    { 0.492,0.276, L"Map Editor", true, 49, 35, 2, 0 },
    { 0.648,0.278, L"Load a Saved Game", true, 66, 52, 3, 0 },
    { 0.726,0.511, L"Options", false, 88, 0, 0, 0 },
    { 0.183,0.276, L"Play the Military Campaign", true, 15, 1, 0, 2 },
    { 0.336,0.276, L"Play a Siege", true, 32, 18, 1, 2 },
    { 0.492,0.276, L"Play an Invasion", true, 49, 35, 2, 2 },
    { 0.648,0.276, L"Play a Multiplayer Game", true, 66, 52, 3, 2 },
    { 0.262,0.276, L"Play the Economics Campaign", true, 15, 1, 0, 3 },
    { 0.416,0.276, L"Play an Economics Mission", true, 32, 18, 1, 3 },
    { 0.571,0.276, L"Free Build", true, 49, 35, 2, 3 },
    { 0.183,0.276, L"New Working Map", true, 15, 1, 0, 4 },
    { 0.336,0.276, L"New Stand-Alone Mission", true, 32, 18, 1, 4 },
    { 0.492,0.276, L"New 'Siege That' Mission", true, 49, 35, 2, 4 },
    { 0.648,0.276, L"New Multiplayer Mission", true, 66, 52, 3, 4 },
    {   0.1, 0.7, L"Back to Main Menu", false, 70, 0, 0, 0 },
    {  0.73, 0.7, L"", false, 70, 0, 0, 2 }
};

MainMenu::MainMenu()
{
    edition = GetEdition();

    mx = (GetWidth() - 1024) / 2;
    my = (GetHeight() - 768) / 2;

    if (edition == STRONGHOLD_HD) {
        screen.AllocNewTarget(1024, 768);
        SetTarget(&screen, Rect<int>{ mx, my, 1024, 768 });
    }

    aud_chantloop.Load(GetDirectory() / "fx/music/chantloop1.raw", true);
    //aud_greetings.LoadEffect(GetGreetingsSound(), false);
    //aud_exit.LoadEffect(GetDirectory() / "fx/speech/General_Quitgame.wav", false);

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
    aud_greetings.Play();

    while (Running()) {
        ClearDisplay();

        if(edition == STRONGHOLD_HD) {
            RenderMenuBorder();
            SetTarget(&screen, Rect<int>{ mx, my, 1024, 768 });
        }

        /* Render the current menu on top of the background */
        HideAll();

        selected = BUTTON_END;
        int buttonEnd = BUTTON_END, buttonStart = MAIN_EXIT;
        switch(currentState) {
        default:
        case MAIN_MENU: {
            Render(*tgx_bg_main);
            buttonEnd = COMBAT_CAMPAIGN;
        }
        break;
        case COMBAT_MENU: {
            Render(*tgx_bg_combat);
            RenderBackToMain();
            buttonEnd = ECO_CAMPAIGN;
            buttonStart = COMBAT_CAMPAIGN;
        }
        break;
        case ECONOMICS_MENU: {
            Render(*tgx_bg_economic);
            RenderBackToMain();
            buttonEnd = BUILDER_WORKING_MAP;
            buttonStart = ECO_CAMPAIGN;
        }
        break;
        case ECONOMICS_CAMPAIGN_MENU: {
            Render(*tgx_bg_economic2);
            RenderBackToMain();
            RenderNext();

            DialogResult res = EconomicsMenuDialog();
            buttonStart = buttonEnd = MAIN_EXIT;
        }
        break;
        case BUILDER_MENU: {
            Render(*tgx_bg_builder);
            RenderBackToMain();
            buttonStart = BUILDER_WORKING_MAP;
            buttonEnd = BACK_TO_MAIN;
        }
        break;
        case MILITARY_CAMPAIGN_MENU: {
            Render(*tgx_bg_combat2);
            RenderBackToMain();
            RenderNext();

            DialogResult res = CombatMenuDialog();
            buttonStart = buttonEnd = MAIN_EXIT;
        }
        break;
        case SIEGE_MENU: {
            Render(*tgx_bg_combat2);
            RenderBackToMain();
            RenderNext();

            DialogResult res = SiegeMenuDialog();
            buttonStart = buttonEnd = MAIN_EXIT;
        }
        break;
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
        }
        break;
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
        }
        break;
        }

        int glareTicks = (int)(GetTime() * 10.0) % 14;
        if(glareTicks == 13) ++glareCounter %= 4;

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
                        return tex->Get(inf.glareIndex + glareTicks);
                    }
                    else {
                        return tex->Get(inf.index);
                    }
                }
            });
        }

        /* Check buttons, TODO: replace with table */
        switch(selected) {
        /* Main menu */
        case MAIN_EXIT: {
            currentState = EXIT_GAME;
        }
        break;
        case MAIN_COMBAT: {
            currentState = COMBAT_MENU;
        }
        break;
        case MAIN_ECONOMIC: {
            currentState = ECONOMICS_MENU;
        }
        break;
        case MAIN_BUILDER: {
            currentState = BUILDER_MENU;
        }
        break;
        case MAIN_LOAD: {
            currentState = LOAD_SAVED_MENU;
        }
        break;
        case MAIN_TUTORIAL: {
            currentState = TUTORIAL;
        }
        break;
        case MAIN_FIREFLY: {
            currentState = CREDITS;
        }
        break;
        case BACK_TO_MAIN: {
            currentState = MAIN_MENU;
        }
        break;
        case MAIN_SETTINGS: {
            currentState = SETTINGS_MENU;
        }
        break;
        /* Combat menu */
        case COMBAT_CAMPAIGN: {
            currentState = MILITARY_CAMPAIGN_MENU;
        }
        break;
        case COMBAT_SIEGE: {
            currentState = SIEGE_MENU;
        }
        break;
        case COMBAT_INVASION: {
        }
        break;
        /* Economics menu */
        case ECO_CAMPAIGN: {
            currentState = ECONOMICS_CAMPAIGN_MENU;
        }
        break;
        case ECO_MISSION: {
        }
        break;
        case ECO_FREEBUILD: {
        }
        break;
        /* Builder menu */
        case BUILDER_WORKING_MAP: {
        }
        break;
        case BUILDER_STANDALONE: {
        }
        break;
        case BUILDER_SIEGE: {
        }
        break;
        case BUILDER_MULTIPLAYER: {
        }
        break;
        case COMBAT_MULTIPLAYER: /* unsupported (yet) */
        default:
            break;
        }

        RenderText(L"V." SOURCEHOLD_VERSION_STRING, 6, 4, FONT_SMALL);
        ResetTarget();

        if(edition == STRONGHOLD_HD) {
            Render(screen, mx, my);
        }

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

void MainMenu::RenderBackToMain()
{
    ui_elems[BACK_TO_MAIN].Show();
    ui_elems[BACK_TO_MAIN].Render(
    [&]() -> SDL_Rect {
        MenuButtonInfo inf = lut_buttons[BACK_TO_MAIN];
        auto tex = ui_tex[inf.atlasIndex];

        if(ui_elems[BACK_TO_MAIN].IsClicked()) selected = BACK_TO_MAIN;
        if (ui_elems[BACK_TO_MAIN].IsMouseOver())
        {
            RenderMenuText(inf.text);
            return tex->Get(inf.index + 1);
        }
        else
        {
            return tex->Get(inf.index);
        }
    });
}

void MainMenu::RenderNext()
{
    ui_elems[NEXT].Show();
    ui_elems[NEXT].Render(
    [&]() -> SDL_Rect {
        MenuButtonInfo inf = lut_buttons[NEXT];
        auto tex = ui_tex[inf.atlasIndex];

        if(ui_elems[NEXT].IsClicked()) selected = NEXT;
        if (ui_elems[NEXT].IsMouseOver())
        {
            RenderMenuText(inf.text);
            return tex->Get(inf.index + 1);
        }
        else
        {
            return tex->Get(inf.index);
        }
    });
}

void MainMenu::HideAll()
{
    for(auto & e : ui_elems) {
        e.Hide();
    }
}

