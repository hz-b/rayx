#version 450

layout(binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
    float near;
    float far;
} cam;

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec4 inColor;

layout(location = 0) out vec4 fragColor;

void main() {
    gl_Position = cam.proj * cam.view * inPosition;
    fragColor = inColor;
}