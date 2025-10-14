#pragma once
#include "computeshader.h"

class GrassScatterCS : public ComputeShader {
public:
    GrassScatterCS() {
        create("grass_scatter.comp");
    }

    void Dispatch(int instanceCount, vec3 chunkId, float chunkSize, int segIndexCount) {
        glUseProgram(getId());

        setUniform(instanceCount, "u_instanceCount");
        setUniform(chunkId, "u_chunkId");
        setUniform(chunkSize, "u_chunkSize");
        setUniform(segIndexCount, "u_segIndexCount");

        const GLuint localSize = 256;
        GLuint groups = (instanceCount + localSize - 1) / localSize;
        glDispatchCompute(groups, 1, 1);
    }
};
