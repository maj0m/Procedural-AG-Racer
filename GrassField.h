#pragma once
#include <vector>
#include <random>
#include "framework.h"
#include "grassshader.h"
#include "GrassScatterComputeShader.h"
#include "TrackManager.h"


struct GrassInstance {
    // 32 bytes
    vec3 pos;     // world pos
    float yaw;    // rotation around Y
    float height; // blade height
    float width;  // blade width
    float phase;  // wind sway
    float _pad1;
};

class GrassField {
public:
    GLuint vao = 0, bladeVBO = 0, instanceVBO = 0;
    size_t instanceCount = 0;   // final instance count after compute shader
    size_t capacity = 0;        // max attempts / capacity passed to compute shader
    Shader* shader = new GrassShader();
    Material* grassMaterial = new Material(vec3(0.5, 0.5, 0.5), vec3(0.4, 0.4, 0.4), vec3(0.4, 0.4, 0.4), 1.0);
    GrassScatterComputeShader scatterComputeShader;


    GrassField(size_t maxCount, vec3 chunkId, float chunkSize, int segIndexCount) {
        capacity = maxCount;
        
        // Base triangle
        const float bladeVerts[3 * 3] = {
            0.0f, 0.0f, 0.0f,   // base left
            1.0f, 0.0f, 0.0f,   // base right
            0.0f, 1.0f, 0.0f    // tip
        };

        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &bladeVBO);
        glBindBuffer(GL_ARRAY_BUFFER, bladeVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(bladeVerts), bladeVerts, GL_STATIC_DRAW);

        // layout(location=0) = vec3 aPos
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

        // Instance buffer (SSBO + VBO in the same GL buffer)
        glGenBuffers(1, &instanceVBO);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, instanceVBO);

        // Header + Payload
        const GLsizeiptr headerSize = 16; // 4 uints = 16 bytes
        const GLsizeiptr bufferSize = headerSize + GLsizeiptr(capacity) * sizeof(GrassInstance);
        glBufferData(GL_SHADER_STORAGE_BUFFER, bufferSize, nullptr, GL_DYNAMIC_DRAW);

        // Zero the atomic counter at offset 0
        GLuint zero = 0;
        glBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(GLuint), &zero);

        // Bind SSBO at binding = 1 for compute shader to write
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, instanceVBO);
        
        // Dispatch
        scatterComputeShader.Dispatch((GLuint)capacity, chunkId, chunkSize, segIndexCount);

        // Ensure writes are visible before reading count / using as vertex source
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);

        // Read back instanceCount
        glGetNamedBufferSubData(instanceVBO, 0, sizeof(GLuint), &instanceCount);

        // Vertex attributes
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        const GLsizei stride = sizeof(GrassInstance);

        // layout(location=1) = vec3 iPos
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(headerSize + offsetof(GrassInstance, pos)));
        glVertexAttribDivisor(1, 1);

        // layout(location=2) = float iYaw
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void*)(headerSize + offsetof(GrassInstance, yaw)));
        glVertexAttribDivisor(2, 1);

        // layout(location=3) = float iHeight
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (void*)(headerSize + offsetof(GrassInstance, height)));
        glVertexAttribDivisor(3, 1);

        // layout(location=4) = float iWidth
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, stride, (void*)(headerSize + offsetof(GrassInstance, width)));
        glVertexAttribDivisor(4, 1);

        // layout(location=5) = float iPhase
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, stride, (void*)(headerSize + offsetof(GrassInstance, phase)));
        glVertexAttribDivisor(5, 1);

        glBindVertexArray(0);
    }

    void Draw(RenderState& state) {
        mat4 M = ScaleMatrix(vec3(1, 1, 1)) * RotationMatrix(0, vec3(0, 1, 0)) * TranslateMatrix(vec3(0, 0, 0));
        state.M = M;
        state.MVP = state.M * state.V * state.P;
        state.material = grassMaterial;

        // save previous state
        GLint prevVAO = 0, prevProgram = 0;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &prevVAO);
        glGetIntegerv(GL_CURRENT_PROGRAM, &prevProgram);

        // set uniforms & draw
        shader->Bind(state);
        glBindVertexArray(vao);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 3, (GLsizei)instanceCount);

        // restore state
        glBindVertexArray(prevVAO);
        glUseProgram(prevProgram);
    }

    void destroy() {
        if (instanceVBO) glDeleteBuffers(1, &instanceVBO);
        if (bladeVBO)    glDeleteBuffers(1, &bladeVBO);
        if (vao)         glDeleteVertexArrays(1, &vao);
        vao = bladeVBO = instanceVBO = 0;
    }
};