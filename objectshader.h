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

		setUniform(state.MVP, "MVP");
		setUniform(state.M, "M");
		setUniform(state.wEye, "wEye");
		setUniformMaterial(*state.material, "material");
		setUniformLight(state.light, "light");
	}
};
