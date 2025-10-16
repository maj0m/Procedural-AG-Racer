#pragma once
#include "framework.h"
#include "geometry.h"
#include "shader.h"
#include "InstanceBatch.h"
#include "InstanceShader.h"
#include "TerrainHeightCS.h"

class InstanceField {
    std::unique_ptr<InstanceBatch> batch;
    TerrainHeightCS* terrainHeightCS;
    GLuint segSSBO = 0, segCount = 0;

public:
    InstanceField(const vec3& chunkId, float chunkSize, Geometry* objGeometry, Shader* instanceShader, TerrainHeightCS* terrainHeightCS, GLuint segSSBO, GLuint segCount) : terrainHeightCS(terrainHeightCS), segSSBO(segSSBO), segCount(segCount) {
        batch = std::make_unique<InstanceBatch>(objGeometry, instanceShader);
        std::vector<mat4> inst;
        scatterCPU(chunkId, chunkSize, inst);
        batch->Update(inst, false);
    }

    void Draw(RenderState& state) {
        glDisable(GL_CULL_FACE);
        batch->Draw(state);
        glEnable(GL_CULL_FACE);
    }

private:
    void scatterCPU(const vec3& chunkId, float chunkSize, std::vector<mat4>& out, float ratePerChunk = 0.1f, int maxPerChunk = 2) {
        // Deterministic per-chunk RNG
        uint32_t seed = hash3(chunkId);
        std::mt19937 rng(seed);

        // How many to spawn this chunk
        std::poisson_distribution<int> pois(ratePerChunk);
        int count = min(pois(rng), maxPerChunk); // cap extreme outliers

        auto randf = [&](float a, float b) {
            std::uniform_real_distribution<float> d(a, b);
            return d(rng);
        };

        for (int i = 0; i < count; ++i) {
            float x = (std::floor(chunkId.x) + randf(0.f, 1.f)) * chunkSize;
            float z = (std::floor(chunkId.z) + randf(0.f, 1.f)) * chunkSize;

            // Sample height
            float maxHeight = 50.0f;
            float objHeight = 0.0f;
            terrainHeightCS->Dispatch(vec3(x, maxHeight, z), maxHeight, objHeight, segSSBO, segCount);
            if (objHeight < 0.01f) continue; // skip if underground

            vec3 pos(x, objHeight, z);

            float yaw = randf(0.0f, 2.0f * M_PI);
            mat4 M = TranslateMatrix(pos) * RotationMatrix(yaw, vec3(0, 1, 0)) * ScaleMatrix(vec3(1, 1, 1));
            out.push_back(M);
        }
    }
};

