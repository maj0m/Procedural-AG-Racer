#pragma once
#include "framework.h"
#include "shader.h"
#include "MarchingCubesCS.h"
#include "GroundDistanceCS.h"
#include "geometry.h"

struct SharedResources {
    // Shaders
    Shader*             terrainShader       = nullptr;
    Shader*             waterShader         = nullptr;
    Shader*             instanceShader      = nullptr;
    Shader*             treeTrunkShader     = nullptr;
    Shader*             treeLeafShader      = nullptr;
    MarchingCubesCS*    marchingCubesCS     = nullptr;
    GroundDistanceCS*   groundDistanceCS    = nullptr;
    TerrainHeightCS*    terrainHeightCS     = nullptr;

    // common geometries
    Geometry* waterGeom     = nullptr;
    Geometry* cactusGeom    = nullptr;
    std::vector<Geometry*> treeTrunkGeoms;
    std::vector<Geometry*> treeCrownGeoms;
};
