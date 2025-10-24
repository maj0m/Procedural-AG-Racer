#pragma once
#include "framework.h"
#include "shader.h"
#include "geometry.h"
#include "renderstate.h"

struct Object {
	vec3 pos;
	vec3 scale;
	Quaternion rotation;
	Geometry* geometry;
	Shader* shader;

public:
	Object(Shader* _shader, Geometry* _geometry) {
		pos = vec3(0, 0, 0);
		scale = vec3(1, 1, 1);
		rotation = Quaternion(1, 0, 0, 0);
		geometry = _geometry;
		shader = _shader;
	}

	void Draw(RenderState& state) {
		state.M = TranslateMatrix(pos) * rotation.toRotationMatrix() * ScaleMatrix(scale);
		state.MVP = state.P * state.V * state.M;
		shader->Bind(state);
		geometry->Draw();
	}

	void SetRotation(Quaternion rot) {
		rotation = rot;
	}
};
