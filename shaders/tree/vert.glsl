#version 430

layout(location = 0) in vec2 pos;

void main() {
    gl_Position = vec4(pos.x - 1.0, pos.y, 0.0, 1.0);
}