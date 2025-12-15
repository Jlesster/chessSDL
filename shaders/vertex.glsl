#version 450

// Input attributes (from your C++ vertex struct)
layout(location = 0) in vec3 inPosition;  // x, y, z
layout(location = 1) in vec4 inColor;     // r, g, b, a

// Output to fragment shader
layout(location = 0) out vec4 fragColor;

void main() {
    // Convert 3D position to 4D clip space
    // SDL3 expects vec4 with w=1.0
    gl_Position = vec4(inPosition, 1.0);

    // Pass color to fragment shader
    fragColor = inColor;
}
