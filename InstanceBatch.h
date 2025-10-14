#pragma once

#include "framework.h"
#include "geometry.h"
#include "shader.h"

class InstanceBatch {
    Geometry* geom = nullptr;
    Shader* shader = nullptr;
    GLuint  vao = 0;
    GLuint  instanceVBO = 0;
    GLsizei instanceCount = 0;

public:
    InstanceBatch(Geometry* g, Shader* s) : geom(g), shader(s) {
        glGenVertexArrays(1, &vao);
        glGenBuffers(1, &instanceVBO);

        glBindVertexArray(vao);

        // Bind GEOMETRY vertex buffer to attrib 0
        glBindBuffer(GL_ARRAY_BUFFER, geom->getVBO());
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

        // Bind INSTANCE buffer to attribs 1-4
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        const GLuint baseLoc = 1;
        for (int i = 0; i < 4; i++) {
            glEnableVertexAttribArray(baseLoc + i);
            glVertexAttribPointer(baseLoc + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(vec4) * i));
            glVertexAttribDivisor(baseLoc + i, 1);
        }
    }
    ~InstanceBatch() {
        if (vao) glDeleteVertexArrays(1, &vao);
        if (instanceVBO) glDeleteBuffers(1, &instanceVBO);
    }

    void Update(const std::vector<mat4>& data, bool dynamic = false) {
        instanceCount = (GLsizei)data.size();
        glBindBuffer(GL_ARRAY_BUFFER, instanceVBO);
        glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(mat4), data.data(), dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
    }

    void Draw(RenderState& state) {
        shader->Bind(state);
        glBindVertexArray(vao);
        glDrawArraysInstanced(GL_TRIANGLES, 0, geom->getVertexCount(), instanceCount);
    }
};
