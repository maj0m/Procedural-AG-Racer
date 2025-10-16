#pragma once
#include "geometry.h"
#include "MeshGenerator.h"
#include "TreeParams.h"

class TrunkGeometry : public Geometry {
    float scale = 1.0f;
    int tesselation = 32;
    TreeParams params;

public:
    TrunkGeometry(float scale, int tesselation, const TreeParams& params) : scale(scale), tesselation(tesselation), params(params) {
        SdfMeshGenerator meshGen(tesselation, scale, [this](vec3 p) { return this->sdf(p); });
        std::vector<vec3> vtxData;
        meshGen.generate(vtxData);
        init(vtxData); // upload to GPU
    }

    float sdf(vec3 p) {
        float trunk = sdCapsule(p, params.trunkStartPos, params.trunkEndPos, params.trunkRadius);
        float branch1 = sdCapsule(p, params.branch1StartPos, params.branch1EndPos, params.branchRadius);
        float branch2 = sdCapsule(p, params.branch2StartPos, params.branch2EndPos, params.branchRadius);
        trunk = min(trunk, min(branch1, branch2));
        
        return trunk + displace(p);
    }

    float sdCapsule(vec3 p, vec3 a, vec3 b, float r) {
        vec3 pa = p - a, ba = b - a;
        float h = dot(pa, ba) / dot(ba, ba);
        clamp(h, 0.0f, 1.0f);
        return length(pa - ba * h) - r;
    }

    float displace(vec3 p) {
        float baseFreq = 0.16f;
        float warpFreq = 0.15f;
        float warpAmp = 6.0f;

        // Domain warp
        vec3 q = p + vec3(
            sin(p.y * warpFreq) + sin(p.z * warpFreq * 0.7f),
            sin(p.z * warpFreq * 1.3f) + sin(p.x * warpFreq),
            sin(p.x * warpFreq * 0.5f) + sin(p.y * warpFreq)
        ) * warpAmp;

        // Main displacement
        float n = sin(q.x * baseFreq) * sin(q.y * baseFreq) * sin(q.z * baseFreq);
        return n * 3.0f;
    }
};