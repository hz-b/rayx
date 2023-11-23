#pragma once

#include <glm/glm.hpp>

struct Camera {
    alignas(16) glm::mat4 view;
    alignas(16) glm::mat4 proj;
    alignas(4) float n;
    alignas(4) float f;
    alignas(4) uint32_t isOrtho;
};

class CameraController {
  public:
    enum class CameraMode { Perspective, Orthogonal };

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
    void lookAtPoint(const glm::vec3& targetPoint, float distance = 200.0f);

    void displaySettings();
    void update(Camera& cam, float aspectRatio);

    void setCameraMode(CameraMode mode);
    glm::vec3 getPosition() const { return m_position; }
    glm::vec3 getDirection() const { return m_direction; }

  private:
    glm::vec3 m_position;
    glm::vec3 m_direction;

    CameraMode m_cameraMode = CameraMode::Perspective;
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