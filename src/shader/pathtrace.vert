#version 410 core

layout(location = 0) in vec2 aPos;
layout(location = 1) in vec2 aTex;

out vec2 ScreenCoords;

void main() {
    ScreenCoords = (vec2(aPos.x, aPos.y) + 1.0) / 2.0;
    gl_Position = vec4(aPos, 0.0f, 1.0f);
}