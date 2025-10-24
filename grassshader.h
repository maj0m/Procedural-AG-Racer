#pragma once
#include "framework.h"
#include "shader.h"
#include "renderstate.h"

class GrassShader : public Shader {

public:
	GrassShader() {
		create("grassshader.vert", "grassshader.frag", "fragmentColor");
	}

	void Bind(RenderState state) {
		Use();

		setUniform(state.time, "u_time");
		setUniform(state.cameraPos, "u_camPos_WS");
		setUniform(state.V, "u_V");
		setUniform(state.P, "u_P");

		// Shadow
		setUniform(state.lightVP, "u_lightVP");
		setUniform(state.shadowTexel, "u_shadowTexel");
		setUniform(state.shadowBias, "u_shadowBias");
		setUniform(2, "u_shadowMap");
	}
};
