#pragma once
#include "computeshader.h"

class ParticleUpdateCS : public ComputeShader {
public:
    ParticleUpdateCS() {
        create("particle_update.comp");
    }

    void Dispatch(float dt, int count, GLuint particleSSBO, GLuint freeListSSBO) {
        glUseProgram(getId());
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleSSBO);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, freeListSSBO);

        setUniform(dt, "u_dt");
        setUniform(count, "u_count");

        // Dispatch
        const GLuint localSize = 256;
        GLuint groups = (count + localSize - 1) / localSize;
        glDispatchCompute(groups, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    }
};
