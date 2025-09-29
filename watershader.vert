#version 450 core
precision highp float;

struct Light {
	vec3 La, Le;
	vec3 dir;
};

layout(location = 0) in vec3 vertexPos;
layout(location = 1) in vec2 vertexUV;

uniform float u_time;
uniform mat4 MVP, M;					// MVP, Model
uniform vec3 wEye;						// Eye position

out float wDist;
out vec3 wView;
out vec3 vtxPos;

float waveOffset(vec3 vertex) {
    float waveLength = 5.0;
    float waveAmplitude = 0.5;
    float x = (vertex.x / waveLength + u_time / 10.0) * 6.28318530718;
    float z = (vertex.z / waveLength + u_time / 10.0) * 6.28318530718;
    return (sin(x) + cos(z)) * waveAmplitude;
}

void main() {
	vtxPos = vertexPos;
	vtxPos.y += waveOffset(vtxPos);
	gl_Position = vec4(vtxPos, 1.0) * MVP;

	vec4 wPos = vec4(vtxPos, 1) * M;
	wView  = wEye - wPos.xyz;
	wDist = length(wView);         // Distance from eye to vertex
}