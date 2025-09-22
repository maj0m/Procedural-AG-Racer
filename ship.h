#pragma once
#include "geometry.h"

class ShipGeometry : public Geometry {

public:
    ShipGeometry() {
        create(32, 0.0);
    }

    float SDF(vec3 p) {
        // Body
        vec3 halfSize = vec3(0.4, 0.2, 0.8);
        vec3 d = abs(p) - halfSize;
        float outsideDist = length(max3(d, vec3(0.0)));
        float insideDist = min(max(d.x, max(d.y, d.z)), 0.0);
        float body = outsideDist + insideDist;

        // Wings
        float right = sdCappedCylinder(p + vec3(0.6, -0.2, 0.5), 0.5, 0.3);
        float left = sdCappedCylinder(p + vec3(-0.6, -0.2, 0.5), 0.5, 0.3);

        return min(body, min(left, right));
        //return left;
    }

    float sdCappedCylinder(vec3 p, float h, float r) {
        vec2 d = abs(vec2(length(vec2(p.x, p.y)), p.z)) - vec2(r, h);
        return min(max(d.x, d.y), 0.0) + length(max2(d, vec2(0.0)));
    }

    // float arch = sdCappedTorus(p, vec2(1.0, 0.0), 2.5, 0.5);
    float sdCappedTorus(vec3 p, vec2 sc, float ra, float rb) {
        p.x = abs(p.x);
        float k = (sc.y * p.x > sc.x * p.y) ? dot(vec2(p.x, p.y), sc) : length(vec2(p.x, p.y));
        return sqrt(dot(p, p) + ra * ra - 2.0 * ra * k) - rb;
    }

    float smoothMin(float d1, float d2, float k) {
        // Calculate the smooth minimum
        float h = 0.5f + 0.5f * (d2 - d1) / k;
        clamp(h, 0.0, 1.0);
        return (1.0f - h) * d1 + h * d2 - k * h * (1.0f - h);
    }
};
