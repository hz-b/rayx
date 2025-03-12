#pragma once

#include <SDL3/SDL.h>

// Forward declaration of the CameraController class.
class CameraController;

/**
 * @brief Context for user input.
 *
 * Encapsulates parameters that might be needed for input processing.
 */
struct UserInputContext {
    CameraController* camController;
    bool isSceneWindowHovered;
};

/**
 * @brief Handles keyboard events from SDL.
 *
 * Processes SDL_EVENT_KEY_DOWN and SDL_EVENT_KEY_UP events. The event is first forwarded to ImGui;
 * if ImGui is not capturing keyboard input, this function performs camera and window control actions.
 *
 * @param window  Pointer to the SDL_Window.
 * @param event   Pointer to the SDL_Event.
 * @param context User input context containing the camera controller and hover state.
 */
void keyEventHandler(const SDL_Event* event, const UserInputContext& context);

/**
 * @brief Handles mouse button events from SDL.
 *
 * Processes SDL_EVENT_MOUSE_BUTTON_DOWN and SDL_EVENT_MOUSE_BUTTON_UP events. The event is forwarded to ImGui;
 * if ImGui is not capturing mouse input, the function manages camera control actions (for the right mouse button).
 *
 * @param window  Pointer to the SDL_Window.
 * @param event   Pointer to the SDL_Event.
 * @param context User input context containing the camera controller and hover state.
 */
void mouseButtonEventHandler(const SDL_Event* event, const UserInputContext& context);

/**
 * @brief Handles mouse motion events from SDL.
 *
 * Processes SDL_EVENT_MOUSE_MOTION events. The event is forwarded to ImGui;
 * if ImGui is not capturing mouse input, the camera’s view direction is updated.
 *
 * @param window  Pointer to the SDL_Window.
 * @param event   Pointer to the SDL_Event.
 * @param context User input context containing the camera controller and hover state.
 */
void mouseMotionEventHandler(const SDL_Event* event, const UserInputContext& context);
