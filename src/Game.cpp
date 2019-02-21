#include <Game.h>

using namespace Sourcehold::Game;
using namespace Sourcehold::Audio;
using namespace Sourcehold::System;
using namespace Sourcehold::Parsers;
using namespace Sourcehold::Rendering;

Game::Game() : GameManager(), menu(*this) {

}

Game::~Game() {
    
}

bool Game::Init(CmdLineOptions &opt) {
    //MlbFile mlb;
    //mlb.LoadFromDisk(boost::filesystem::path("data/stronghold.mlb"));

    /*Tileset set;
    set.SetContext(ctx);
    if(!set.LoadFromDisk("data/gm/tile_castle.gm1")) {
        Logger::error("GAME") << "Tileset loading error!" << std::endl;
    }*/

    //auto entries = set.GetEntries();
    //tex1 = entries[0].image;
    //tex2 = entries[1].image;
    //tex3 = entries[2].image;
    //tex4 = entries[3].image;

    /*tgx.SetContext(ctx);
    if(!tgx.LoadFromDisk("data/gfx/defeat_pig.tgx")) {
        Logger::error("GAME") << "Tgx file loading error!" << std::endl;
        return false;
    }*/

    //VolumeTxt txt;
    //txt.LoadFromDisk("data/fx/volume.txt");

    /*bik.Init();
    if(!bik.LoadFromDisk("data/binks/intro.bik")) {
        Logger::error("GAME") << "Bink video error!" << std::endl;
        return false;
    }
    bik.InitFramebuffer(vidFrame);
    vidSound.Create(NULL, 0, false);*/

    return true;
}

int Game::Start() {
    /*if(!menu.Start()) {
        // exited out of intro or menu
        return EXIT_SUCCESS;
    }*/

    Playlist list({
        "data/fx/music/the maidenB.raw",
        "data/fx/music/underanoldtree.raw",
        "data/fx/music/appytimes.raw",
    }, false);
    list.Play();

    /*AudioSource audio;
    audio.LoadSong("data/fx/music/appytimes.raw", true);
    audio.Play();*/

//    Building building(ctx);
//    building.LoadFromDisk("");

//    Gm1File gm1(ctx);
//    gm1.LoadFromDisk("data/gm/body_siege_tower.gm1");
//    auto entries = gm1.GetEntries();
//    tex1 = entries[0].image;

    while(Display::IsOpen()) {
        Display::Clear();
        Display::HandleEvents();
        Display::StartTimer();

        list.Update();

        //Display::RenderTextureFullscreen(tgx);
        //Display::RenderTextureScale(tex1, 0.0, 0.0, 0.6, 0.3);
        //Display::RenderTextureScale(tex2, 0, 0, 30*8, 16*8);
        //Display::RenderTextureScale(tex2, 30*8, 0, 30*8, 16*8);
        //Display::RenderTextureScale(tex3, 0, 16*8, 30*8, 16*8);
        //Display::RenderTextureScale(tex4, 30*8, 16*8, 30*8, 16*8);

        //bik.Decode(vidFrame, vidSound);
        //Display::RenderTextureScale(vidFrame, 0.0f, 0.15f, 1.0f, 0.7f);

        Display::Flush();
        Display::EndTimer();
    }

    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    Game game;
    CmdLineOptions opt = ParseCmdLine(argc, argv);

    if(!opt.valid) return EXIT_FAILURE;
    if(opt.info) return EXIT_SUCCESS;
    
    if(!game.Init(opt)) {
        Logger::error("GAME") << "Game initialization failed due to previous errors!" << std::endl;
        return EXIT_FAILURE;
    }

    return game.Start();
}

#if SOURCEHOLD_WINDOWS == 1
#include <windows.h>
int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevIns, LPSTR lpszArgument, int iShow) {
    return main(NULL, 0);
}
#endif
