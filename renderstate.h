#pragma once
#include "framework.h"

struct RenderState {
	float time;
	vec3 cameraPos;
	vec3 cameraDir;
	float nearPlane, farPlane;
	mat4 MVP, M, V, P;
	mat4 invP;
	vec3 chunkId;
	float chunkSize;

	// Shadow params
	mat4  lightVP;
	vec2  shadowTexel;
	float shadowBias;
	GLuint shadowTex;

	// Textures
	GLuint sceneColor = 0;
	GLuint sceneDepth = 0;
	GLuint sceneColorPrev = 0;
	GLuint sceneDepthCopy = 0;

	// Depth of Field
	float focusDist = 50.0f;
	float focusRange = 500.0f;

	// Bloom
	float bloomThreshold = 0.75f;
	float bloomSoftKnee = 0.6f;
	float bloomIntensity = 0.5f;

	// Color grading
	float saturation = 1.05f;
	float vibrance = 0.10f;
};
