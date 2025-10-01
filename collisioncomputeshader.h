#pragma once
#include "computeshader.h"
#include <iostream>

class CollisionComputeShader : public ComputeShader {
    GLuint collisionBuffer;

public:
    CollisionComputeShader() {
        create("collision.comp");

        glGenBuffers(1, &collisionBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, collisionBuffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(float), NULL, GL_DYNAMIC_DRAW);
    }

    void Dispatch(vec3 pos, float hoverHeight, float& outGroundDist, GLuint segIndexSSBO, GLuint segIndexCount) {
        glUseProgram(getId());

        setUniform(pos, "pos");
        setUniform(hoverHeight, "hoverHeight");
        setUniform((int)segIndexCount, "u_segIndexCount");

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, collisionBuffer);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, segIndexSSBO);

        glDispatchCompute(1, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        glBindBuffer(GL_SHADER_STORAGE_BUFFER, collisionBuffer);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(float), &outGroundDist);
    }


    ~CollisionComputeShader() {

    }
};
