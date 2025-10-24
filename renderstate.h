#pragma once
#include "framework.h"

struct RenderState {
	float time;
	vec3 cameraPos;
	vec3 cameraDir;
	mat4 MVP, M, V, P;
	mat4 invP;
	vec3 chunkId;
	float chunkSize;

	// Shadow params
	mat4  lightVP;
	vec2  shadowTexel;
	float shadowBias;
	GLuint shadowTex;

	// SSR inputs
	GLuint sceneColorPrev;
	GLuint sceneDepthCopy;
};
