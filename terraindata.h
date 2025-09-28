#pragma once

struct TerrainData {
	float bedrockFrequency;
	float bedrockAmplitude;

	float frequency;
	float frequencyMultiplier;
	float amplitude;
	float amplitudeMultiplier;
	float floorLevel;
	float blendFactor;

	float warpFreq;
	float warpAmp;
	float warpStrength;
	float warpFreqMult;
	float warpAmpMult;
	int   warpOctaves;
	int   seed;
	float _pad4;
};