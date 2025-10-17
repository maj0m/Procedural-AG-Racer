#pragma once

#include "framework.h"
#include "lut.h"
#include "shader.h"
#include "renderstate.h"
#include "MarchingCubesCS.h"
#include <vector>
#include "GrassField.h"
#include "object.h"
#include "plane.h"
#include "terrainshader.h"
#include "TrackManager.h"
#include "InstanceField.h"
#include "SharedResources.h"
#include "WorldConfig.h"

class Chunk {
protected:
    vec3 id;
    
    WorldConfig* cfg = nullptr;
    SharedResources* resources = nullptr;

    unsigned int vbo = 0;

    unsigned int maxVertices = 0;
    GLuint actualVertexCount = 0;

    GrassField* grassField = nullptr;
    GLuint segIndexSSBO = 0;     // binding = 5
    GLuint segIndexCount = 0;    // small number per chunk
    std::unique_ptr<InstanceField> treeTrunkField;
    std::unique_ptr<InstanceField> treeCrownField;

public:
    Chunk(vec3 id, WorldConfig* cfg, SharedResources* resources, TrackManager* trackManager)
        : id(id), cfg(cfg), resources(resources) {

        // Build per-chunk list of road segments
        std::vector<int> indices;
        trackManager->GetSegmentsForChunk(id, cfg->chunkSize, indices);
        segIndexCount = (GLuint)indices.size();

        glGenBuffers(1, &segIndexSSBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, segIndexSSBO);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * segIndexCount, indices.data(), GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, segIndexSSBO);

        grassField = new GrassField(24000, id, cfg->chunkSize, segIndexCount);

        maxVertices = cfg->tesselation * cfg->tesselation * cfg->tesselation * 15;
        const GLsizeiptr headerSize = 16;
        const GLsizeiptr bufferSize = headerSize + sizeof(vec4) * maxVertices;

        // VBO
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, vbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, nullptr, GL_STATIC_DRAW);
        
        // Zero header (vertexCount)
        GLuint zero = 0;
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint), &zero);

        // Bind to binding = 0
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbo);

        // Dispatch
        resources->marchingCubesCS->Dispatch(cfg->tesselation / 8, cfg->tesselation / 8, cfg->tesselation / 8, id, cfg->chunkSize, cfg->tesselation, segIndexCount);
        
        // Retrieve actual vertex count
        glGetNamedBufferSubData(vbo, 0, sizeof(GLuint), &actualVertexCount);

        treeTrunkField = std::make_unique<InstanceField>(id, cfg->chunkSize, resources->treeTrunkGeoms, resources->treeTrunkShader, resources->terrainHeightCS, segIndexSSBO, segIndexCount);
        treeCrownField = std::make_unique<InstanceField>(id, cfg->chunkSize, resources->treeCrownGeoms, resources->treeLeafShader, resources->terrainHeightCS, segIndexSSBO, segIndexCount);
    }

    ~Chunk() {
        if (vbo) glDeleteBuffers(1, &vbo);
        if (segIndexSSBO) glDeleteBuffers(1, &segIndexSSBO);
        if (grassField) { grassField->destroy(); delete grassField; grassField = nullptr; }
    }

    void Draw(RenderState& state) {
        state.M = mat4();
        state.MVP = state.P * state.V * state.M;
        state.chunkId = id;
        state.chunkSize = cfg->chunkSize;

        resources->terrainShader->Bind(state);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbo);
        glDrawArrays(GL_TRIANGLES, 0, actualVertexCount);

        if(grassField) grassField->Draw(state);
        if (treeTrunkField)  treeTrunkField->Draw(state);
        if (treeCrownField)  treeCrownField->Draw(state);
    }

    // Getters
    GLuint getSegIndexSSBO()  const { return segIndexSSBO; }
    GLuint getSegIndexCount() const { return segIndexCount; }

    Chunk(const Chunk&) = delete;
    Chunk& operator=(const Chunk&) = delete;
};
