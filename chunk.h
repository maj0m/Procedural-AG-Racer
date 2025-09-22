#pragma once

#include "framework.h"
#include "lut.h"
#include "shader.h"
#include "material.h"
#include "renderstate.h"
#include "volumecomputeshader.h"
#include <vector>
#include "GrassField.h"

class Chunk {
protected:
    vec3 id;
    vec3 color = vec3(0.7, 0.25, 0.14);
    Shader* shader;
    Material* material;
    unsigned int vbo = 0;
    float chunkSize;
    unsigned int tesselation = 32;
    unsigned int maxVertices;
    GLuint actualVertexCount;
    GrassField* grassField;

public:
    Chunk(vec3 id, float chunkSize, Shader* shader, Material* material, VolumeComputeShader* computeShader)
        : id(id), chunkSize(chunkSize), shader(shader), material(material) {

        grassField = new GrassField(32000, id, chunkSize);

        maxVertices = tesselation * tesselation * tesselation * 15;
        const GLsizeiptr headerSize = 16;
        const GLsizeiptr bufferSize = headerSize + sizeof(vec4) * maxVertices;

        // VBO
        glGenBuffers(1, &vbo);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, vbo);
        glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);
        
        // Zero header (vertexCount)
        GLuint zero = 0;
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint), &zero);

        // Bind to binding = 0
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbo);

        // Dispatch
        computeShader->Dispatch(tesselation / 8, tesselation / 8, tesselation / 8, id, chunkSize, tesselation);
        
        // Retrieve actual vertex count
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint), &actualVertexCount);
    }

    ~Chunk() {
        if (vbo) glDeleteBuffers(1, &vbo);
        grassField->destroy();
    }

    void Draw(RenderState& state) {
        mat4 M = ScaleMatrix(vec3(1, 1, 1)) * RotationMatrix(0, vec3(0, 1, 0)) * TranslateMatrix(vec3(0, 0, 0));
        state.M = M;
        state.MVP = state.M * state.V * state.P;
        state.material = material;
        shader->Bind(state);

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, vbo);
        glDrawArrays(GL_TRIANGLES, 0, actualVertexCount);

        grassField->Draw(state);
        // Debug
        if(KEYDOWN_E) grassField->destroy();
    }

    void ReloadGrassField(u_int lod) {
        // TODO
    }

    Chunk(const Chunk&) = delete;
    Chunk& operator=(const Chunk&) = delete;
};
