#pragma once
#include "geometry.h"
#include "MeshGenerator.h"

class SphereGeometry : public Geometry {
	float scale = 1.0f;
	int tesselation = 32;

public:
	SphereGeometry(float scale, int tesselation) : scale(scale), tesselation(tesselation) {
		ParametricMeshGenerator meshGen(tesselation, [this](float u, float v) { return this->eval(u, v); });
		std::vector<vec3> vtxData;
		meshGen.generate(vtxData);
		init(vtxData); // upload to GPU
	}

	vec3 eval(float u, float v) const {
		float U = u * 2.0f * M_PI;
		float V = v * M_PI;

		float X = scale * sinf(V) * cosf(U);
		float Y = scale * sinf(U) * sinf(V);
		float Z = scale * -cosf(V);

		return vec3(X, Y, Z);
	}
};