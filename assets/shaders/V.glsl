#version 330 core

layout(location = 0) in vec3 aPos;
layout(location = 1) in vec3 aColor[3];

layout(std140) uniform CameraBlock {
    mat4 uView;
    mat4 uProj[2];
} block[2];

layout(std140) uniform something {
    mat4 uView;
    mat4 uProj;
} some;

uniform mat4 model;
uniform mat4 view;

uniform mat4 uProj;
uniform mat4 uView;

out vec4 outColor;

void main() {
    vec4 worldPos = model * vec4(aPos, 1.0);

    //gl_Position = block[1].uProj[1] * block[1].uView * worldPos;
    gl_Position = uProj * uView * worldPos;

    outColor = vec4(aColor[1], 1.f);

    some;
}