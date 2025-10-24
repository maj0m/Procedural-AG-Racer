#pragma once
#include "framework.h"
#include <functional>

struct MeshGenerator {
    virtual ~MeshGenerator() = default;
    virtual void generate(std::vector<vec3>& outVerts) = 0;
};


struct ParametricMeshGenerator : MeshGenerator {
    int tesselation;
    std::function<vec3(float u, float v)> eval;

    ParametricMeshGenerator(int tesselation, std::function<vec3(float, float)> eval) : tesselation(tesselation), eval(eval) {}

    void generate(std::vector<vec3>& vtxData) override {
        vtxData.reserve(tesselation * tesselation * 6); // 6 verts per quad

        const float step = 1.0f / float(tesselation);

        for (int i = 0; i < tesselation; ++i) {
            float v0 = float(i) * step;
            float v1 = float(i + 1) * step;

            for (int j = 0; j < tesselation; ++j) {
                float u0 = float(j) * step;
                float u1 = float(j + 1) * step;

                // Sample the four corners
                vec3 p00 = eval(u0, v0);
                vec3 p10 = eval(u1, v0);
                vec3 p11 = eval(u1, v1);
                vec3 p01 = eval(u0, v1);

                // Emit two triangles
                vtxData.push_back(p11);
                vtxData.push_back(p10);
                vtxData.push_back(p00);

                vtxData.push_back(p01);
                vtxData.push_back(p11);
                vtxData.push_back(p00);
            }
        }
    }
};


struct SdfMeshGenerator : MeshGenerator {
    float scale;
    int tesselation;
    std::function<float(vec3)> sdf;

    SdfMeshGenerator(int tesselation, float scale, std::function<float(vec3)> sdf) : scale(scale), tesselation(tesselation), sdf(sdf) {}

    void generate(std::vector<vec3>& vtxData) override {
        vtxData.reserve(tesselation * tesselation * tesselation * 15);

        const float step = (scale * 2.0f) / float(tesselation);

        for (int i = 0; i < tesselation; i++) {
            float x0 = -scale + i * step;
            for (int j = 0; j < tesselation; j++) {
                float y0 = -scale + j * step;
                for (int k = 0; k < tesselation; k++) {
                    float z0 = -scale + k * step;
                    vec3 p[8];
                    float val[8];

                    // Define cube vertices
                    p[0] = vec3(x0,         y0,         z0);
                    p[1] = vec3(x0 + step,  y0,         z0);
                    p[2] = vec3(x0 + step,  y0 + step,  z0);
                    p[3] = vec3(x0,         y0 + step,  z0);
                    p[4] = vec3(x0,         y0,         z0 + step);
                    p[5] = vec3(x0 + step,  y0,         z0 + step);
                    p[6] = vec3(x0 + step,  y0 + step,  z0 + step);
                    p[7] = vec3(x0,         y0 + step,  z0 + step);

                    // Evaluate scalar field at cube vertices
                    for (int n = 0; n < 8; n++)
                        val[n] = sdf(p[n]);

                    // Determine cube index
                    int cubeindex = 0;
                    for (int n = 0; n < 8; n++)
                        if (val[n] < 0.0f) cubeindex |= 1 << n;

                    // Skip if the cube is entirely inside or outside
                    if (edgeTable[cubeindex] == 0)
                        continue;

                    // Find the vertices where the surface intersects the cube
                    vec3 vertlist[12];
                    if (edgeTable[cubeindex] & 1)       vertlist[0] = VertexInterp(0.0f, p[0], p[1], val[0], val[1]);
                    if (edgeTable[cubeindex] & 2)       vertlist[1] = VertexInterp(0.0f, p[1], p[2], val[1], val[2]);
                    if (edgeTable[cubeindex] & 4)       vertlist[2] = VertexInterp(0.0f, p[2], p[3], val[2], val[3]);
                    if (edgeTable[cubeindex] & 8)       vertlist[3] = VertexInterp(0.0f, p[3], p[0], val[3], val[0]);
                    if (edgeTable[cubeindex] & 16)      vertlist[4] = VertexInterp(0.0f, p[4], p[5], val[4], val[5]);
                    if (edgeTable[cubeindex] & 32)      vertlist[5] = VertexInterp(0.0f, p[5], p[6], val[5], val[6]);
                    if (edgeTable[cubeindex] & 64)      vertlist[6] = VertexInterp(0.0f, p[6], p[7], val[6], val[7]);
                    if (edgeTable[cubeindex] & 128)     vertlist[7] = VertexInterp(0.0f, p[7], p[4], val[7], val[4]);
                    if (edgeTable[cubeindex] & 256)     vertlist[8] = VertexInterp(0.0f, p[0], p[4], val[0], val[4]);
                    if (edgeTable[cubeindex] & 512)     vertlist[9] = VertexInterp(0.0f, p[1], p[5], val[1], val[5]);
                    if (edgeTable[cubeindex] & 1024)    vertlist[10] = VertexInterp(0.0f, p[2], p[6], val[2], val[6]);
                    if (edgeTable[cubeindex] & 2048)    vertlist[11] = VertexInterp(0.0f, p[3], p[7], val[3], val[7]);

                    // Create triangles
                    for (int n = 0; triTable[cubeindex][n] != -1; n += 3) {
                        vec3 v0 = vertlist[triTable[cubeindex][n]];
                        vec3 v1 = vertlist[triTable[cubeindex][n + 1]];
                        vec3 v2 = vertlist[triTable[cubeindex][n + 2]];

                        // Add vertices
                        vtxData.push_back(v0);
                        vtxData.push_back(v2);
                        vtxData.push_back(v1);
                    }
                }
            }
        }
    }

