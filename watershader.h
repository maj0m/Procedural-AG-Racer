#pragma once
#include "framework.h"
#include "shader.h"
#include "renderstate.h"

class WaterShader : public Shader {

public:
	WaterShader() {
		create("watershader.vert", "watershader.frag", "fragmentColor");
	}

	void Bind(RenderState state) {
		Use();

		setUniform(state.time, "u_time");
		setUniform(state.chunkId, "u_chunkId");
		setUniform(state.chunkSize, "u_chunkSize");
		setUniform(state.MVP, "MVP");
		setUniform(state.M, "M");
		setUniform(state.wEye, "wEye");
		setUniformMaterial(*state.material, "material");
		setUniformLight(state.light, "light");
	}
};
