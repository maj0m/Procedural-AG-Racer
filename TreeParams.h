#pragma once
#include "framework.h"

// Deterministic per-chunk RNG
uint32_t seed = hash3(33823);
std::mt19937 rng(seed);

auto randf = [&](float a, float b) {
    std::uniform_real_distribution<float> d(a, b);
    return d(rng);
    };

struct TreeParams {
    const float trunkRadius = 8.0f;
    const float branchRadius = 6.0f;
    const vec3 trunkStartPos = vec3(0.0f, 0.0f, 0.0f);
    const vec3 trunkEndPos = vec3(randf(0.0f, 15.0f), randf(40.0f, 75.0f), randf(0.0f, 15.0f));
    const float branch1OffsetX = randf(20.0f, 50.0f);
    const float branch1OffsetY = randf(20.0f, 70.0f);
    const float branch2OffsetX = randf(-50.0f, -20.0f);
    const float branch2OffsetY = randf(20.0f, 70.0f);

    const vec3 branch1StartPos = trunkEndPos;
    const vec3 branch2StartPos = trunkEndPos;
    vec3 branch1EndPos = vec3(branch1OffsetX, trunkEndPos.y + branch1OffsetY, 0.0f);
    vec3 branch2EndPos = vec3(branch2OffsetX, trunkEndPos.y + branch2OffsetY, 0.0f);

    float leaves1Radius = 50.0f;
    float leaves2Radius = 50.0f;
    vec3 leaves1Scale = vec3(randf(0.9f, 1.1f), randf(0.7f, 0.9f), randf(0.9f, 1.1f));
    vec3 leaves2Scale = vec3(randf(0.9f, 1.1f), randf(0.7f, 0.9f), randf(0.9f, 1.1f));
    float minLeafSize = 4.0f;
    float maxLeafSize = 8.0f;
};