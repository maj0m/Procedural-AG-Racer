#pragma once
#include "computeshader.h"
#include <iostream>
#include "terraindata.h"

class CollisionComputeShader : public ComputeShader {
    GLuint collisionBuffer;
    struct CollisionData {
        float groundDist;
        alignas(16) vec3 normal;
    };

public:
    CollisionComputeShader() {
        create("collision.comp");

        glGenBuffers(1, &collisionBuffer);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, collisionBuffer);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(CollisionData), NULL, GL_DYNAMIC_DRAW);
    }

    void Dispatch(vec3 pos, float& groundDist, vec3& normal, TerrainData data) {
        glUseProgram(getId());

        setUniform(pos, "pos");
        setUniform(data.frequency, "u_frequency");
        setUniform(data.frequencyMultiplier, "u_frequencyMultiplier");
        setUniform(data.amplitude, "u_amplitude");
        setUniform(data.amplitudeMultiplier, "u_amplitudeMultiplier");
        setUniform(data.floorLevel, "u_floorLevel");
        setUniform(data.blendFactor, "u_blendFactor");

        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, collisionBuffer);
        glDispatchCompute(1, 1, 1);
        glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);

        CollisionData collisionData;
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, collisionBuffer);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(CollisionData), &collisionData);

        groundDist = collisionData.groundDist;
        normal = collisionData.normal;
    }


    ~CollisionComputeShader() {

    }
};
