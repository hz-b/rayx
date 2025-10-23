#include "Camera.h"

#include <imgui_impl_sdl3.h>
#include <imgui_impl_vulkan.h>

#include <fstream>
#include <glm/gtc/matrix_transform.hpp>
#include <sstream>

CameraController::CameraController()
    : m_position(0.0f, 5.0f, -10.0f),
      m_direction(0.0f, 0.0f, 1.0f),
      m_cameraMode(CameraMode::Perspective),
      m_perspectiveCfg({60.0f, 0.1f, 10000.0f}),
      m_orthogonalCfg({100.0f, -1000.0f, 1000.0f}),
      m_up(0.0f, 1.0f, 0.0f),
      m_yaw(90.0f),
      m_pitch(0.0f),
      m_mouseLooking(false),
      m_lastMouseX(0.0),
      m_lastMouseY(0.0) {}

void CameraController::updateDirection(float deltaYaw, float deltaPitch) {
    m_yaw += deltaYaw;
    m_pitch += deltaPitch;

    // Clamp pitch to prevent flips
    if (m_pitch > 89.0f) m_pitch = 89.0f;
    if (m_pitch < -89.0f) m_pitch = -89.0f;

    glm::vec3 newDirection;
    newDirection.x = (float)(cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)));
    newDirection.y = (float)sin(glm::radians(m_pitch));
    newDirection.z = (float)(sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)));
    m_direction    = glm::normalize(newDirection);
}

void CameraController::moveForward(float distance) { m_position += m_direction * distance; }
void CameraController::moveSideways(float distance) { m_position += glm::normalize(glm::cross(m_direction, m_up)) * distance; }
void CameraController::moveUp(float distance) { m_position += m_up * distance; }
void CameraController::startMouseLook() { m_mouseLooking = true; }
void CameraController::stopMouseLook() { m_mouseLooking = false; }
bool CameraController::isMouseLooking() const { return m_mouseLooking; }

void CameraController::setLastMousePos(float x, float y) {
    m_lastMouseX = x;
    m_lastMouseY = y;
}

void CameraController::updateDirectionViaMouse(float mouseX, float mouseY) {
    float deltaX = mouseX - m_lastMouseX;
    float deltaY = mouseY - m_lastMouseY;
    setLastMousePos(mouseX, mouseY);
    updateDirection(0.05f * deltaX, -0.05f * deltaY);
}

void CameraController::PerspectiveConfig::display() {
    ImGui::SliderFloat("FOV", &m_FOV, 1.0f, 180.0f);
    ImGui::SliderFloat("Near", &m_near, 0.1f, 1000.0f);
    ImGui::SliderFloat("Far", &m_far, 0.1f, 10000.0f);
}

void CameraController::OrthogonalConfig::display() {
    ImGui::SliderFloat("Frustum Scale", &m_frustumScale, 0.1f, 1000.0f);
    ImGui::SliderFloat("Near", &m_near, -1000.0f, 1000.0f);
    ImGui::SliderFloat("Far", &m_far, -1000.0f, 1000.0f);
}

void CameraController::displaySettings() {
    ImGui::Text("Camera:");
    bool isOrtho = m_cameraMode == CameraMode::Orthogonal;
    if (ImGui::Checkbox("Orthographic Camera", &isOrtho)) { m_cameraMode = isOrtho ? CameraMode::Orthogonal : CameraMode::Perspective; }

    if (m_cameraMode == CameraMode::Perspective) {
        m_perspectiveCfg.display();
    } else if (m_cameraMode == CameraMode::Orthogonal) {
        m_orthogonalCfg.display();
    }

    ImGui::InputFloat3("Position", &m_position.x);
    ImGui::InputFloat3("Direction", &m_direction.x);

    if (ImGui::Button("Save Camera")) { SaveCameraControllerToFile(*this, "camera_save.txt"); }
    ImGui::SameLine();
    if (ImGui::Button("Load Camera")) { LoadCameraControllerFromFile(*this, "camera_save.txt"); }
}

void CameraController::update(Camera& cam, float aspectRatio) {
    cam.view = glm::lookAt(m_position, m_position + m_direction, m_up);

    if (m_cameraMode == CameraMode::Perspective) {
        cam.proj = glm::perspective(glm::radians(m_perspectiveCfg.m_FOV), aspectRatio, m_perspectiveCfg.m_near, m_perspectiveCfg.m_far);
        cam.n    = m_perspectiveCfg.m_near;
        cam.f    = m_perspectiveCfg.m_far;
    } else if (m_cameraMode == CameraMode::Orthogonal) {
        float top   = m_orthogonalCfg.m_frustumScale * 0.5f;
        float right = top * aspectRatio;

        float left   = -right;
        float bottom = -top;

        cam.proj = glm::ortho(left, right, bottom, top, m_orthogonalCfg.m_near, m_orthogonalCfg.m_far);
        cam.n    = m_orthogonalCfg.m_near;
        cam.f    = m_orthogonalCfg.m_far;
    }
    cam.proj[1][1] *= -1;  // Flip the y-axis
    cam.isOrtho = m_cameraMode == CameraMode::Orthogonal ? 1 : 0;
}

void CameraController::lookAtPoint(const glm::vec3& targetPoint, float distance) {
    // Set the camera's x-coordinate to match the target point
    float x = targetPoint.x;

    // Calculate the z-coordinate using Pythagoras, ensuring the camera stays at the specified distance
    float offset = std::sqrt(distance);
    float z      = targetPoint.z - offset;

    // Set the camera's y-coordinate with an offset
    float y = targetPoint.y + offset;

    m_position = glm::vec3(x, y, z);

    // Update the camera's direction to point towards the target
    m_direction = glm::normalize(targetPoint - m_position);

    // Update the pitch and yaw based on the new direction
    m_pitch = glm::degrees(std::asin(m_direction.y));
    m_yaw   = glm::degrees(std::atan2(m_direction.z, m_direction.x));

    updateDirection(0.0, 0.0);
}

void CameraController::setCameraMode(CameraMode mode) {
    m_cameraMode = mode;
    if (m_cameraMode == CameraMode::Perspective) {
        m_perspectiveCfg.m_near = 0.1f;
        m_perspectiveCfg.m_far  = 10000.0f;
    } else if (m_cameraMode == CameraMode::Orthogonal) {
        m_perspectiveCfg.m_near = -1000.0f;
        m_perspectiveCfg.m_far  = 1000.0f;
    }
}

// ---- SERDE ----
// Serialize the CameraController to a string
std::string SerializeCameraController(const CameraController& cam) {
    std::ostringstream ss;
    glm::vec3 pos = cam.getPosition();
    glm::vec3 dir = cam.getDirection();

    ss << pos.x << " " << pos.y << " " << pos.z << "\n";
    ss << dir.x << " " << dir.y << " " << dir.z << "\n";
    return ss.str();
}

// Deserialize the CameraController from a string
void DeserializeCameraController(CameraController& cam, const std::string& data) {
    std::istringstream ss(data);
    glm::vec3 pos;
    glm::vec3 dir;

    ss >> pos.x >> pos.y >> pos.z;
    ss >> dir.x >> dir.y >> dir.z;
    cam.setPosition(pos);
    cam.setDirection(dir);
}

// Save to file
void SaveCameraControllerToFile(const CameraController& cam, const std::string& filepath) {
    std::ofstream file(filepath);
    file << SerializeCameraController(cam);
    file.close();
}

// Load from file
void LoadCameraControllerFromFile(CameraController& cam, const std::string& filepath) {
    std::ifstream file(filepath);
    std::string data((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    DeserializeCameraController(cam, data);
    file.close();
}