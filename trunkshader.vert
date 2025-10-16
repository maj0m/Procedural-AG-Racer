#version 450 core
precision highp float;

layout(location=0) in vec3 vtxPos;
layout(location=1) in mat4 M;

uniform mat4 V, P;
uniform vec3 wEye;

out vec3 wView;
out float wDist;

void main(){
    gl_Position = P * V * M * vec4(vtxPos, 1.0);

    vec4 wPos = M * vec4(vtxPos, 1);
    wView  = wEye - wPos.xyz;
    wDist = length(wView);
}