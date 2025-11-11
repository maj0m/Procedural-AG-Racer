#pragma once
#include "framework.h"
#include "shader.h"
#include "renderstate.h"

class ParticleShader : public Shader {

public:
	ParticleShader() {
		create("particle.vert", "particle.frag", "fragmentColor", "particle.geom");
	}

	void Bind(RenderState state) {
		Use();

		setUniform(state.V, "u_V");
		setUniform(state.P, "u_P");
	}
};
