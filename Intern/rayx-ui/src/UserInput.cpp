#include "UserInput.h"

#include <SDL3/SDL_keycode.h>
#include <imgui.h>
#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>

#include "Camera.h"

//------------------------------------------------------------------------------
// keyEventHandler
//------------------------------------------------------------------------------
void keyEventHandler(const SDL_Event* event, const UserInputContext& context) {
    // Process only keyboard events.
    if (event->type != SDL_EVENT_KEY_DOWN && event->type != SDL_EVENT_KEY_UP) return;

    // Forward the event to ImGui.
    ImGui_ImplSDL3_ProcessEvent(event);

    // If ImGui wants to capture keyboard input, do not process further.
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    // Get the camera controller from the context.
    CameraController* camController = context.camController;
    if (!camController) return;

    // Determine movement speed; holding shift yields faster movement.
    Uint32 mod  = event->key.mod;
    float speed = (mod & SDL_KMOD_SHIFT) ? 50.0f : 2.0f;

    if (event->type == SDL_EVENT_KEY_DOWN) {
        switch (event->key.key) {
            case SDLK_W:
                camController->moveForward(speed);
                break;
            case SDLK_S:
                camController->moveForward(-speed);
                break;
            case SDLK_A:
                camController->moveSideways(-speed);
                break;
            case SDLK_D:
                camController->moveSideways(speed);
                break;
            case SDLK_Q:
                camController->moveUp(-speed);
                break;
            case SDLK_E:
                camController->moveUp(speed);
                break;
            default:
                break;
        }
    }
}

//------------------------------------------------------------------------------
// mouseButtonEventHandler
//------------------------------------------------------------------------------
void mouseButtonEventHandler(const SDL_Event* event, const UserInputContext& context) {
    // Process only mouse button events.
    if (event->type != SDL_EVENT_MOUSE_BUTTON_DOWN && event->type != SDL_EVENT_MOUSE_BUTTON_UP) return;

    // Forward the event to ImGui.
    ImGui_ImplSDL3_ProcessEvent(event);

    // If ImGui is capturing mouse events, do not process further.
    if (ImGui::GetIO().WantCaptureMouse && !context.isSceneWindowHovered) return;

    CameraController* camController = context.camController;
    if (!camController) return;
    // printf("Mouse hovers scene: %i\n", context.isSceneWindowHovered);

    // Process only the right mouse button.
    if (event->button.button == SDL_BUTTON_RIGHT) {
        if (event->type == SDL_EVENT_MOUSE_BUTTON_DOWN) {
            // Only start mouse look if the scene window is hovered.
            if (!context.isSceneWindowHovered) return;
            camController->startMouseLook();

            float mouseX, mouseY;
            SDL_GetMouseState(&mouseX, &mouseY);
            camController->setLastMousePos(mouseX, mouseY);
        } else if (event->type == SDL_EVENT_MOUSE_BUTTON_UP) {
            camController->stopMouseLook();
        }
    }
}

//------------------------------------------------------------------------------
// mouseMotionEventHandler
//------------------------------------------------------------------------------
void mouseMotionEventHandler(const SDL_Event* event, const UserInputContext& context) {
    // Process only mouse motion events.
    if (event->type != SDL_EVENT_MOUSE_MOTION) return;

    // Forward the event to ImGui.
    ImGui_ImplSDL3_ProcessEvent(event);

    // If ImGui is capturing mouse events, do not process further.
    if (ImGui::GetIO().WantCaptureMouse && !context.isSceneWindowHovered) return;

    CameraController* camController = context.camController;
    if (!camController) return;

    // Update the camera's direction if mouse look is active.
    if (camController->isMouseLooking()) {
        camController->updateDirectionViaMouse(static_cast<float>(event->motion.x), static_cast<float>(event->motion.y));
    }
}
