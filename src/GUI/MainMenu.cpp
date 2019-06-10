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
	double w;
	double h;
	const wchar_t *text;
	bool hasGlare;
	uint8_t index;
	uint8_t glareIndex;
};
const static MenuButtonInfo lut_buttons[] = {
	{ 0.12, 0.67,0.17578125,0.234375, L"Exit Stronghold", false, 68, 0 },
	{ 0.423,0.774,0.1337890625,0.098958334, L"Credits", false, 77, 0 },
	{ 0.691,0.687,0.17578125,0.234375, L"Tutorial", false, 72, 0 },
	{ 0.183,0.276,0.17578125,0.234375, L"Combat-based games", true, 15, 1 },
	{ 0.336,0.276,0.17578125,0.234375, L"Economic-based games", true, 32, 18 },
	{ 0.492,0.276,0.17578125,0.234375, L"Map Editor", true, 49, 35 },
	{ 0.648,0.278,0.17578125,0.234375, L"Load a Saved Game", true, 66, 52 },
	{ 0.726,0.511,0.1,0.16, L"Options", false, 88, 0 },
	{ 0.183,0.276,0.17578125,0.234375, L"Play the Military Campaign", true, 15, 1 },
	{ 0.336,0.276,0.17578125,0.234375, L"Play a Siege", true, 32, 18 },
	{ 0.492,0.276,0.17578125,0.234375, L"Play an Invasion", true, 49, 35 },
	{ 0.648,0.276,0.17578125,0.234375, L"Play a Multiplayer Game", true, 66, 52 },
	{ 0.262,0.276,0.17578125,0.234375, L"Play the Economics Campaign", true, 15, 1 },
	{ 0.416,0.276,0.17578125,0.234375, L"Play an Economics Mission", true, 32, 18 },
	{ 0.571,0.276,0.17578125,0.234375, L"Free Build", true, 49, 35 },
	{ 0.183,0.276,0.17578125,0.234375, L"New Working Map", true, 15, 1 },
	{ 0.336,0.276,0.17578125,0.234375, L"New Stand-Alone Mission", true, 32, 18 },
	{ 0.492,0.276,0.17578125,0.234375, L"New 'Siege That' Mission", true, 49, 35 },
	{ 0.648,0.276,0.17578125,0.234375, L"New Multiplayer Mission", true, 66, 52 },
	{ 0.12,0.67,0.17578125,0.234375, L"Back to Main Menu", false, 68, 0 }
};

MainMenu::MainMenu()
{
	edition = GetEdition();

	if (edition == STRONGHOLD_HD) {
		tgx_border = GetTgx(GetDirectory() / "gfx/SH1_Back.tgx").lock();
		screen.AllocNewTarget(1024, 768);
	}

    aud_chantloop.LoadSong(GetDirectory() / "fx/music/chantloop1.raw", true);

	gm1_icons_additional = GetGm1(GetDirectory() / "gm/interface_buttons.gm1").lock();
	gm1_icons_main = GetGm1(GetDirectory() / "gm/icons_front_end.gm1").lock();
	gm1_icons_combat = GetGm1(GetDirectory() / "gm/icons_front_end_combat.gm1").lock();
	gm1_icons_economic = GetGm1(GetDirectory() / "gm/icons_front_end_economics.gm1").lock();
	gm1_icons_builder = GetGm1(GetDirectory() / "gm/icons_front_end_builder.gm1").lock();

	tgx_bg_main = GetTgx(GetDirectory() / "gfx/frontend_main.tgx").lock();
	tgx_bg_combat = GetTgx(GetDirectory() / "gfx/frontend_combat.tgx").lock();
	tgx_bg_economic = GetTgx(GetDirectory() / "gfx/frontend_economics.tgx").lock();
	tgx_bg_builder = GetTgx(GetDirectory() / "gfx/frontend_builder.tgx").lock();

	aud_greetings.LoadEffect(GetGreetingsSound(), false);
	aud_exit.LoadEffect(GetDirectory() / "fx/speech/General_Quitgame.wav", false);

	/**
	* Render the border 'zoomed in' so that the
	* menu can be placed in the middle without scaling.
	*/
	border_rect.x = (1920 - GetWidth()) / 2;
	border_rect.y = (1200 - GetHeight()) / 2;
	border_rect.w = GetWidth();
	border_rect.h = GetHeight();

	mx = (GetWidth() - 1024) / 2;
	my = (GetHeight() - 768) / 2;

	/* Allocate buttons */
	if(edition == STRONGHOLD_HD) SetTarget(&screen, mx, my, 1024, 768);
	ui_elems.resize(BUTTON_END);
	for(size_t i = 0; i < BUTTON_END; i++) {
		const MenuButtonInfo *inf = &lut_buttons[i];
		ui_elems[i].Translate(inf->x, inf->y);
		ui_elems[i].Scale(inf->w, inf->h);
	}
	ResetTarget();
}

