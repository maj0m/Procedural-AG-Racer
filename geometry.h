#pragma once
#include "framework.h"
#include "lut.h"

class Geometry {
protected:
	unsigned int vao, vbo;
    unsigned int nVertices;

	struct VertexData {
		vec3 pos;
		vec2 tex;
	};

    virtual float SDF(vec3 p) = 0;

public:
	Geometry() {
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
	}

    vec3 VertexInterp(float isolevel, vec3 p1, vec3 p2, float valp1, float valp2) {
        if (fabs(isolevel - valp1) < 0.00001)
            return p1;
        if (fabs(isolevel - valp2) < 0.00001)
            return p2;
        if (fabs(valp1 - valp2) < 0.00001)
            return p1;
        float mu = (isolevel - valp1) / (valp2 - valp1);
        return p1 + mu * (p2 - p1);
    }

    void create(int N, float isolevel) {
        std::vector<VertexData> vtxData;
        float min = 1.5f;
        float xmin = -min, xmax = min;
        float ymin = -min, ymax = min;
        float zmin = -min, zmax = min;
        float dx = (xmax - xmin) / N;
        float dy = (ymax - ymin) / N;
        float dz = (zmax - zmin) / N;

        for (int i = 0; i < N; i++) {
            float x0 = xmin + i * dx;
            for (int j = 0; j < N; j++) {
                float y0 = ymin + j * dy;
                for (int k = 0; k < N; k++) {
                    float z0 = zmin + k * dz;
                    vec3 p[8];
                    float val[8];

                    // Define cube vertices
                    p[0] = vec3(x0, y0, z0);
                    p[1] = vec3(x0 + dx, y0, z0);
                    p[2] = vec3(x0 + dx, y0 + dy, z0);
                    p[3] = vec3(x0, y0 + dy, z0);
                    p[4] = vec3(x0, y0, z0 + dz);
                    p[5] = vec3(x0 + dx, y0, z0 + dz);
                    p[6] = vec3(x0 + dx, y0 + dy, z0 + dz);
                    p[7] = vec3(x0, y0 + dy, z0 + dz);

                    // Evaluate scalar field at cube vertices
                    for (int n = 0; n < 8; n++)
                        val[n] = SDF(p[n]);

                    // Determine cube index
                    int cubeindex = 0;
                    for (int n = 0; n < 8; n++)
                        if (val[n] < isolevel) cubeindex |= 1 << n;

                    // Skip if the cube is entirely inside or outside
                    if (edgeTable[cubeindex] == 0)
                        continue;

                    // Find the vertices where the surface intersects the cube
                    vec3 vertlist[12] = {};
                    if (edgeTable[cubeindex] & 1)
                        vertlist[0] = VertexInterp(isolevel, p[0], p[1], val[0], val[1]);
                    if (edgeTable[cubeindex] & 2)
                        vertlist[1] = VertexInterp(isolevel, p[1], p[2], val[1], val[2]);
                    if (edgeTable[cubeindex] & 4)
                        vertlist[2] = VertexInterp(isolevel, p[2], p[3], val[2], val[3]);
                    if (edgeTable[cubeindex] & 8)
                        vertlist[3] = VertexInterp(isolevel, p[3], p[0], val[3], val[0]);
                    if (edgeTable[cubeindex] & 16)
                        vertlist[4] = VertexInterp(isolevel, p[4], p[5], val[4], val[5]);
                    if (edgeTable[cubeindex] & 32)
                        vertlist[5] = VertexInterp(isolevel, p[5], p[6], val[5], val[6]);
                    if (edgeTable[cubeindex] & 64)
                        vertlist[6] = VertexInterp(isolevel, p[6], p[7], val[6], val[7]);
                    if (edgeTable[cubeindex] & 128)
                        vertlist[7] = VertexInterp(isolevel, p[7], p[4], val[7], val[4]);
                    if (edgeTable[cubeindex] & 256)
                        vertlist[8] = VertexInterp(isolevel, p[0], p[4], val[0], val[4]);
                    if (edgeTable[cubeindex] & 512)
                        vertlist[9] = VertexInterp(isolevel, p[1], p[5], val[1], val[5]);
                    if (edgeTable[cubeindex] & 1024)
                        vertlist[10] = VertexInterp(isolevel, p[2], p[6], val[2], val[6]);
                    if (edgeTable[cubeindex] & 2048)
                        vertlist[11] = VertexInterp(isolevel, p[3], p[7], val[3], val[7]);



                    // Create triangles
                    for (int n = 0; triTable[cubeindex][n] != -1; n += 3) {
                        vec3 v0 = vertlist[triTable[cubeindex][n]];
                        vec3 v1 = vertlist[triTable[cubeindex][n + 1]];
                        vec3 v2 = vertlist[triTable[cubeindex][n + 2]];


                        // Add vertices
                        vtxData.push_back({ v0, vec2(0) });
                        vtxData.push_back({ v1, vec2(0) });
                        vtxData.push_back({ v2, vec2(0) });

                    }
                }
            }
        }


     
        nVertices = vtxData.size();

        // Upload to GPU
        glBindVertexArray(vao);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferData(GL_ARRAY_BUFFER, vtxData.size() * sizeof(VertexData), &vtxData[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0); // AttArr 0 = POSITION
        glEnableVertexAttribArray(1); // AttArr 1 = UV
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, pos));
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexData), (void*)offsetof(VertexData, tex));
    }




	void Draw() {
		glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, nVertices);
	}

	virtual ~Geometry() {
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}
};