    vec3 VertexInterp(float isolevel, vec3 p1, vec3 p2, float valp1, float valp2) {
        if (fabs(valp1 - valp2) < 0.00001) return p1;
        float mu = (isolevel - valp1) / (valp2 - valp1);
        return p1 + mu * (p2 - p1);
    }
};


struct LeafCloudGenerator : MeshGenerator {
    // Controls
    int   leafCount;          // Max number of leaves
    float radius;             // Half-size of the grid box
    float sizeMin;
    float sizeMax;

    std::function<float(vec3)> sdf;
    int   gridRes;

    LeafCloudGenerator(int leafCount, float radius, std::function<float(vec3)> sdf, int gridRes = 32, float sizeMin = 0.3f, float sizeMax = 0.8f)
        : leafCount(leafCount), radius(radius), sizeMin(sizeMin), sizeMax(sizeMax), sdf(std::move(sdf)), gridRes(gridRes) {}

    static inline vec3 anyPerp(const vec3& n) {
        vec3 t = (fabs(n.z) < 0.99f) ? vec3(0, 0, 1) : vec3(0, 1, 0);
        return normalize(cross(t, n));
    }
    inline vec3 sdfNormal(vec3 pLocal) const {
        const float e = 1e-2f;
        float dx = sdf(pLocal + vec3(e, 0, 0)) - sdf(pLocal - vec3(e, 0, 0));
        float dy = sdf(pLocal + vec3(0, e, 0)) - sdf(pLocal - vec3(0, e, 0));
        float dz = sdf(pLocal + vec3(0, 0, e)) - sdf(pLocal - vec3(0, 0, e));
        vec3 n = vec3(dx, dy, dz);
        float L = length(n);
        return (L > 0.0f) ? (n / L) : vec3(0, 1, 0);
    }

    void generate(std::vector<vec3>& outVerts) override {
        outVerts.clear();
        outVerts.reserve(leafCount * 3);

        std::mt19937 rng(1337);
        std::uniform_real_distribution<float> U(0.0f, 1.0f);
        std::uniform_real_distribution<float> Size(sizeMin, sizeMax);

        const vec3 bmin = vec3(-radius, - radius, -radius);
        const vec3 bmax = vec3(+radius, + radius, +radius);
        const vec3 dim = bmax - bmin;

        const vec3  step = dim / float(gridRes);
        const vec3  half = step * 0.5f; // center offset within each cell

        std::vector<vec3> centers;
        centers.reserve(gridRes * gridRes * gridRes / 4);

        for (int ix = 0; ix < gridRes; ++ix) {
            for (int iy = 0; iy < gridRes; ++iy) {
                for (int iz = 0; iz < gridRes; ++iz) {
                    vec3 pWorld = bmin + vec3(ix, iy, iz) * step + half;

                    if (fabsf(sdf(pWorld)) < 5.0f) {
                        vec3 jitter = vec3(U(rng) - 0.5f, U(rng) - 0.5f, U(rng) - 0.5f) * step * 0.8f;
                        centers.push_back(pWorld + jitter);
                    }
                }
            }
        }

        if (centers.empty()) {
            return;
        }

        // Shuffle and pick up to leafCount centers
        std::shuffle(centers.begin(), centers.end(), rng);
        const int used = min(leafCount, (int)centers.size());

        for (int i = 0; i < used; ++i) {
            vec3 center = centers[i];

            // Orientation (aligned to SDF normal)
            vec3 n = sdfNormal(center);
            vec3 uAxis = anyPerp(n);
            vec3 vAxis = normalize(cross(n, uAxis));

            // Random in-plane rotation
            float theta = U(rng) * 2.0f * float(M_PI);
            float c = cosf(theta), s = sinf(theta);
            vec3 ux = uAxis * c + vAxis * s;
            vec3 vx = -uAxis * s + vAxis * c;

            // Leaf triangle
            float side = Size(rng);
            float d = side / sqrtf(3.0f);

            vec3 v0 = center + d * (ux * 1.0f);
            vec3 v1 = center + d * (-0.5f * ux + (sqrtf(3.0f) / 2.0f) * vx);
            vec3 v2 = center + d * (-0.5f * ux - (sqrtf(3.0f) / 2.0f) * vx);

            outVerts.push_back(v0);
            outVerts.push_back(v1);
            outVerts.push_back(v2);
        }
    }
};
