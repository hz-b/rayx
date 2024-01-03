#version 450

// Push constants
layout(push_constant) uniform PushConstants {
    mat4 model;
} push;

layout(location = 0) in vec4 fragColor;


layout(location = 0) out vec4 outColor;

void main() {
    outColor = fragColor;
}