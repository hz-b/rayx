#pragma once

#include <vulkan/vulkan.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct CameraSettings {
    float FOV = 90.0f;
    glm::vec3 position = glm::vec3(0.0f, 0.0f, -2.0f);
    glm::vec3 target = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 up = glm::vec3(0.0f, 1.0f, 0.0f);  // Add Up vector
    float near = 0.1f;
    float far = 100.0f;

    glm::mat4 getViewMatrix() const { return glm::lookAt(position, target, up); }

    glm::mat4 getProjectionMatrix(float aspectRatio) const { return glm::perspective(glm::radians(FOV), aspectRatio, near, far); }
};

struct Camera {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(4) float n;
    alignas(4) float f;
};

struct FrameInfo {
    uint32_t frameIndex;
    float frameTime;
    VkCommandBuffer commandBuffer;
    Camera& camera;
    CameraSettings& cameraSettings;  // TODO(Jannis): remove this
    VkDescriptorSet descriptorSet;
};