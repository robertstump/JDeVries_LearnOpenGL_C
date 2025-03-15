#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;

out vec3 ourColor;
out vec3 aPosition;

uniform float angle;

void main() {
    float rad = radians(angle);
    float cosA = cos(rad);
    float sinA = sin(rad);

    vec3 rotatedPos = vec3(
        aPos.x * cosA - aPos.y * sinA,
        aPos.x * sinA + aPos.y * cosA,
        aPos.z
    );

    gl_Position = vec4(rotatedPos, 1.0);
    ourColor = aColor;
    aPosition = aPos;
}
