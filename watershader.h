#pragma once
#include "framework.h"
#include "shader.h"
#include "renderstate.h"

class WaterShader : public Shader {

public:
	WaterShader() {
		create("watershader.vert", "watershader.frag", "fragmentColor");
	}

	void Bind(RenderState state) {
		Use();

		setUniform(state.time, "u_time");
		vec2 originXZ = vec2(floor(state.wEye.x / state.chunkSize), floor(state.wEye.z / state.chunkSize)) * state.chunkSize + vec2(state.chunkSize / 2.0f, state.chunkSize / 2.0f);
		setUniform(originXZ, "u_planeOriginXZ");
		setUniform(state.MVP, "MVP");
		setUniform(state.M, "M");
		setUniform(state.wEye, "wEye");
	}
};
