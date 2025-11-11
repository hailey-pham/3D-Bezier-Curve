#version 330 core

layout(location = 0) in vec3 position;

uniform mat4 MVP;

flat out vec3 pickColor;

void main() {
    gl_Position = MVP * vec4(position, 1.0);
    pickColor = vec3((float(gl_VertexID + 1)) / 255.0, 0.0, 0.0);
}

