#pragma once

#include "GLFW/glfw3.h"

/**
 * @brief Callback for key presses and releases in a GLFW window.
 * @param window   Pointer to the GLFW window that received the event.
 * @param key      Key code of the key that was pressed or released.
 * @param scancode System-specific scancode of the key.
 * @param action   Action type (GLFW_PRESS, GLFW_RELEASE, GLFW_REPEAT).
 * @param mods     Bit field describing which modifier keys were held down.
 */
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);

/**
 * @brief Callback for mouse button presses and releases in a GLFW window.
 * @param window  Pointer to the GLFW window that received the event.
 * @param button  Mouse button that was pressed or released.
 * @param action  Action type (GLFW_PRESS or GLFW_RELEASE).
 * @param mods    Bit field describing which modifier keys were held down.
 */
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);

/**
 * @brief Callback for mouse cursor movement in a GLFW window.
 * @param window Pointer to the GLFW window that received the event.
 * @param xpos   X-coordinate of the cursor.
 * @param ypos   Y-coordinate of the cursor.
 */
void cursorPosCallback(GLFWwindow* window, double xpos, double ypos);