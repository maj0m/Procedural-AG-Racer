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

		setUniform(state.cameraPos, "u_camPos_WS");
		setUniform(state.V, "u_V");
		setUniform(state.P, "u_P");

		// Shadow
		setUniform(state.lightVP, "u_lightVP");
		setUniform(state.shadowTexel, "u_shadowTexel");
		setUniform(state.shadowBias, "u_shadowBias");
	}
};
