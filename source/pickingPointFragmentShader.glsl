#version 330 core

flat in vec3 pickColor;
out vec4 finalColor;

void main() {
    finalColor = vec4(pickColor, 1.0);
}

