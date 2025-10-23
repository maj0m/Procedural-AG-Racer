#pragma once
#include "framework.h"
#include "shader.h"
#include "renderstate.h"

class LeafShader : public Shader {

public:
	LeafShader() {
		create("leafshader.vert", "leafshader.frag", "fragmentColor");
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
