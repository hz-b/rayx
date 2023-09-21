#pragma once

#include <glm/glm.hpp>

struct Camera {
    alignas(16) glm::mat4 model;
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(4) float n;
    alignas(4) float f;
};

class CameraController {
  public:
    glm::vec3 m_position;
    glm::vec3 m_direction;
    glm::vec3 m_up;

    float m_yaw;
    float m_pitch;

    bool m_mouseLooking;
    float m_lastMouseX;
    float m_lastMouseY;

    struct Config {
        float FOV;
        float near;
        float far;
    } m_config;

    CameraController();

    void updateDirection(float deltaYaw, float deltaPitch);
    void moveForward(float distance);
    void moveSideways(float distance);
    void moveUp(float distance);
    void startMouseLook();
    void stopMouseLook();
    bool isMouseLooking() const;
    void setLastMousePos(float x, float y);
    void updateDirectionViaMouse(float mouseX, float mouseY);
    void update(Camera& cam, float aspectRatio);
};

std::string SerializeCameraController(const CameraController& cam);
void DeserializeCameraController(CameraController& cam, const std::string& data);
void SaveCameraControllerToFile(const CameraController& cam, const std::string& filepath);
void LoadCameraControllerFromFile(CameraController& cam, const std::string& filepath);