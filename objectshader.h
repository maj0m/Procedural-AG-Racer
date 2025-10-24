#pragma once
#include "framework.h"
#include "shader.h"
#include "renderstate.h"

class ObjectShader : public Shader {

public:
	ObjectShader() {
		create("objectshader.vert", "objectshader.frag", "fragmentColor");
	}

	void Bind(RenderState state) {
		Use();

		setUniform(state.wEye, "u_camPos_WS");
		setUniform(state.M, "u_M");
		setUniform(state.MVP, "u_MVP");

		// Shadow
		setUniform(state.lightVP, "u_lightVP");
		setUniform(state.shadowTexel, "u_shadowTexel");
		setUniform(state.shadowBias, "u_shadowBias");
		setUniform(2, "u_shadowMap");
	}
};
