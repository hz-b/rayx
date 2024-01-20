#version 450

// Push constants
layout(push_constant) uniform PushConstants {
    mat4 model;
} push;

layout(set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
    float near;
    float far;
    bool isOrtho;
} cam;

layout(location = 0) in vec4 inPosition;
layout(location = 2) in vec2 inTexCoord;

layout(location = 1) out vec2 fragTexCoord;

void main() {
    gl_Position = cam.proj * cam.view * push.model * inPosition;
    fragTexCoord = inTexCoord;
}