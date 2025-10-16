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
	}
};
