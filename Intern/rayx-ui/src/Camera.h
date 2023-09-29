#pragma once

#include <glm/glm.hpp>

struct Camera {
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

    double m_yaw;
    double m_pitch;

    bool m_mouseLooking;
    double m_lastMouseX;
    double m_lastMouseY;

    struct Config {
        float FOV;
        float near;
        float far;
    } m_config;

    CameraController();

    void updateDirection(double deltaYaw, double deltaPitch);
    void moveForward(float distance);
    void moveSideways(float distance);
    void moveUp(float distance);
    void startMouseLook();
    void stopMouseLook();
    bool isMouseLooking() const;
    void setLastMousePos(double x, double y);
    void updateDirectionViaMouse(double mouseX, double mouseY);
    void update(Camera& cam, float aspectRatio);
};

std::string SerializeCameraController(const CameraController& cam);
void DeserializeCameraController(CameraController& cam, const std::string& data);
void SaveCameraControllerToFile(const CameraController& cam, const std::string& filepath);
void LoadCameraControllerFromFile(CameraController& cam, const std::string& filepath);