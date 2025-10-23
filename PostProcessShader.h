#pragma once
#include "shader.h"

class PostProcessShader : public Shader {
public:
	PostProcessShader() {
		create("postprocess.vert", "postprocess.frag", "fragmentColor");
	}

	void Bind(RenderState state) override {
		Use();
		setUniform(0, "uScene");
	}
};