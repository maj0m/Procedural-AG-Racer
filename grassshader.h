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
		setUniform(state.MVP, "MVP");
		setUniform(state.wEye, "wEye");
	}
};
