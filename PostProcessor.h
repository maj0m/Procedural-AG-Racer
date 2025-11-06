#pragma once
#include "framework.h"
#include "shader.h"
#include "PostProcessShader.h"
#include "RenderState.h"

class PostProcessor {
    GLuint fsVAO = 0, fsVBO = 0;
    PostProcessShader* shader = nullptr;

    void createFullscreenTriangle() {
        if (fsVAO) return;
        const float verts[] = {
            -1.0f, -1.0f,   0.0f, 0.0f,
             3.0f, -1.0f,   2.0f, 0.0f,
            -1.0f,  3.0f,   0.0f, 2.0f
        };
        glGenVertexArrays(1, &fsVAO);
        glGenBuffers(1, &fsVBO);
        glBindVertexArray(fsVAO);
        glBindBuffer(GL_ARRAY_BUFFER, fsVBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0); // position
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1); // uv
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glBindVertexArray(0);
    }

public:
    void init() {
        shader = new PostProcessShader();
        createFullscreenTriangle();
    }

    void destroy() {
        if (fsVBO) { glDeleteBuffers(1, &fsVBO); fsVBO = 0; }
        if (fsVAO) { glDeleteVertexArrays(1, &fsVAO); fsVAO = 0; }
        if (shader) { delete shader; shader = nullptr; }
    }

    // Runs post-process on default framebuffer
    void run(const RenderState& state, GLuint sceneColor, GLuint sceneDepth, int width, int height) {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, width, height);

        shader->Bind(state);

        // sceneColor = 0
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, sceneColor);
        glGenerateMipmap(GL_TEXTURE_2D); // generate lower mips for Bloom

        // sceneDepth = 1
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, sceneDepth);

        glBindVertexArray(fsVAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
    }
};
