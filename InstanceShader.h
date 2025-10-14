#pragma once
#include "framework.h"
#include "shader.h"
#include "renderstate.h"

class InstanceShader : public Shader {

public:
	InstanceShader() {
		create("instanceshader.vert", "instanceshader.frag", "fragmentColor");
	}

	void Bind(RenderState state) {
		Use();

		setUniform(state.V, "V");
		setUniform(state.P, "P");
		setUniform(state.wEye, "wEye");
		setUniformMaterial(*state.material, "material");
	}
};
