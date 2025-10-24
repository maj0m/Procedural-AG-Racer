#pragma once
#include "framework.h"
#include "shader.h"
#include "renderstate.h"
#include "light.h"

class TerrainShader : public Shader {

public:
	TerrainShader() {
		create("terrainshader.vert", "terrainshader.frag", "fragmentColor");
	}

	void Bind(RenderState state) {
		Use();

		setUniform(state.V, "u_V");
		setUniform(state.P, "u_P");
		setUniform(state.wEye, "u_camPos_WS");

		// Shadow
		setUniform(state.lightVP, "u_lightVP");
		setUniform(state.shadowTexel, "u_shadowTexel");
		setUniform(state.shadowBias, "u_shadowBias");
		setUniform(2, "u_shadowMap");
	}
};
