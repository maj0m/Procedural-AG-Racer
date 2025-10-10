#pragma once
#include "geometry.h"
#include "MeshGenerator.h"

class ShipGeometry : public Geometry {
	float scale = 1.0f;
	int tesselation = 32;
	
public:
	ShipGeometry(float scale, int tesselation) : scale(scale), tesselation(tesselation) {
		SdfMeshGenerator meshGen(tesselation, scale, [this](vec3 p) { return this->sdf(p); });
		std::vector<vec3> vtxData;
		meshGen.generate(vtxData);
		init(vtxData); // upload to GPU
	}

    float sdf(vec3 p) {
        // Body
        vec3 bodyTransform = vec3(1.0f, 2.0f, 0.5f);
        return sdSphere(p * bodyTransform, scale/2.0f);
    }

    float sdSphere(vec3 p, float r) {
        return length(p) - r;
    }

    float sdCappedCylinder(vec3 p, float h, float r) {
        vec2 d = abs(vec2(length(vec2(p.x, p.y)), p.z)) - vec2(r, h);
        return min(max(d.x, d.y), 0.0) + length(max2(d, vec2(0.0)));
    }

    // float arch = sdCappedTorus(p, vec2(1.0, 0.0), 2.5, 0.5);
    float sdCappedTorus(vec3 p, vec2 sc, float ra, float rb) {
        p.x = abs(p.x);
        float k = (sc.y * p.x > sc.x * p.y) ? dot(vec2(p.x, p.y), sc) : length(vec2(p.x, p.y));
        return sqrt(dot(p, p) + ra * ra - 2.0 * ra * k) - rb;
    }

    float smoothMin(float d1, float d2, float k) {
        // Calculate the smooth minimum
        float h = 0.5f + 0.5f * (d2 - d1) / k;
        clamp(h, 0.0, 1.0);
        return (1.0f - h) * d1 + h * d2 - k * h * (1.0f - h);
    }
};