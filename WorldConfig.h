#pragma once
#include "framework.h"
#include "terraindata.h"

struct WorldConfig {
    float chunkSize = 256.0f;
    unsigned int renderDist = 8;
    unsigned int tesselation = 32;
    TerrainData terrain;
};
