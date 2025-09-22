#pragma once
#include "computeshader.h"

class GrassScatterComputeShader : public ComputeShader {
public:
    GrassScatterComputeShader() {
        create("grass_scatter.comp");
    }

    void Dispatch(int instanceCount, vec3 chunkId, float chunkSize, uint32_t seed) {
        glUseProgram(getId());

        setUniform(instanceCount, "u_instanceCount");
       //setUniform(chunkId, "chunkID");
        setUniform(chunkSize, "u_chunkSize");

        //glUniform1ui(glGetUniformLocation(getId(), "u_instanceCount"), instanceCount);
        glUniform3f (glGetUniformLocation(getId(), "u_chunkId"), chunkId.x, chunkId.y, chunkId.z);
        //glUniform1f (glGetUniformLocation(getId(), "u_chunkSize"), chunkSize);
        glUniform1ui(glGetUniformLocation(getId(), "u_seed"), seed);

        const GLuint localSize = 256;
        GLuint groups = (instanceCount + localSize - 1) / localSize;
        glDispatchCompute(groups, 1, 1);

        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT | GL_VERTEX_ATTRIB_ARRAY_BARRIER_BIT);
    }
};
