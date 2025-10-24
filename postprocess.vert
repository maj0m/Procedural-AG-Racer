#version 450 core

layout(location=0) in vec2 vtxPos;
layout(location=1) in vec2 vtxUV;

out vec2 uv;

void main() {
    uv = vtxUV;
    gl_Position = vec4(vtxPos, 0.0, 1.0);
}
