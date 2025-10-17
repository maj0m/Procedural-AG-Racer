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
	}
};
