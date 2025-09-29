#pragma once
#include "framework.h"
#include "chunk.h"
#include <unordered_map>
#include <string>
#include "camera.h"
#include <memory>
#include "object.h"
#include "plane.h"
#include "terrainshader.h"
#include "TrackManager.h"
#include "watershader.h"

class ChunkManager {
private:
    std::unordered_map<vec3, std::unique_ptr<Chunk>, Vec3Hash, Vec3Equal> chunkMap;
    Shader* terrainShader;
    Material* terrainMaterial;
    VolumeComputeShader* volumeComputeShader;
    TerrainData terrainData;
    unsigned int chunkSize;
    int renderDistance;
    GLuint vao = 0;         // Shared VAO for all chunks
    GLuint terrainUBO = 0;  // Terrain UBO that any shader can access
    TrackManager* trackManager;
    Object* water;

public:
    ChunkManager(float chunkSize, int renderDistance, TerrainData terrainData) : chunkSize(chunkSize), renderDistance(renderDistance), terrainData(terrainData) {
        terrainShader = new TerrainShader();
        volumeComputeShader = new VolumeComputeShader();
        terrainMaterial = new Material(vec3(0.5, 0.5, 0.5), vec3(0.4, 0.4, 0.4), vec3(0.4, 0.4, 0.4), 1.0);
        trackManager = new TrackManager();
        water = new Object(new WaterShader(), new PlaneGeometry(100, chunkSize * ((float)renderDistance * 4.0 + 1.0)));

        // Create shared VAO for all chunks
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);
        glBindVertexArray(0);

        // UBO for terrain params (binding = 3)
        glGenBuffers(1, &terrainUBO);
        glBindBuffer(GL_UNIFORM_BUFFER, terrainUBO);
        glBufferData(GL_UNIFORM_BUFFER, sizeof(TerrainData), nullptr, GL_DYNAMIC_DRAW);
        glBindBufferRange(GL_UNIFORM_BUFFER, 3, terrainUBO, 0, sizeof(TerrainData));

        updateTerrainUBO();
    }

    ~ChunkManager() {
        if (vao) glDeleteVertexArrays(1, &vao);
        if (terrainUBO) glDeleteBuffers(1, &terrainUBO);
        delete terrainShader;
        delete volumeComputeShader;
        delete terrainMaterial;
    }

    void LoadChunk(const vec3& id) {
        if (chunkMap.find(id) == chunkMap.end()) {
            chunkMap.emplace(id, std::make_unique<Chunk>(id, chunkSize, terrainShader, terrainMaterial, volumeComputeShader, trackManager));
        }
    }

    void UnloadChunk(const vec3& id) {
        chunkMap.erase(id);
    }

    void Update(const vec3& cameraPos) {
        vec3 currentChunk = vec3(floor(cameraPos.x / chunkSize), floor(cameraPos.y / chunkSize), floor(cameraPos.z / chunkSize));

        for (int x = -renderDistance; x <= renderDistance; ++x) {
            for (int z = -renderDistance; z <= renderDistance; ++z) {
                LoadChunk(vec3(currentChunk.x + x, 0, currentChunk.z + z));
            }
        }

        std::vector<vec3> chunksToUnload;
        for (const auto& pair : chunkMap) {
            vec3 chunkId = pair.first;
            if (abs(chunkId.x - currentChunk.x) > renderDistance || abs(chunkId.z - currentChunk.z) > renderDistance) {
                chunksToUnload.push_back(chunkId);
            }
        }

        for (const vec3& id : chunksToUnload) {
            UnloadChunk(id);
        }
    }

    bool isChunkVisible(const vec3& center, float chunkSize, const std::vector<vec4>& frustumPlanes, const vec3& cameraPos) {
        // Build AABB
        const float half = 0.5f * chunkSize;
        AABB box;
        box.min = center - vec3(half, half, half);
        box.max = center + vec3(half, half, half);

        // If the camera is inside the box, always draw
        if (pointInAABB(cameraPos, box)) return true;

        // p-vertex test
        for (const vec4& p : frustumPlanes) {
            vec3 n = vec3(p.x, p.y, p.z);

            // choose farthest corner in direction of the plane normal
            vec3 pv = vec3(
                n.x >= 0.0f ? box.max.x : box.min.x,
                n.y >= 0.0f ? box.max.y : box.min.y,
                n.z >= 0.0f ? box.max.z : box.min.z
            );

            float dist = dot(n, pv) + p.w;
            if (dist < 0) return false; // outside
        }
        return true; // intersects or fully inside
    }


    void ReloadChunks() {
        // Create a vector to store chunk IDs before reloading them
        std::vector<vec3> chunkIds;

        // Store the IDs of all currently loaded chunks
        for (const auto& pair : chunkMap) {
            chunkIds.push_back(pair.first);
        }

        // Unload all chunks
        for (const vec3& id : chunkIds) {
            UnloadChunk(id);
        }

        // Reload all chunks
        for (const vec3& id : chunkIds) {
            LoadChunk(id);
        }
    }


    void DrawChunks(RenderState& state, Camera& camera) {
        auto frustumPlanes = camera.getFrustumPlanes();

        glBindVertexArray(vao);

        int totalChunks = 0;
        int drawnChunks = 0;
        for (auto& pair : chunkMap) {
            totalChunks++;
            vec3 chunkCenter = pair.first * chunkSize + vec3(chunkSize / 2.0f, chunkSize / 2.0f, chunkSize / 2.0f);
            if (isChunkVisible(chunkCenter,chunkSize, frustumPlanes, camera.getEyePos())) {
                drawnChunks++;
                pair.second->Draw(state);
            }
        }

        water->Draw(state);
    }

    void updateTerrainUBO() {
        // order must match the GLSL block
        struct Params {
            float bedrockFrequency;
            float bedrockAmplitude;
            float frequency;
            float frequencyMultiplier;
            float amplitude;
            float amplitudeMultiplier;
            float floorLevel;
            float blendFactor;

            float warpFreq;
            float warpAmp;
            float warpStrength;
            float warpFreqMult;
            float warpAmpMult;
            int   warpOctaves;
            int   seed;
            float   _pad4;
        } p{
            terrainData.bedrockFrequency,
            terrainData.bedrockAmplitude,
            terrainData.frequency,
            terrainData.frequencyMultiplier,
            terrainData.amplitude,
            terrainData.amplitudeMultiplier,
            terrainData.floorLevel,
            terrainData.blendFactor,
            terrainData.warpFreq,
            terrainData.warpAmp,
            terrainData.warpStrength,
            terrainData.warpFreqMult,
            terrainData.warpAmpMult,
            terrainData.warpOctaves,
            terrainData.seed,
            terrainData._pad4
        };

        glBindBuffer(GL_UNIFORM_BUFFER, terrainUBO);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(p), &p);
    }

    vec3 getSpawnPoint() {
        float x = trackManager->segments[0].start_r.x;
        float y = trackManager->segments[0].start_r.y;
        float z = trackManager->segments[0].start_r.z;
        return vec3(x, y, z);
    }


    // Getters
    TerrainData getTerrainData() const { return terrainData; }

    // Setters
    void setTerrainData(const TerrainData& data) {
        terrainData = data;
        updateTerrainUBO();
    }
};
