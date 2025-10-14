#pragma once
#include "computeshader.h"

class GroundDistanceCS : public ComputeShader {
    GLuint distanceBuffer;

public:
    GroundDistanceCS() {
        create("ground_distance.comp");

        glGenBuffers(1, &distanceBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, distanceBuffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float), NULL, GL_DYNAMIC_DRAW);
    }

    void Dispatch(vec3 pos, float hoverHeight, float& outGroundDist, GLuint segIndexSSBO, GLuint segIndexCount) {
        glUseProgram(getId());

        setUniform(pos, "pos");
        setUniform(hoverHeight, "hoverHeight");
        setUniform((int)segIndexCount, "u_segIndexCount");

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, distanceBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, segIndexSSBO);

        glDispatchCompute(1, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, distanceBuffer);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float), &outGroundDist);
    }


    ~GroundDistanceCS() {
        if (distanceBuffer) glDeleteBuffers(1, &distanceBuffer);
    }
};
