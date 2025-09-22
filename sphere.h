#pragma once
#include "geometry.h"

class SphereGeometry : public Geometry {

public:

    SphereGeometry() {
        create(10, 1);
    }

    float SDF(vec3 pos) {
        return length(pos) - 10.0;
    }
};