#version 450 core
precision highp float;

layout(location = 0) in vec3 vtxPos_OS;

uniform vec3 u_camPos_WS;
uniform mat4 u_M, u_MVP;

out vec3 viewDir_WS;

void main() {
	gl_Position = u_MVP * vec4(vtxPos_OS, 1.0);
	vec4 vtxPos_WS = u_M * vec4(vtxPos_OS, 1);
	viewDir_WS  = u_camPos_WS - vtxPos_WS.xyz;
}