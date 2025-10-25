#pragma once
#include "shader.h"

class PostProcessShader : public Shader {
public:
	PostProcessShader() {
		create("postprocess.vert", "postprocess.frag", "fragmentColor");
	}

	void Bind(RenderState state) override {
		Use();
		
		setUniform(0, "u_sceneColor");
		setUniform(1, "u_sceneDepth");
		setUniform(state.nearPlane, "u_near");
		setUniform(state.farPlane, "u_far");
		
		// Depth of Field
		setUniform(state.focusDist, "u_focusDist");
		setUniform(state.focusRange, "u_focusRange");

		// Bloom
		setUniform(state.bloomThreshold, "u_bloomThreshold");
		setUniform(state.bloomSoftKnee, "u_bloomSoftKnee");
		setUniform(state.bloomIntensity, "u_bloomIntensity");

		// Color grading
		setUniform(state.saturation, "u_saturation");
		setUniform(state.vibrance, "u_vibrance");
	}
};