#pragma once
#include "geometry.h"

class ShipGeometry : public Geometry {
public:
	int tesselation = 6;
	float scale = 2;

	ShipGeometry() {
		create(tesselation, tesselation);
	}

	void eval(float u, float v, vec3& pos) {
		float U = u * 2.0f * M_PI;
		float V = v * M_PI;

		float X = scale * sinf(V) * cosf(U);
		float Y = scale * sinf(U) * sinf(V) * 0.5;
		float Z = scale * -cosf(V) * 2.0;

		pos = vec3(X, Y, Z);
	}
};