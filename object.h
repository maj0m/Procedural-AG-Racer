#pragma once
#include "framework.h"
#include "shader.h"
#include "geometry.h"
#include "renderstate.h"

struct Object {
	vec3 pos;
	vec3 scale;
	Quaternion rotation;
	Shader* shader;
	Geometry* geometry;

public:
	Object(Shader* _shader, Geometry* _geometry) {
		pos = vec3(0, 0, 0);
		scale = vec3(1, 1, 1);
		rotation = Quaternion(1, 0, 0, 0); // Identity
		shader = _shader;
		geometry = _geometry;
	}

	void Draw(RenderState& state) {
		mat4 rotationMatrix = rotation.toRotationMatrix();
		mat4 M = ScaleMatrix(scale) * rotationMatrix * TranslateMatrix(pos);

		state.M = M;
		state.MVP = state.M * state.V * state.P;
		shader->Bind(state);
		geometry->Draw();
	}

	void SetRotation(Quaternion rot) {
		rotation = rot;
	}
};
