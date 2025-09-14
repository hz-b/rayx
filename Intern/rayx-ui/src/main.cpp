#include <clocale>
#include <cstdio>

#define SDL_MAIN_USE_CALLBACKS
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>

#include "Application.h"
#include "Debug/Debug.h"

extern "C" {
SDL_AppResult SDL_AppInit(void** appstate, int argc, char* argv[]) {
    RAYX_VERB << "SDL_AppInit: initializing application...";
    *appstate = new Application(1920, 1080, "RAYX-UI", argc, argv);
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void* appstate, SDL_Event* event) {
    RAYX_VERB << "SDL_AppEvent: event type " << event->type;
    if (event->type == SDL_EVENT_QUIT) {
        return SDL_APP_SUCCESS;  // Signal to exit
    }

    static_cast<Application*>(appstate)->handleEvent(event);
    ImGui_ImplSDL3_ProcessEvent(event);

    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void* appstate) {
    auto* app = static_cast<Application*>(appstate);
    app->run();
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void* appstate, SDL_AppResult result) {
    RAYX_VERB << "SDL_AppQuit: quitting application with code: " << result;
    delete static_cast<Application*>(appstate);
    SDL_Quit();  // Clean up SDL subsystems
}
}
