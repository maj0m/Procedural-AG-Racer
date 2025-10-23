#pragma once
#include "framework.h"
#include "light.h"

struct RenderState {
	float time;
	vec3 wEye;
	vec3 wFront;
	vec3 wUp;
	mat4 MVP, M, V, P;
	mat4 invP;
	vec3 chunkId;
	float chunkSize;

	// Shadow params
	mat4  lightVP;
	vec2  shadowTexel;	// (1/width, 1/height)
	float shadowBias;
	GLuint shadowTex;

	// SSR inputs
	GLuint sceneColorPrev;  // last frame color (reflection source)
	GLuint sceneDepthCopy;  // current frame depth copy (for ray hits)
};
