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
	Material* material;

public:
	Object(Shader* _shader, Geometry* _geometry) {
		pos = vec3(0, 0, 0);
		scale = vec3(1, 1, 1);
		rotation = Quaternion(1, 0, 0, 0); // Identity
		shader = _shader;
		geometry = _geometry;
		material = new Material(vec3(0.5, 0.5, 0.5), vec3(0.4, 0.4, 0.4), vec3(0.4, 0.4, 0.4), 1.0);
	}

	void Draw(RenderState& state) {
		mat4 rotationMatrix = rotation.toRotationMatrix();
		mat4 M = TranslateMatrix(pos) * rotationMatrix * ScaleMatrix(scale);

		state.M = M;
		state.MVP = state.P * state.V * state.M;
		state.material = material;
		shader->Bind(state);
		geometry->Draw();
	}

	void SetRotation(Quaternion rot) {
		rotation = rot;
	}
};
