#version 450 core
precision highp float;

layout(location = 0) in vec3 vtxPos;
layout(location = 1) in vec2 vtxUV;

uniform mat4 MVP, M;
uniform vec3 wEye;

out vec3 wView;

void main() {
	gl_Position = vec4(vtxPos, 1.0) * MVP;

	vec4 wPos = vec4(vtxPos, 1) * M;

	wView  = wEye - wPos.xyz;
}