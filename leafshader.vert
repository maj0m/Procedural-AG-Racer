#version 450 core
precision highp float;

layout(location=0) in vec3 vtxPos_OS;
layout(location=1) in mat4 instM;

uniform vec3 u_camPos_WS;
uniform mat4 u_V, u_P;
uniform mat4 u_lightVP;

out float viewDist_WS;
out vec3 viewDir_WS;
out vec4 lightPos_CS;

void main() {
    vec4 vtxPos_WS = instM * vec4(vtxPos_OS, 1.0);
    gl_Position = u_P * u_V * vtxPos_WS;

    viewDir_WS  = u_camPos_WS - vtxPos_WS.xyz;
    viewDist_WS = length(viewDir_WS);
    lightPos_CS = u_lightVP * vtxPos_WS;
}