#pragma once
#include "computeshader.h"
#include "tabletexture.h"
#include<iostream>
#include "terraindata.h"

class VolumeComputeShader : public ComputeShader {

    EdgeTableTexture* edgeTableTex = new EdgeTableTexture();
    TriTableTexture* triTableTex = new TriTableTexture();

public:
    VolumeComputeShader() {
        create("mc.comp");
    }

    void Dispatch(int workGroupsX, int workGroupsY, int workGroupsZ, vec3 chunkID, float chunkSize, int tesselation, int segIndexCount) {
        glUseProgram(getId());


        setUniform(123, "u_seed");
        setUniform(chunkID, "chunkID");
        setUniform(chunkSize, "chunkSize");
        setUniform(tesselation, "tesselation");
        setUniform(segIndexCount, "u_segIndexCount");
        setUniformTexture(*edgeTableTex, "edgeTableTex", 0);
        setUniformTexture(*triTableTex, "triTableTex", 1);

        glDispatchCompute(workGroupsX, workGroupsY, workGroupsZ);
    }

    void setUniformTexture(const EdgeTableTexture& texture, const std::string& name, unsigned int textureUnit = 0) {
        int location = glGetUniformLocation(getId(), name.c_str());
        if (location >= 0) {
            glUniform1i(location, textureUnit);
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, texture.textureId);
        }
    }

    void setUniformTexture(const TriTableTexture& texture, const std::string& name, unsigned int textureUnit = 0) {
        int location = glGetUniformLocation(getId(), name.c_str());
        if (location >= 0) {
            glUniform1i(location, textureUnit);
            glActiveTexture(GL_TEXTURE0 + textureUnit);
            glBindTexture(GL_TEXTURE_2D, texture.textureId);
        }
    }

    ~VolumeComputeShader() {
        delete edgeTableTex;
        delete triTableTex;
    }
};
