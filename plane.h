#pragma once
#include "geometry.h"

class PlaneGeometry : public Geometry {

public:

    PlaneGeometry() {
        create(1, 1);
    }

    void eval(float u, float v, vec3& pos) {
        float U = (u-0.5) * 512;
        float V = (v-0.5) * 512;

        pos = vec3(U, 0, V);
    }
};