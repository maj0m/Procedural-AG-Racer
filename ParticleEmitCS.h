#pragma once
#include "computeshader.h"

class ParticleEmitCS : public ComputeShader {
public:
    ParticleEmitCS() {
        create("particle_emit.comp");
    }

    void Dispatch(float time, float dt, int emitterCount, GLuint particleSSBO, GLuint freeListSSBO, GLuint emittersSSBO) {
        glUseProgram(getId());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, freeListSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, emittersSSBO);

        setUniform(time, "u_time");
        setUniform(dt, "u_dt");

        // Write emitter count into SSBO
        glNamedBufferSubData(emittersSSBO, 0, sizeof(int), &emitterCount);

        // Dispatch one workgroup per emitter
        const GLuint localSize = 128;
        GLuint groups = (emitterCount + localSize - 1) / localSize;
        glDispatchCompute(groups, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    }
};
