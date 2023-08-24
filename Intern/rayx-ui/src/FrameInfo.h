#pragma once

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Camera {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(4) float n;
    alignas(4) float f;
};

class CameraController {
  public:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 up;

    float yaw;
    float pitch;

    bool mouseLooking = false;
    double lastMouseX = 0.0;
    double lastMouseY = 0.0;

    struct Config {
        float FOV = 90.0f;
        float near = 0.1f;
        float far = 100.0f;
    } config;

    CameraController() : position(0.0f, 0.0f, -2.0f), direction(0.0f, 0.0f, 1.0f), up(0.0f, 1.0f, 0.0f), yaw(90.0f), pitch(0.0f) {}

    void updateDirection(float deltaYaw, float deltaPitch) {
        yaw += deltaYaw;
        pitch += deltaPitch;

        // Clamp pitch to prevent flips
        if (pitch > 89.0f) pitch = 89.0f;
        if (pitch < -89.0f) pitch = -89.0f;

        glm::vec3 newDirection;
        newDirection.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        newDirection.y = sin(glm::radians(pitch));
        newDirection.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        direction = glm::normalize(newDirection);
    }

    void moveForward(float distance) { position += direction * distance; }
    void moveSideways(float distance) { position += glm::normalize(glm::cross(direction, up)) * distance; }
    void moveUp(float distance) { position += up * distance; }
    void startMouseLook() { mouseLooking = true; }
    void stopMouseLook() { mouseLooking = false; }
    bool isMouseLooking() const { return mouseLooking; }
    void setLastMousePos(double x, double y) {
        lastMouseX = x;
        lastMouseY = y;
    }

    void updateDirectionViaMouse(double mouseX, double mouseY) {
        double deltaX = mouseX - lastMouseX;
        double deltaY = mouseY - lastMouseY;
        lastMouseX = mouseX;
        lastMouseY = mouseY;

        updateDirection(0.1f * deltaX, 0.1f * deltaY);
    }

    void update(Camera& cam, float aspectRatio) {
        cam.model = glm::mat4(1.0f);
        cam.view = glm::lookAt(position, position + direction, up);
        cam.proj = glm::perspective(glm::radians(config.FOV), aspectRatio, config.near, config.far);
        cam.n = config.near;
        cam.f = config.far;
    }
};

struct FrameInfo {
    uint32_t frameIndex;
    float frameTime;
    VkCommandBuffer commandBuffer;
    Camera& camera;
    VkDescriptorSet descriptorSet;
};