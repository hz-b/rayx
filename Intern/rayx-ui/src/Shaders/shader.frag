#version 450

layout(location = 0) out vec4 outColor;

void main() {
    if (gl_FragCoord.x < 400.0) {
        outColor = vec4(1.0, 0.0, 0.0, 1.0);  // Red for fragments on the left side
    } else {
        outColor = vec4(0.0, 0.0, 1.0, 1.0);  // Blue for fragments on the right side
    }
}
