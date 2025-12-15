#version 450

// Input from vertex shader (interpolated)
layout(location = 0) in vec4 fragColor;

// Output color for this pixel
layout(location = 0) out vec4 outColor;

void main() {
    // Simply output the interpolated color
    outColor = fragColor;
}