MainMenu::~MainMenu() {

}

UIState MainMenu::EnterMenu() {
	if(edition == STRONGHOLD_HD) {
		SetTarget(&screen, mx, my, 1024, 768);		
	}

	ResetTarget();

	currentState = MAIN_MENU;

	aud_chantloop.Play();
	aud_greetings.Play();

	while (Running()) {
		ClearDisplay();

		if(edition == STRONGHOLD_HD) {
			Render(*tgx_border, &border_rect);
			SetTarget(&screen, mx, my, 1024, 768);
		}

		/* Render the current menu on top of the background */
		switch(currentState) {
			case MAIN_MENU: {
				RenderMain();
			} break;
			case COMBAT_MENU: {
				RenderCombat();
				RenderBackToMain();
			} break;
			case ECONOMICS_MENU: {
				RenderEconomic();
				RenderBackToMain();
			} break;
			case BUILDER_MENU: {
				RenderBuilder();
				RenderBackToMain();
			} break;
			default: break;
		}

		/* Check buttons */
		if (selected == MAIN_EXIT) {
			// TODO: exit prompt
			aud_chantloop.Stop();
			return EXIT_GAME;
		}
		else if (selected == MAIN_COMBAT) {
			currentState = COMBAT_MENU;
		}
		else if (selected == MAIN_ECONOMIC) {
			currentState = ECONOMICS_MENU;
		}
		else if (selected == MAIN_BUILDER) {
			currentState = BUILDER_MENU;
		}
		else if (selected == MAIN_LOAD) {
			currentState = LOAD_SAVED_MENU;
		}
		else if (selected == MAIN_TUTORIAL) {
			currentState = TUTORIAL;
		}
		else if (selected == MAIN_FIREFLY) {
			currentState = CREDITS;
		}
		else if(selected == BACK_TO_MAIN) {
			currentState = MAIN_MENU;
		}
		else if (selected == MAIN_SETTINGS) {
			currentState = SETTINGS_MENU;
		}

		RenderText(L"V" SOURCEHOLD_VERSION_STRING, 4, 4);

		ResetTarget();
		Render(screen, mx, my);

		/* Reset target first, then play credits and leave again */
		if(currentState == CREDITS) {
    		aud_chantloop.Stop();
    		/* Start credits loop */
    		cred.Play(false, true, true);
    		aud_chantloop.SetGain(1.0);
    		aud_chantloop.Resume();

    		currentState = MAIN_MENU;
		}

		aud_greetings.Update();

		glareCounter = (int)(GetTime() * 10.0);

		FlushDisplay();
		SyncDisplay();
	}

	aud_chantloop.Stop();
	return EXIT_GAME;
}

boost::filesystem::path MainMenu::GetGreetingsSound() {
	boost::filesystem::path snd = GetDirectory() / "fx/speech/";

	int index = GetUsernameIndex();
	if(index == -1) {
		snd /= "General_Startgame.wav";
	}else {
		snd /= "name" + std::to_string(index + 1) + ".wav";
	}

	return snd;
}

void MainMenu::RenderMain() {
	/* Render the buttons */
	selected = BUTTON_END;

	Render(*tgx_bg_main);

	auto icons_main = gm1_icons_main->GetTextureAtlas().lock();
	auto icons_additional = gm1_icons_additional->GetTextureAtlas().lock();
	for(int i = MAIN_EXIT; i < MAIN_SETTINGS+1; i++) {
		ui_elems[i].Render(
			[&]() -> Texture& {
				MenuButtonInfo inf = lut_buttons[i];

				if(ui_elems[i].IsClicked()) selected = (MenuButton)i;
				/* Special case */
				if(i == MAIN_FIREFLY) {
					if (ui_elems[i].IsMouseOver()) {
						icons_additional->SetRect(icons_additional->Get(inf.index + 1));
						RenderMenuText(inf.text);
					} else {
						icons_additional->SetRect(icons_additional->Get(inf.index));
					}
					return *icons_additional;
				}else {
					if (ui_elems[i].IsMouseOver()) {
						icons_main->SetRect(icons_main->Get(inf.index + 1));
						RenderMenuText(inf.text);
					} else {
						if(inf.hasGlare) {
							icons_main->SetRect(icons_main->Get(inf.glareIndex + (glareCounter % 14)));
						}else {
							icons_main->SetRect(icons_main->Get(inf.index));
						}
					}
					return *icons_main;
				}
		});
	}
}

