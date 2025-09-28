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

		setUniform(state.time, "u_time");
		setUniform(state.MVP, "MVP");
		setUniform(state.M, "M");
		setUniform(state.wEye, "wEye");

		setUniform(vec3(0.06f, 0.22f, 0.60f) , "u_zenithColor");
		setUniform(vec3(0.65f, 0.78f, 0.90f), "u_horizonColor");
		setUniform(state.light.dir, "u_sunDir");
		setUniform(0.08f, "u_sunDiscSize");
		setUniform(0.3f, "u_sunGlow");
	}
};
