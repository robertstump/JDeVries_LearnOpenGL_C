#version 330 core
out vec4 FragColor;
in vec3 ourColor;
in vec3 aPosition;

void main() {
    FragColor = vec4(ourColor.x + aPosition.x,
        ourColor.y + aPosition.y, 
        ourColor.z + aPosition.z,
        1.0);
}
