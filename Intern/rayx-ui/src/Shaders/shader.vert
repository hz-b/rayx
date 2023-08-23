#version 450

layout(location = 0) out vec3 fragColor;

void main() {
    // Hardcoded triangle vertices
    vec3[6] vertices = vec3[6](
        vec3(-0.5, -0.5, 0.0),
        vec3(0.5, -0.5, 0.0),
        vec3(0.0, 0.5, 0.0),
        vec3(-0.5, 0.5, 1.0),
        vec3(0.5, 0.5, 1.0),
        vec3(0.0, -0.5, 1.0)
    );
    
    // Assigning colors to the vertices for debugging
    vec3[6] colors = vec3[6](
        vec3(1.0, 0.0, 0.0),  // Red
        vec3(0.0, 1.0, 0.0),  // Green
        vec3(0.0, 0.0, 1.0),  // Blue
        vec3(1.0, 1.0, 0.0),  // Yellow
        vec3(0.0, 1.0, 1.0),  // Cyan
        vec3(1.0, 0.0, 1.0)   // Magenta
    );

    gl_Position = vec4(vertices[gl_VertexIndex], 1.0);
    fragColor = colors[gl_VertexIndex];
}
