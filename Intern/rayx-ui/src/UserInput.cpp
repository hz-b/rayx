#include "UserInput.h"

#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_vulkan.h>

#include "Camera.h"

// Key callback
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

void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods) {
    ImGui_ImplGlfw_MouseButtonCallback(window, button, action, mods);
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    auto camController = reinterpret_cast<CameraController*>(glfwGetWindowUserPointer(window));

    if (button == GLFW_MOUSE_BUTTON_RIGHT) {
        if (action == GLFW_PRESS) {
            camController->startMouseLook();
            float mouseX, mouseY;
            glfwGetCursorPos(window, &mouseX, &mouseY);
            camController->setLastMousePos(mouseX, mouseY);
        } else if (action == GLFW_RELEASE) {
            camController->stopMouseLook();
        }
    }
}

void cursorPosCallback(GLFWwindow* window, float xpos, float ypos) {
    ImGui_ImplGlfw_CursorPosCallback(window, xpos, ypos);
    if (ImGui::GetIO().WantCaptureKeyboard) return;

    auto camController = reinterpret_cast<CameraController*>(glfwGetWindowUserPointer(window));

    if (camController->isMouseLooking()) {
        camController->updateDirectionViaMouse(xpos, ypos);
    }
}