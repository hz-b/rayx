#include "Camera.h"

CameraController::CameraController()
    : m_position(0.0f, 0.0f, -2.0f),
      m_direction(0.0f, 0.0f, 1.0f),
      m_up(0.0f, 1.0f, 0.0f),
      m_yaw(90.0f),
      m_pitch(0.0f),
      m_mouseLooking(false),
      m_lastMouseX(0.0),
      m_lastMouseY(0.0) {
    m_config.FOV = 60.0f;
    m_config.near = 0.1f;
    m_config.far = 10000.0f;
}

void CameraController::updateDirection(double deltaYaw, double deltaPitch) {
    m_yaw += deltaYaw;
    m_pitch += deltaPitch;

    // Clamp pitch to prevent flips
    if (m_pitch > 89.0f) m_pitch = 89.0f;
    if (m_pitch < -89.0f) m_pitch = -89.0f;

    glm::vec3 newDirection;
    newDirection.x = (float)(cos(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)));
    newDirection.y = (float)sin(glm::radians(m_pitch));
    newDirection.z = (float)(sin(glm::radians(m_yaw)) * cos(glm::radians(m_pitch)));
    m_direction = glm::normalize(newDirection);
}

void CameraController::moveForward(float distance) { m_position += m_direction * distance; }
void CameraController::moveSideways(float distance) { m_position += glm::normalize(glm::cross(m_direction, m_up)) * distance; }
void CameraController::moveUp(float distance) { m_position += m_up * distance; }
void CameraController::startMouseLook() { m_mouseLooking = true; }
void CameraController::stopMouseLook() { m_mouseLooking = false; }
bool CameraController::isMouseLooking() const { return m_mouseLooking; }

void CameraController::setLastMousePos(double x, double y) {
    m_lastMouseX = x;
    m_lastMouseY = y;
}

void CameraController::updateDirectionViaMouse(double mouseX, double mouseY) {
    double deltaX = mouseX - m_lastMouseX;
    double deltaY = mouseY - m_lastMouseY;
    m_lastMouseX = mouseX;
    m_lastMouseY = mouseY;

    updateDirection(0.1f * deltaX, -0.1f * deltaY);
}

void CameraController::update(Camera& cam, float aspectRatio) {
    cam.model = glm::mat4(1.0f);
    cam.view = glm::lookAt(m_position, m_position + m_direction, m_up);
    cam.proj = glm::perspective(glm::radians(m_config.FOV), aspectRatio, m_config.near, m_config.far);
    cam.proj[1][1] *= -1;  // Vulkan has inverted Y coordinates
    cam.n = m_config.near;
    cam.f = m_config.far;
}

// ---- SERDE ----
// Serialize the CameraController to a string
std::string SerializeCameraController(const CameraController& cam) {
    std::ostringstream ss;
    ss << cam.m_position.x << " " << cam.m_position.y << " " << cam.m_position.z << "\n";
    ss << cam.m_direction.x << " " << cam.m_direction.y << " " << cam.m_direction.z << "\n";
    return ss.str();
}

// Deserialize the CameraController from a string
void DeserializeCameraController(CameraController& cam, const std::string& data) {
    std::istringstream ss(data);
    ss >> cam.m_position.x >> cam.m_position.y >> cam.m_position.z;
    ss >> cam.m_direction.x >> cam.m_direction.y >> cam.m_direction.z;
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