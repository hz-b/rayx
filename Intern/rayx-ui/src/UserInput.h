#pragma once

#include <GLFW/glfw3.h>

// Key callback
void keyCallback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseButtonCallback(GLFWwindow* window, int button, int action, int mods);
void cursorPosCallback(GLFWwindow* window, float xpos, float ypos);