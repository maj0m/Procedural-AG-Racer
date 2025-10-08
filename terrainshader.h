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

		setUniform(state.MVP, "MVP");
		setUniform(state.M, "M");
		setUniform(state.wEye, "wEye");
		setUniformMaterial(*state.material, "material");
	}
};
