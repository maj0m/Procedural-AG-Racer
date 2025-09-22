#pragma once
#include "framework.h"
#include "lut.h"

class EdgeTableTexture {
    std::vector<vec4> image;
    int width = 256, height = 1;

public:
    unsigned int textureId;

    EdgeTableTexture() {
        glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(textureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(textureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureStorage2D(textureId, 1, GL_R32I, width, height);
        glTextureSubImage2D(textureId, 0, 0, 0, width, height, GL_RED_INTEGER, GL_INT, edgeTable);
    }
};

class TriTableTexture {
    std::vector<vec4> image;
    int width = 16, height = 256;
public:
    unsigned int textureId;

    TriTableTexture() {
        glCreateTextures(GL_TEXTURE_2D, 1, &textureId);
        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTextureParameteri(textureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(textureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureStorage2D(textureId, 1, GL_R32I, width, height);
        glTextureSubImage2D(textureId, 0, 0, 0, width, height, GL_RED_INTEGER, GL_INT, triTable);
    }
};