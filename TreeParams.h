#pragma once
#include "framework.h"

struct TreeParams {
private:
    std::mt19937 rng;

    auto randf() {
        return [&](float a, float b) {
            std::uniform_real_distribution<float> d(a, b);
            return d(this->rng);
        };
    }

public:
    float trunkRadius;
    float branchRadius;
    vec3 trunkStartPos;
    vec3 trunkEndPos;
    vec3 branch1Offset;
    vec3 branch2Offset;
    vec3 branch1StartPos;
    vec3 branch2StartPos;
    vec3 branch1EndPos;
    vec3 branch2EndPos;
    float leaves1Radius;
    float leaves2Radius;
    vec3 leaves1Scale;
    vec3 leaves2Scale;
    float minLeafSize;
    float maxLeafSize;

    TreeParams(uint32_t seed) : rng(seed) {
        auto r = randf();

        trunkRadius = 8.0f;
        branchRadius = 7.0f;
        trunkStartPos = vec3(0.0f, 0.0f, 0.0f);

        trunkEndPos = vec3(r(0.0f, 15.0f), r(15.0f, 60.0f), r(0.0f, 15.0f));
        branch1Offset = vec3(r(10.0f, 60.0f), r(60.0f, 90.0f), r(-10.0f, 10.0f));
        branch2Offset = vec3(r(-60.0f, -10.0f), r(60.0f, 90.0f), r(-10.0f, 10.0f));

        branch1StartPos = trunkEndPos;
        branch2StartPos = trunkEndPos;
        branch1EndPos = trunkEndPos + branch1Offset;
        branch2EndPos = trunkEndPos + branch2Offset;

        leaves1Radius = 50.0f;
        leaves2Radius = 50.0f;
        leaves1Scale = vec3(r(0.9f, 1.1f), r(0.7f, 0.9f), r(0.9f, 1.1f));
        leaves2Scale = vec3(r(0.9f, 1.1f), r(0.7f, 0.9f), r(0.9f, 1.1f));
        minLeafSize = 16.0f;
        maxLeafSize = 20.0f;
    }
};