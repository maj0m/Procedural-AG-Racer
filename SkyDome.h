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
		geometry = new Sphere();
	}

	void Draw(RenderState& state) {
		mat4 M = ScaleMatrix(vec3(1.0, 1.0, 1.0)) * TranslateMatrix(state.wEye);
		state.M = M;
		state.MVP = state.M * state.V * state.P;
		shader->Bind(state);

		// Disable depth testing and cull outside of dome
		glDepthMask(GL_FALSE);
		glDisable(GL_DEPTH_TEST);
		glCullFace(GL_FRONT);

		geometry->Draw();

		// Revert after drawing
		glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);
	}
};
