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

		setUniform(state.cameraPos, "u_camPos_WS");
		setUniform(state.M, "u_M");
		setUniform(state.MVP, "u_MVP");
	}
};
