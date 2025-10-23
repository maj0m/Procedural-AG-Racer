#pragma once
#include "framework.h"
#include "shader.h"
#include "renderstate.h"

class TrunkShader : public Shader {

public:
	TrunkShader() {
		create("trunkshader.vert", "trunkshader.frag", "fragmentColor");
	}

	void Bind(RenderState state) {
		Use();

		setUniform(state.V, "V");
		setUniform(state.P, "P");
		setUniform(state.wEye, "wEye");

		// Shadow
		setUniform(state.lightVP, "lightVP");
		setUniform(state.shadowTexel, "u_shadowTexel");
		setUniform(state.shadowBias, "u_shadowBias");
		setUniform(2, "u_shadowMap");
	}
};
