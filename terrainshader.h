#pragma once
#include "framework.h"
#include "shader.h"
#include "renderstate.h"

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
		setUniform((int)state.lights.size(), "nLights");
		for (unsigned int i = 0; i < state.lights.size(); i++) {
			setUniformLight(state.lights[i], std::string("lights[") + std::to_string(i) + std::string("]"));
		}
	}
};
