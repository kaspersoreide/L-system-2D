#version 430 core

layout (local_size_x = 1) in;

layout (binding = 0) coherent readonly buffer block1
{
    uint string[];
};

layout (binding = 1) coherent writeonly buffer block2
{
    vec4 vertices[];    //line segments
};

uniform layout (location = 0) uint stringLength;
uniform layout (location = 1) float segmentLength;
uniform layout (location = 2) float turnAngle;

mat4 rotate(float a) {
    return mat4(
        cos(a), -sin(a), 0.0, 0.0,
        sin(a), cos(a), 0.0, 0.0,
        0.0, 0.0, 1.0, 0.0,
        0.0, 0.0, 0.0, 1.0
    );
}

mat4 translate(vec3 v) {
    return mat4(
        vec4(1.0, 0.0, 0.0, 0.0),
        vec4(0.0, 1.0, 0.0, 0.0),
        vec4(0.0, 0.0, 1.0, 0.0),
        vec4(v, 1.0)
    );
}




void main() {
    mat4 transform = mat4(1.0);
    float currentLength = segmentLength;
    int top = -1;
    mat4 matrixStack[64];   
    for (uint i = 0; i < stringLength; i++) {
        switch (string[i]) {
            case 43:    //+ 
                transform = rotate(turnAngle) * transform;
                break;
            case 45:    //-
                transform = rotate(-turnAngle) * transform;
                break;
            case 91:    //[
                matrixStack[++top] = transform;
                currentLength *= 0.8;
                break;
            case 93:    //]
                transform = matrixStack[top--];
                currentLength /= 0.8;
                break; 
            default:
                vertices[2 * i] = transform[3];
                vec4 forward = transform * vec4(currentLength, 0.0, 0.0, 0.0);
                transform = translate(forward.xyz) * transform;
                vertices[2 * i + 1] = transform[3];
                break;
        };
        //vertices[2 * i] = vec4(0.001 * i, 0.0, 0.0, 1.0);
        //vertices[2 * i + 1] = vec4(0.001 * i + 0.0005, 0.0, 0.0, 1.0);
    }
}