void MainMenu::RenderCombat() {
	selected = BUTTON_END;

	Render(*tgx_bg_combat);

	auto icons_combat = gm1_icons_combat->GetTextureAtlas().lock();
	for(int i = COMBAT_CAMPAIGN; i < COMBAT_MULTIPLAYER+1; i++) {
		ui_elems[i].Render(
			[&]() -> Texture& {
				MenuButtonInfo inf = lut_buttons[i];

				if(ui_elems[i].IsClicked()) selected = (MenuButton)i;
				if (ui_elems[i].IsMouseOver()) {
					icons_combat->SetRect(icons_combat->Get(inf.index + 1));
					RenderMenuText(inf.text);
				} else {
					if(inf.hasGlare) {
						icons_combat->SetRect(icons_combat->Get(inf.glareIndex + (glareCounter % 14)));
					}else {
						icons_combat->SetRect(icons_combat->Get(inf.index));
					}
				}
			return *icons_combat;
		});
	}
}

void MainMenu::RenderEconomic() {
	selected = BUTTON_END;

	Render(*tgx_bg_economic);

	auto icons_economics = gm1_icons_economic->GetTextureAtlas().lock();
	for(int i = ECO_CAMPAIGN; i < ECO_FREEBUILD+1; i++) {
		ui_elems[i].Render(
			[&]() -> Texture& {
				MenuButtonInfo inf = lut_buttons[i];

				if(ui_elems[i].IsClicked()) selected = (MenuButton)i;
				if (ui_elems[i].IsMouseOver()) {
					icons_economics->SetRect(icons_economics->Get(inf.index + 1));
					RenderMenuText(inf.text);
				} else {
					if(inf.hasGlare) {
						icons_economics->SetRect(icons_economics->Get(inf.glareIndex + (glareCounter % 14)));
					}else {
						icons_economics->SetRect(icons_economics->Get(inf.index));
					}
				}
			return *icons_economics;
		});
	}
}

void MainMenu::RenderBuilder() {
	selected = BUTTON_END;

	Render(*tgx_bg_builder);

	auto icons_economics = gm1_icons_economic->GetTextureAtlas().lock();
	for(int i = BUILDER_WORKING_MAP; i < BUILDER_MULTIPLAYER+1; i++) {
		ui_elems[i].Render(
			[&]() -> Texture& {
				MenuButtonInfo inf = lut_buttons[i];

				if(ui_elems[i].IsClicked()) selected = (MenuButton)i;
				if (ui_elems[i].IsMouseOver()) {
					icons_economics->SetRect(icons_economics->Get(inf.index + 1));
					RenderMenuText(inf.text);
				} else {
					if(inf.hasGlare) {
						icons_economics->SetRect(icons_economics->Get(inf.glareIndex + (glareCounter % 14)));
					}else {
						icons_economics->SetRect(icons_economics->Get(inf.index));
					}
				}
			return *icons_economics;
		});
	}
}

void MainMenu::RenderBackToMain() {
	auto icons_combat = gm1_icons_combat->GetTextureAtlas().lock();
	ui_elems[BACK_TO_MAIN].Render(
		[&]() -> Texture& {
			MenuButtonInfo inf = lut_buttons[BACK_TO_MAIN];
			if(ui_elems[BACK_TO_MAIN].IsClicked()) selected = BACK_TO_MAIN;
			if (ui_elems[BACK_TO_MAIN].IsMouseOver()) {
				icons_combat->SetRect(icons_combat->Get(inf.index + 1));
				RenderMenuText(inf.text);
			} else {
					icons_combat->SetRect(icons_combat->Get(inf.index));
			}
		return *icons_combat;
	});
}
