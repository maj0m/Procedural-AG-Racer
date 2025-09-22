#pragma once
#include <vector>
#include <random>
#include "framework.h"
#include "grassshader.h"
#include "GrassScatterComputeShader.h"



struct GrassInstance {
    // Packed to 32 bytes (nice for GPU)
    vec3 pos;     // world position (x,y,z)
    float yaw;    // rotation around Y in radians
    float height; // blade height multiplier
    float width;  // blade width multiplier
    float phase;  // wind sway desync
    float _pad1;
};

class GrassField {
public:
    GLuint vao = 0, bladeVBO = 0, instanceVBO = 0;
    size_t instanceCount = 0;
    Shader* shader = new GrassShader();
    vec3 color = vec3(0.4, 0.65, 0.34);
    Material* grassMaterial = new Material(vec3(0.5, 0.5, 0.5), vec3(0.4, 0.4, 0.4), vec3(0.4, 0.4, 0.4), 1.0);
    GrassScatterComputeShader g_scatterCS;

    GrassField(size_t count, vec3 chunkId, float chunkSize) {
        instanceCount = count;

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

        // Instance buffer (GPU-filled)
        glGenBuffers(1, &instanceVBO);
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, instanceCount * sizeof(GrassInstance), nullptr, GL_DYNAMIC_DRAW);

        // Bind the SAME buffer as an SSBO at binding=1 for the compute shader to write into
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, instanceVBO);

        int seed = 12345;

        // Dispatch compute to fill instances (writes pos.xz, yaw, height, width, phase)
        g_scatterCS.Dispatch((GLuint)instanceCount, chunkId, chunkSize, seed);

        // Define vertex attribs from this buffer,
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);

        // Per-instance attributes
        const GLsizei stride = sizeof(GrassInstance);

        // layout(location=1) = vec3 iPos
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(GrassInstance, pos));
        glVertexAttribDivisor(1, 1);

        // layout(location=2) = float iYaw
        glEnableVertexAttribArray(2);
        glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(GrassInstance, yaw));
        glVertexAttribDivisor(2, 1);

        // layout(location=3) = float iHeight
        glEnableVertexAttribArray(3);
        glVertexAttribPointer(3, 1, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(GrassInstance, height));
        glVertexAttribDivisor(3, 1);

        // layout(location=4) = float iWidth
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 1, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(GrassInstance, width));
        glVertexAttribDivisor(4, 1);

        // layout(location=5) = float iPhase
        glEnableVertexAttribArray(5);
        glVertexAttribPointer(5, 1, GL_FLOAT, GL_FALSE, stride, (void*)offsetof(GrassInstance, phase));
        glVertexAttribDivisor(5, 1);

        glBindVertexArray(0);
    }

    void Draw(RenderState& state) {
        mat4 M = ScaleMatrix(vec3(1, 1, 1)) * RotationMatrix(0, vec3(0, 1, 0)) * TranslateMatrix(vec3(0, 0, 0));
        state.M = M;
        state.MVP = state.M * state.V * state.P;
        state.material = grassMaterial;

        // save state
        GLint prevVAO = 0, prevProgram = 0;
        glGetIntegerv(GL_VERTEX_ARRAY_BINDING, &prevVAO);
        glGetIntegerv(GL_CURRENT_PROGRAM, &prevProgram);

        // save SSBO binding(0)
        GLint prevSSBO0 = 0;
        glGetIntegeri_v(GL_SHADER_STORAGE_BUFFER_BINDING, 0, &prevSSBO0);

        // set uniforms & draw
        shader->Bind(state);
        glBindVertexArray(vao);
        glDrawArraysInstanced(GL_TRIANGLES, 0, 3, (GLsizei)instanceCount);

        // restore state
        glBindVertexArray(prevVAO);
        glUseProgram(prevProgram);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, prevSSBO0);
    }

    void destroy() {
        if (instanceVBO) glDeleteBuffers(1, &instanceVBO);
        if (bladeVBO)    glDeleteBuffers(1, &bladeVBO);
        if (vao)         glDeleteVertexArrays(1, &vao);
        vao = bladeVBO = instanceVBO = 0;
    }
};