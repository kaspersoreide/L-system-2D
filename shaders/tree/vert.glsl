#version 430

layout(location = 0) in vec4 pos;

void main() {
    gl_Position = pos;
}