#pragma once
#include "geometry.h"
#include "MeshGenerator.h"

class PlaneGeometry : public Geometry {
    float scale = 1.0f;
    int tesselation = 32;

public:
    PlaneGeometry(float scale, int tesselation) : scale(scale), tesselation(tesselation) {
        ParametricMeshGenerator meshGen(tesselation, [this](float u, float v) { return this->eval(u, v); });
        std::vector<vec3> vtxData;
        meshGen.generate(vtxData);
        init(vtxData); // upload to GPU
    }

    vec3 eval(float u, float v) const {
        float U = (u - 0.5f) * scale;
        float V = (v - 0.5f) * scale;

        return vec3(U, 0, V);
    }
};