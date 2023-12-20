#version 450

// Push constants
layout(push_constant) uniform PushConstants {
    mat4 model;
    bool isTextured;
} push;

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(set = 1, binding = 0) uniform sampler2D texSampler;

layout(location = 0) out vec4 outColor;

void main() {
    if (push.isTextured) {
        outColor = texture(texSampler, fragTexCoord);
    } else {
        outColor = fragColor;
    }
}