#pragma once
#include "geometry.h"
#include "MeshGenerator.h"
#include "TreeParams.h"

class LeavesGeometry : public Geometry {
    TreeParams params;

public:
    LeavesGeometry(float radius, int tesselation, int leafCount, const TreeParams& params) : params(params) {

        LeafCloudGenerator gen(leafCount, radius, [this](vec3 p) { return this->sdf(p); }, tesselation, params.minLeafSize, params.maxLeafSize);

        std::vector<vec3> vtx;
        gen.generate(vtx);
        init(vtx);
    }

    float sdf(vec3 p) {
        vec3 pLeaves1 = p - params.branch1EndPos;
        vec3 pLeaves2 = p - params.branch2EndPos;

        float leaves1 = sdSphere(pLeaves1 / params.leaves1Scale, params.leaves1Radius) + displace(p);
        float leaves2 = sdSphere(pLeaves2 / params.leaves2Scale, params.leaves2Radius) + displace(p);
        float crown = opSmoothUnion(leaves1, leaves2, 10.0f);
        return crown;
    }

    float sdSphere(vec3 p, float r) {
        return length(p) - r;
    }

    float displace(vec3 p) {
        float baseFreq = 0.05f;
        float baseAmpl = 10.0f;
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
        return n * baseAmpl;
    }

    float opSmoothUnion(float d1, float d2, float k) {
        k *= 4.0f;
        float h = max(k - fabsf(d1 - d2), 0.0f);
        return min(d1, d2) - h * h * 0.25f / k;
    }
};
