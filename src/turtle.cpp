#include "turtle.h"

Turtle::Turtle(float step, float angle) {
    state = {vec2(0.0f, 0.0f), 0.0f, step};
    rotationAngle = angle;
}

void Turtle::pushState() {
    stateStack.push(state);
}
void Turtle::popState() {
    state = stateStack.top();
    stateStack.pop();
}

void Turtle::build(string buildString) {
    for (int i = 0; i < buildString.length(); i++) {
        
        switch (buildString[i]) {
            case 'F':
                vertices.push_back(state.pos);
                state.pos += state.step * vec2(
                    cosf(state.angle),
                    sinf(state.angle)
                );
                vertices.push_back(state.pos);
                break;
            case '[':
                pushState();
                state.step *= 0.8f;
                break;
            case ']':
                popState();
                break;
            case '+':
                state.angle += rotationAngle;
                break;
            case '-':
                state.angle -= rotationAngle;
                break;
            default:
                break;
        }
        
    }
}
    