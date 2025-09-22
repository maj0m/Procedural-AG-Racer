#pragma once
#include "volumecomputeshader.h"
#include "renderstate.h"
#include "FastNoiseLite.h"

class VolumeTexture {
    std::vector<vec4> image;
    int width = 100, height = 100, depth = 100;

public:
    unsigned int textureId = 0;

    VolumeTexture() {
        FastNoiseLite* noise = new FastNoiseLite(100);
        noise->SetNoiseType(FastNoiseLite::NoiseType_Perlin);
        noise->SetFrequency(0.05f);
        noise->SetFractalLacunarity(2.0f); // Ensure floating point literal is correctly specified
        noise->SetFractalGain(0.5f);

        image.resize(width * height * depth);
        for (int x = 0; x < width; x++) {
            for (int y = 0; y < height; y++) {
                for (int z = 0; z < depth; z++) {

                    vec3 n = (noise->GetNoise((float)x, (float)y, (float)z) + 1.0) * 5.0;
                    image[z * width * height + y * width + x] = vec4(n.x, n.y, n.z, 1.0f);
                }
            }
        }

        // Create and bind 3D texture
        glCreateTextures(GL_TEXTURE_3D, 1, &textureId);
        glTextureParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(textureId, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(textureId, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTextureParameteri(textureId, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
        glTextureStorage3D(textureId, 1, GL_RGBA32F, width, height, depth);
        glTextureSubImage3D(textureId, 0, 0, 0, 0, width, height, depth, GL_RGBA, GL_FLOAT, image.data());
        glBindImageTexture(0, textureId, 0, GL_TRUE, 0, GL_READ_WRITE, GL_RGBA32F);

        // deployComputeShader();
    }

    // void deployComputeShader() {
    //    VolumeComputeShader* computeShader = new VolumeComputeShader();
    //    computeShader->Bind();
    // }
};
