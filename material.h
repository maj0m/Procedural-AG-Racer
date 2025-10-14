#pragma once
#include "framework.h"

struct Material {
    vec4 kd;          // kd.xyz, kd.w unused
    vec4 ks;          // ks.xyz
    vec4 ka;          // ka.xyz
    vec4 shininess_pad;
};

class MaterialUBO {
public:
    static constexpr GLuint BINDING = 6;
    static constexpr int    MAX_MATERIALS = 16;

    GLuint ubo = 0;

    void init() {
        glGenBuffers(1, &ubo);
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferData(GL_UNIFORM_BUFFER, MAX_MATERIALS * sizeof(Material), nullptr, GL_DYNAMIC_DRAW);
        glBindBufferBase(GL_UNIFORM_BUFFER, BINDING, ubo);
    }

    // Fill/overwrite one slot
    void set(int index, const Material& m) {
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, index * sizeof(Material), sizeof(Material), &m);
    }

    // Bulk upload
    void setAll(const std::vector<Material>& list) {
        glBindBuffer(GL_UNIFORM_BUFFER, ubo);
        glBufferSubData(GL_UNIFORM_BUFFER, 0, MAX_MATERIALS * sizeof(Material), list.data());
    }

    ~MaterialUBO() { if (ubo) glDeleteBuffers(1, &ubo); }
};