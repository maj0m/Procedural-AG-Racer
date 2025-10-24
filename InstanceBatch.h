#pragma once
#include "framework.h"
#include "geometry.h"
#include "shader.h"

class InstanceBatch {
    std::vector<Geometry*> geoms;
    Shader* shader = nullptr;

    std::vector<GLuint> vaos;
    std::vector<GLuint> instanceVBOs;
    std::vector<GLsizei> instanceCounts;

public:
    InstanceBatch(std::vector<Geometry*> geomList, Shader* shader) : geoms(geomList), shader(shader) {
        const size_t N = geoms.size();
        vaos.resize(N, 0);
        instanceVBOs.resize(N, 0);
        instanceCounts.resize(N, 0);

        for (size_t i = 0; i < N; i++) {
            glGenVertexArrays(1, &vaos[i]);
            glGenBuffers(1, &instanceVBOs[i]);

            glBindVertexArray(vaos[i]);

            // Bind GEOMETRY vertex buffer to attrib 0
            glBindBuffer(GL_ARRAY_BUFFER, geoms[i]->getVBO());
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);

            // Bind INSTANCE buffer to attribs 1-4
            glBindBuffer(GL_ARRAY_BUFFER, instanceVBOs[i]);
            const GLuint baseLoc = 1;
            for (int i = 0; i < 4; i++) {
                glEnableVertexAttribArray(baseLoc + i);
                glVertexAttribPointer(baseLoc + i, 4, GL_FLOAT, GL_FALSE, sizeof(mat4), (void*)(sizeof(vec4) * i));
                glVertexAttribDivisor(baseLoc + i, 1);
            }
        }

    }
    ~InstanceBatch() {
        for (auto vao : vaos) if (vao) glDeleteVertexArrays(1, &vao);
        for (auto vbo : instanceVBOs) if (vbo) glDeleteBuffers(1, &vbo);
    }

    void Update(const std::vector<std::vector<mat4>>& dataPerVariant, bool dynamic = false) {
        const size_t N = min(dataPerVariant.size(), instanceVBOs.size());

        for (size_t i = 0; i < N; i++) {
            std::vector<mat4> data = dataPerVariant[i];
            instanceCounts[i] = (GLsizei)data.size();
            glBindBuffer(GL_ARRAY_BUFFER, instanceVBOs[i]);
            glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(mat4), data.data(), dynamic ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);
        }
    }

    void Draw(RenderState& state) {
        shader->Bind(state);

        const size_t N = geoms.size();
        for (size_t i = 0; i < N; i++) {
            glBindVertexArray(vaos[i]);
            glDrawArraysInstanced(GL_TRIANGLES, 0, geoms[i]->getVertexCount(), instanceCounts[i]);
        }
    }

    size_t VariantCount() const { return geoms.size(); }
};
