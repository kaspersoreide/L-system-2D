#include "turtle.h"

Turtle::Turtle(float initialStep, float _stepDecay, float angle) {
    state = {vec2(0.0f, 0.0f), 1.57079632679489661923f, initialStep};
    rotationAngle = angle;
    stepDecay = _stepDecay;
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
            case '[':
                pushState();
                state.step *= 1.0f - stepDecay;
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
                vec2 dir = {cosf(state.angle), sinf(state.angle)};
                vertices.push_back(state.pos);
                state.pos += state.step * dir;
                vertices.push_back(state.pos);
                break;
        }
        
    }
}
    