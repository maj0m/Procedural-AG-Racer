#pragma once

struct TerrainData {
	float bedrockFrequency;
	float bedrockAmplitude;

	float frequency;
	float frequencyMultiplier;
	float amplitude;
	float amplitudeMultiplier;
	int octaves;
	float floorLevel;
	float blendFactor;

	float warpFreq;
	float warpAmp;
	float warpFreqMult;
	float warpAmpMult;
	int   warpOctaves;
	int   seed;
	float waterLevel;
};