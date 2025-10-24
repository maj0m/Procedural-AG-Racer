#pragma once
#include "geometry.h"
#include "MeshGenerator.h"

class CactusGeometry : public Geometry {
    float scale = 1.0f;
    int tesselation = 32;

public:
    CactusGeometry(float scale, int tesselation) : scale(scale), tesselation(tesselation) {
        SdfMeshGenerator meshGen(tesselation, scale, [this](vec3 p) { return this->sdf(p); });
        std::vector<vec3> vtxData;
        meshGen.generate(vtxData);
        init(vtxData); // upload to GPU
    }

    float sdf(vec3 p) {       
        const float trunkRadius = 3.2f;
        const float armDistH = 8.0f;
        const float ySmall = 20.0f;
        const float yBig = 15.0f;

        float trunk = sdCapsule(p, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 32.0f, 0.0f), trunkRadius);

        float smallH = sdCapsule(p, vec3(trunkRadius, ySmall, 0.0f), vec3(armDistH, ySmall, 0.0f), 2.0f);
        float smallV = sdCapsule(p, vec3(armDistH, ySmall, 0.0f), vec3(armDistH, ySmall + 8.0f, 0.0f), 2.0f);
        float smallArm = min(smallH, smallV);

        float bigH = sdCapsule(p, vec3(-trunkRadius, yBig, 0.0f), vec3(-armDistH, yBig, 0.0f), 2.0f);
        float bigV = sdCapsule(p, vec3(-armDistH, yBig, 0.0f), vec3(-armDistH, yBig + 10.0f, 0.0f), 2.0f);
        float bigArm = min(bigH, bigV);

        return min(trunk, min(smallArm, bigArm));
    }

    float sdCapsule(vec3 p, vec3 a, vec3 b, float r) {
        vec3 pa = p - a, ba = b - a;
        float h = dot(pa, ba) / dot(ba, ba);
        clamp(h, 0.0f, 1.0f);
        return length(pa - ba * h) - r;
    }
};