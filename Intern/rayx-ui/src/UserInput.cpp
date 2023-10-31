#include "UserInput.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "Camera.h"

/**
 * This function handles various keyboard events including window manipulation and camera movement.
 * It integrates ImGui for GUI events and directly manipulates a CameraController object for 3D navigation.
 */
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    ImGui_ImplGlfw_KeyCallback(window, key, scancode, action, mods);
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    auto camController = reinterpret_cast<CameraController*>(glfwGetWindowUserPointer(window));

    // The movement speed factor
    float speed = (mods & GLFW_MOD_SHIFT) ? 20.0f : 0.5f;

    // Quit
    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    // Fullscreen
    else if (key == GLFW_KEY_F11 && action == GLFW_PRESS) {
        auto monitor = glfwGetPrimaryMonitor();
        auto mode = glfwGetVideoMode(monitor);
        glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
    }
    // Windowed
    else if (key == GLFW_KEY_F10 && action == GLFW_PRESS) {
        glfwSetWindowMonitor(window, nullptr, 100, 100, 1280, 720, 60);
    }
    // wasd movement (+ faster movement with shift)
    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        camController->moveForward(speed);
    } else if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        camController->moveForward(-speed);
    } else if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        camController->moveSideways(-speed);
    } else if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        camController->moveSideways(speed);
    } else if (key == GLFW_KEY_Q && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        camController->moveUp(-speed);
    } else if (key == GLFW_KEY_E && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        camController->moveUp(speed);
    }
}

/**
 * This function handles mouse button events, mainly focusing on the right mouse button for camera
 * control. It also checks if ImGui wants to capture the keyboard and mouse events.
 */
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    auto camController = reinterpret_cast<CameraController*>(glfwGetWindowUserPointer(window));

    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            camController->startMouseLook();
            double mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            camController->setLastMousePos((float)mouseX, (float)mouseY);
        } else if (action == GLFW_RELEASE) {
            camController->stopMouseLook();
        }
    }
}

/**
 * This function updates the direction of the camera when the mouse is moved. It also integrates
 * ImGui for cursor position callbacks.
 */
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos) {
    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    auto camController = reinterpret_cast<CameraController*>(glfwGetWindowUserPointer(window));

    if (camController->isMouseLooking()) {
        camController->updateDirectionViaMouse((float)xpos, (float)ypos);
    }
}