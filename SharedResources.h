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
    MarchingCubesCS*    marchingCubesCS     = nullptr;
    GroundDistanceCS*   groundDistanceCS    = nullptr;
    TerrainHeightCS*    terrainHeightCS     = nullptr;

    // common geometries
    Geometry* waterGeom     = nullptr;
    Geometry* cactusGeom    = nullptr;
    Geometry* treeTrunkGeom = nullptr;
    Geometry* treeCrownGeom = nullptr;
};
