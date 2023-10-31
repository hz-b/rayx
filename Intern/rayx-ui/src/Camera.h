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
    struct PerspectiveConfig {
        float m_FOV;
        float m_near;
        float m_far;

        void display();
    } m_perspectiveCfg;

    struct OrthogonalConfig {
        float m_frustumScale;
        float m_near;
        float m_far;

        void display();
    } m_orthogonalCfg;

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
    void lookAtPoint(const glm::vec3& targetPoint, float distance = 50.0f);

    void displaySettings();
    void update(Camera& cam, float aspectRatio);

    enum class CameraMode { Perspective, Orthogonal };
    void setCameraMode(CameraMode mode);

  private:
    CameraMode m_cameraMode = CameraMode::Perspective;

    glm::vec3 m_up;

    float m_yaw;
    float m_pitch;

    bool m_mouseLooking;
    float m_lastMouseX;
    float m_lastMouseY;
};

std::string SerializeCameraController(const CameraController& cam);
void DeserializeCameraController(CameraController& cam, const std::string& data);
void SaveCameraControllerToFile(const CameraController& cam, const std::string& filepath);
void LoadCameraControllerFromFile(CameraController& cam, const std::string& filepath);