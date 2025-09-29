#pragma once
#include "geometry.h"

class PlaneGeometry : public Geometry {
    float scale;
    int tesselation;
public:


    PlaneGeometry(int _tesselation, float _scale) {
        scale = _scale;
        tesselation = _tesselation;
        create(tesselation, tesselation);
    }

    void eval(float u, float v, vec3& pos) {
        float U = u * scale;
        float V = v * scale;

        pos = vec3(U, 0, V);
    }
};