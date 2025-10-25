#pragma once
#include "computeshader.h"

class BedrockHeightCS : public ComputeShader {
    GLuint distanceBuffer;

public:
    BedrockHeightCS() {
        create("bedrock_height.comp");

        glGenBuffers(1, &distanceBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, distanceBuffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float), NULL, GL_DYNAMIC_DRAW);
    }

    void Dispatch(vec3 pos, float maxHeight, float& outGroundDist) {
        glUseProgram(getId());

        setUniform(pos, "pos");
        setUniform(maxHeight, "maxHeight");

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, distanceBuffer);

        glDispatchCompute(1, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, distanceBuffer);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float), &outGroundDist);
    }


    ~BedrockHeightCS() {
        if (distanceBuffer) glDeleteBuffers(1, &distanceBuffer);
    }
};
