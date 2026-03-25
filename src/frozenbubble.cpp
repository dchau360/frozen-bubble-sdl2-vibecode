/*
 * Frozen-Bubble SDL2 C++ Port
 * Copyright (c) 2000-2012 The Frozen-Bubble Team
 * Copyright (c) 2026 Huy Chau
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License
 * version 2, as published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "frozenbubble.h"
#include "logger.h"
#include "platform.h"
#include <sys/stat.h>

FrozenBubble *FrozenBubble::ptrInstance = NULL;

// Helper function to verify asset directory exists
bool VerifyAssetDirectory(const char* dataDir) {
    struct stat st;
    if (stat(dataDir, &st) != 0 || !S_ISDIR(st.st_mode)) {
        SDL_LogError(SDL_LOG_CATEGORY_APPLICATION,
            "Asset directory not found: %s\n"
            "Please ensure the game is installed correctly and DATA_DIR is set properly.",
            dataDir);
        return false;
    }
    return true;
}

const char *formatTime(int time){
    int h = int(time/3600.0);
    int m = int((time-h*3600)/60.0);
    int s = int((time-h*3600)-(m*60));

    static char fm[128];
    size_t offset = 0;
    fm[0] = '\0';
    if (h > 0) {
        offset += snprintf(fm + offset, 128 - offset, "%dh ", h);
    }
    if (m > 0) {
        if (h > 0) {
            offset += snprintf(fm + offset, 128 - offset, "%02dm ", m);
        } else {
            offset += snprintf(fm + offset, 128 - offset, "%dm ", m);
        }
    }
    if (s > 0) {
        if (m > 0) {
            snprintf(fm + offset, 128 - offset, "%02ds", s);
        } else {
            snprintf(fm + offset, 128 - offset, "%ds", s);
        }
    }
    return fm;
}

FrozenBubble *FrozenBubble::Instance()
{
    if(ptrInstance == NULL)
        ptrInstance = new FrozenBubble();
    return ptrInstance;
}

FrozenBubble::FrozenBubble() {
    // Initialize logger first so all subsequent logs are captured
    // Determine log file name based on existing logs (creator, joiner1-4)
    // Supports up to 5 players total
    const char* logFilename = nullptr;
    struct stat st;

    if (stat("frozen-bubble-creator.log", &st) != 0) {
        // creator.log doesn't exist - this is the creator
        logFilename = "frozen-bubble-creator.log";
    } else if (stat("frozen-bubble-joiner1.log", &st) != 0) {
        // creator.log exists but joiner1.log doesn't - this is joiner1
        logFilename = "frozen-bubble-joiner1.log";
    } else if (stat("frozen-bubble-joiner2.log", &st) != 0) {
        // joiner1.log exists but joiner2.log doesn't - this is joiner2
        logFilename = "frozen-bubble-joiner2.log";
    } else if (stat("frozen-bubble-joiner3.log", &st) != 0) {
        // joiner2.log exists but joiner3.log doesn't - this is joiner3
        logFilename = "frozen-bubble-joiner3.log";
    } else {
        // All other logs exist - this is joiner4 (5th player)
        logFilename = "frozen-bubble-joiner4.log";
    }

    Logger::Initialize(logFilename);

    // Verify asset directory exists before proceeding
#ifndef __ANDROID__
    if (!VerifyAssetDirectory(g_dataDir.c_str())) {
        std::string msg = "Could not find game assets at: " + g_dataDir + "\n\nPlease ensure the game is installed correctly.";
        SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR,
            "Asset Directory Missing",
            msg.c_str(),
            NULL);
        IsGameQuit = true;
        return;
    }
#endif

    gameOptions = GameSettings::Instance();
    gameOptions->ReadSettings();

    SDL_Point resolution = gameOptions->curResolution();
    Uint32 fullscreen = gameOptions->fullscreenMode() ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0;

    window = SDL_CreateWindow("Frozen-Bubble: SDL2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, resolution.x, resolution.y, fullscreen);
    if(gameOptions->linearScaling) SDL_SetHint(SDL_HINT_RENDER_SCALE_QUALITY, "best");

    if(!window) {
        IsGameQuit = true;
        std::cout << "Failed to create window: " << SDL_GetError() << std::endl;
    }

    SDL_Surface *icon = SDL_LoadBMP(ASSET("/gfx/pinguins/window_icon_penguin.bmp").c_str());
    if (icon) {
        SDL_SetWindowIcon(window, icon);
        SDL_FreeSurface(icon);
    } else {
        SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION, "Failed to load window icon: %s", SDL_GetError());
    }

    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    SDL_RenderSetLogicalSize(renderer, 640, 480);

    if(!renderer) {
        IsGameQuit = true;
        std::cout << "Failed to create renderer: " << SDL_GetError() << std::endl;
    }

    if( TTF_Init() == -1 )
    {
        IsGameQuit = true;
        std::cout << "Failed to initialise SDL_ttf: " << SDL_GetError() << std::endl;
    }

    audMixer = AudioMixer::Instance();
    hiscoreManager = HighscoreManager::Instance(renderer);

    init_effects((char*)g_dataDir.c_str());
    mainMenu = new MainMenu(renderer);
    mainGame = new BubbleGame(renderer);

}

FrozenBubble::~FrozenBubble() {
    if(renderer) {
        SDL_DestroyRenderer(renderer);
        renderer = nullptr;
    }

    if(window) {
        SDL_DestroyWindow(window);
        window = nullptr;
    }

    hiscoreManager->Dispose();
    audMixer->Dispose();
    gameOptions->Dispose();

    TTF_Quit();
    IMG_Quit();
    Mix_Quit();
    SDL_Quit();

    // Shutdown logger last to capture all cleanup logs
    Logger::Shutdown();
}

uint8_t FrozenBubble::RunForEver()
{
    // on init, try playing one of these songs depending on the current state:
    if(currentState == TitleScreen) audMixer->PlayMusic("intro");
    //else if (currentState == MainGame) mainGame->NewGame({false, 1, false});

    float framerate = 60;
    float frametime = 1/framerate * 1000;

    unsigned int ticks, lasttick = 0;
    float elapsed = 0;

    while(!IsGameQuit) {
        lasttick = ticks;
        ticks = SDL_GetTicks();
        elapsed = ticks - lasttick;

        // handle input
        SDL_Event e;
        while (SDL_PollEvent (&e)) {
            HandleInput(&e);
        }

        // render
        if(!IsGamePause) {
            SDL_RenderClear(renderer);
            if (currentState == TitleScreen) mainMenu->Render();
            else if (currentState == MainGame) mainGame->Render();
            else if (currentState == Highscores) {
                if (hiscoreManager->lastState == 1) mainGame->Render();
                hiscoreManager->RenderScoreScreen();
            }
            SDL_RenderPresent(renderer);
        }
        else {
            if (currentState == MainGame){
                mainGame->RenderPaused();
                SDL_RenderPresent(renderer);
            }
        }
        if(elapsed < frametime) {
            SDL_Delay(frametime - elapsed);
        }
    }
    if (startTime != 0) addictedTime += SDL_GetTicks() - startTime;
    if(addictedTime != 0) printf("Addicted for %s, %d bubbles were launched.", formatTime(addictedTime/1000), totalBubbles);
    this->~FrozenBubble();
    return 0;
}

void FrozenBubble::HandleInput(SDL_Event *e) {
    switch(e->type) {
        case SDL_WINDOWEVENT:
            switch (e->window.event) {
                case SDL_WINDOWEVENT_CLOSE:
                {
                    IsGameQuit = true;
                    break;
                }
            }
            break;
        case SDL_QUIT:
            IsGameQuit = true;
            break;
        case SDL_KEYDOWN:
            if(e->key.repeat) break;
            switch(e->key.keysym.sym) {
                case SDLK_F12:
                    gameOptions->SetValue("GFX:Fullscreen", "");
                    SDL_SetWindowFullscreen(window, gameOptions->fullscreenMode() ? SDL_WINDOW_FULLSCREEN_DESKTOP : 0);
                    break;
                case SDLK_PAUSE:
                    CallGamePause();
                    if (currentState == MainGame) {
                        if (!mainGame->playedPause) mainGame->playedPause = false;
                    }
                    break;
            }
            break;
    }

    if (IsGamePause) return;
    if(currentState == Highscores) {
        hiscoreManager->HandleInput(e);
        return;
    }
    if(currentState == TitleScreen) mainMenu->HandleInput(e);
    if(currentState == MainGame) mainGame->HandleInput(e);
}
