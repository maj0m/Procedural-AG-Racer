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
		setUniform(state.V, "V");
		setUniform(state.P, "P");
		setUniform(state.invP, "invP");
		setUniform(state.wEye, "wEye");

		// Shadow
		setUniform(state.lightVP, "lightVP");
		setUniform(state.shadowTexel, "u_shadowTexel");
		setUniform(state.shadowBias, "u_shadowBias");
		setUniform(2, "u_shadowMap");

		// SSR
		setUniform(3, "u_sceneColor");  // prev frame
		setUniform(4, "u_sceneDepth");  // copied depth
	}
};
