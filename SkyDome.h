#pragma once
#include "framework.h"
#include "shader.h"
#include "geometry.h"
#include "renderstate.h"
#include "skyshader.h"
#include "sphere.h"

class SkyDome {
	Shader* shader;
	Geometry* geometry;

public:
	SkyDome() {
		shader = new SkyShader();
		geometry = new SphereGeometry(1.0f, 32);
	}

	void Draw(RenderState& state) {
		state.M = TranslateMatrix(state.wEye) * ScaleMatrix(vec3(1.0, 1.0, 1.0));
		state.MVP = state.P * state.V * state.M;
		shader->Bind(state);

		// Disable depth testing
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);

		geometry->Draw();

		// Revert after drawing
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
	}
};
