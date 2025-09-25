#pragma once
#include "geometry.h"

class SphereGeometry : public Geometry {

public:

    SphereGeometry() {
        create(10, 0.0);
    }

    float SDF(vec3 p) {
        return length(p) - 10.0;
    }
};