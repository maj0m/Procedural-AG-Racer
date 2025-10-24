#version 450 core
precision highp float;

layout(location = 0) in vec3 vtxPos_OS;

uniform vec3 u_camPos_WS;
uniform mat4 u_M, u_MVP;
uniform mat4 u_lightVP;

out float viewDist_WS;
out vec3 viewDir_WS;
out vec4 lightPos_CS;

void main() {
	gl_Position = u_MVP * vec4(vtxPos_OS, 1.0);

	vec4 wPos = u_M * vec4(vtxPos_OS, 1);
	viewDir_WS  = u_camPos_WS - wPos.xyz;
	viewDist_WS = length(viewDir_WS);
	lightPos_CS = u_lightVP * wPos;
}