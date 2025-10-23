#pragma once
#include "framework.h"
#include "light.h"

struct RenderState {
	float time;
	vec3 wEye;
	vec3 wFront;
	vec3 wUp;
	mat4 MVP, M, V, P;
	vec3 chunkId;
	float chunkSize;

	// Shadow params
	mat4  lightVP;
	vec2  shadowTexel;  // (1/width, 1/height)
	float shadowBias;
	GLuint shadowTex;
};
