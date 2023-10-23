#version 430

layout(location = 0) in vec4 pos;

void main() {
    gl_Position = vec4(0.01 * pos.xy, 0.0, 1.0);
}