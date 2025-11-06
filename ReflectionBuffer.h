#pragma once
#include "framework.h"

struct ReflectionBuffer {
    GLuint prevSceneColor = 0;
    GLuint sceneDepthCopy = 0;
    int width = 0, height = 0;

    void destroy() {
        if (prevSceneColor) { glDeleteTextures(1, &prevSceneColor); prevSceneColor = 0; }
        if (sceneDepthCopy) { glDeleteTextures(1, &sceneDepthCopy); sceneDepthCopy = 0; }
        width = height = 0;
    }

    void create(int w, int h) {
        destroy();
        width = w; height = h;

        glGenTextures(1, &prevSceneColor);
        glBindTexture(GL_TEXTURE_2D, prevSceneColor);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenTextures(1, &sceneDepthCopy);
        glBindTexture(GL_TEXTURE_2D, sceneDepthCopy);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    }

    // Copy depth so water can safely sample it
    void copyDepthFrom(GLuint srcDepth, int w, int h) {
        glCopyImageSubData(
            srcDepth, GL_TEXTURE_2D, 0, 0, 0, 0,
            sceneDepthCopy, GL_TEXTURE_2D, 0, 0, 0, 0,
            w, h, 1
        );
    }

    // Save previous frame color for SSR
    void updatePrevColor(GLuint srcColor, int w, int h) {
        glCopyImageSubData(
            srcColor, GL_TEXTURE_2D, 0, 0, 0, 0,
            prevSceneColor, GL_TEXTURE_2D, 0, 0, 0, 0,
            w, h, 1
        );
    }
};
