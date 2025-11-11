#version 330 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;
layout(location = 2) in vec3 subdivisionPosition;
layout(location = 3) in vec3 bezierPosition;


uniform mat4 MVP;
uniform bool drawingLine;
uniform bool drawingSubdivision;
uniform bool drawingBezier;
uniform float yOffset;
uniform vec3 currentPoint;
uniform bool animatingPoint;

out vec3 fragColor;


void main() {
    if (drawingSubdivision) {
        gl_Position = MVP * vec4(vec3(subdivisionPosition.x, subdivisionPosition.y+yOffset, subdivisionPosition.z), 1.0);
    } else if (drawingBezier) {
        gl_Position = MVP * vec4(vec3(bezierPosition.x, bezierPosition.y+yOffset, bezierPosition.z), 1.0);
    } else if (animatingPoint) {
        gl_Position = MVP * vec4(vec3(currentPoint.x, currentPoint.y+yOffset, currentPoint.z), 1.0);
    } else {
        gl_Position = MVP * vec4(vec3(position.x, position.y+yOffset,position.z), 1.0);
    }

    if(drawingLine) {
        fragColor = vec3(1.0,1.0,1.0);
    } else if (drawingSubdivision) {
        fragColor = vec3(0.0,1.0,1.0);
    } else if (drawingBezier) {
        fragColor = vec3(1.0,1.0,0.0);
    } else if (animatingPoint) {
        fragColor = vec3(1.0,1.0,0.0);
    } else {
        fragColor = color;
    }
    
}

