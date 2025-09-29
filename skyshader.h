#pragma once
#include "framework.h"
#include "shader.h"
#include "renderstate.h"

class SkyShader : public Shader {

public:
	SkyShader() {
		create("skyshader.vert", "skyshader.frag", "fragmentColor");
	}

	void Bind(RenderState state) {
		Use();

		//setUniform(state.time, "u_time");
		setUniform(state.MVP, "MVP");
		setUniform(state.M, "M");
		setUniform(state.wEye, "wEye");

		setUniform(state.light.dir, "u_sunDir");
		setUniform(0.08f, "u_sunDiscSize");
		setUniform(0.3f, "u_sunGlow");
	}
};
