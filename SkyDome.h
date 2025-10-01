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
		geometry = new SphereGeometry();
	}

	void Draw(RenderState& state) {
		mat4 M = TranslateMatrix(state.wEye) * ScaleMatrix(vec3(1.0, 1.0, 1.0));
		state.M = M;
		state.MVP = state.P * state.V * state.M;
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
