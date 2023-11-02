#version 450

layout(location = 0) in float near; //0.01
layout(location = 1) in float far; //100
layout(location = 2) flat in int isOrthographic;
layout(location = 3) in vec3 nearPoint;
layout(location = 4) in vec3 farPoint;
layout(location = 5) in mat4 fragView;
layout(location = 9) in mat4 fragProj;
layout(location = 0) out vec4 outColor;

vec4 grid(vec3 fragPos3D, float scale, bool drawAxis) {
    vec2 coord = fragPos3D.xz * scale;
    vec2 derivative = fwidth(coord);
    vec2 grid = abs(fract(coord - 0.5) - 0.5) / derivative;
    float line = min(grid.x, grid.y);
    float minimumz = min(derivative.y, 1);
    float minimumx = min(derivative.x, 1);
    vec4 color = vec4(0.3, 0.3, 0.3, 1.0 - min(line, 1.0));
    // z axis
    if(fragPos3D.x > -2.0 * minimumx && fragPos3D.x < 2.0 * minimumx)
        color.z = 1.0;
    // x axis
    if(fragPos3D.z > -2.0 * minimumz && fragPos3D.z < 2.0 * minimumz)
        color.x = 1.0;
    return color;
}

float computeDepth(vec3 pos) {
    vec4 clip_space_pos = fragProj * fragView * vec4(pos.xyz, 1.0);
    if (isOrthographic == 1) {
        // For orthographic projection, the depth is linear
        return clip_space_pos.z;
    } else {
        // For perspective projection, divide by w
        return (clip_space_pos.z / clip_space_pos.w);
    }
}

float computeLinearDepth(vec3 pos) {
    if (isOrthographic == 1) {
        // For orthographic projection, depth is already linear
        return (pos.z - near) / (far - near);
    } else {
        // For perspective projection, do the perspective divide and linearize
        vec4 clip_space_pos = fragProj * fragView * vec4(pos.xyz, 1.0);
        float clip_space_depth = (clip_space_pos.z / clip_space_pos.w) * 2.0 - 1.0; // put back between -1 and 1
        float linearDepth = (2.0 * near * far) / (far + near - clip_space_depth * (far - near)); // get linear value between 0.01 and 100
        return linearDepth / far; // normalize
    }
}

void main() {
    float t = -nearPoint.y / (farPoint.y - nearPoint.y);
    vec3 fragPos3D = nearPoint + t * (farPoint - nearPoint);

    gl_FragDepth = computeDepth(fragPos3D);

    float linearDepth = computeLinearDepth(fragPos3D);
    float fading = max(0, (0.5 - linearDepth));

    outColor = (grid(fragPos3D, 1, true) + grid(fragPos3D, 0.1, true))* float(t > 0); // adding multiple resolution for the grid
    outColor.a *= fading;
}