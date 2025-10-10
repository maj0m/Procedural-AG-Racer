#version 450 core
precision highp float;

layout(location = 0) in vec3 vtxPos;   // Originally in modeling space, now directly using NDC

uniform mat4 MVP, M;					// MVP, Model
uniform vec3 wEye;						// Eye position

out vec3 wView;

void main() {
	gl_Position = MVP * vec4(vtxPos, 1.0);

	vec4 wPos = M * vec4(vtxPos, 1);

	wView  = wEye - wPos.xyz;
}