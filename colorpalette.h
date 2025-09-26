#pragma once
#include "framework.h"

struct ColorPaletteUBO {
	vec4 terrainColors[5];
	vec4 grassColor;
	vec4 waterColor;
	vec4 fogColor;
	vec4 angleThresholds;
	float fogDensity;
	float _pad[3];
};

class ColorPalette {
private:
	ColorPaletteUBO palette;
	GLuint colorUBO; // binding = 7

public:
	ColorPalette() {
		glGenBuffers(1, &colorUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, colorUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(ColorPaletteUBO), nullptr, GL_STATIC_DRAW);
		glBindBufferBase(GL_UNIFORM_BUFFER, 7, colorUBO);

		UpdateColorPaletteUBO();
	}

	void UpdateColorPaletteUBO() {
		palette.terrainColors[0] = vec4(0.30f, 0.20f, 0.10f, 1.0f); // dirt
		palette.terrainColors[1] = vec4(0.45f, 0.35f, 0.20f, 1.0f); // soil
		palette.terrainColors[2] = vec4(0.60f, 0.55f, 0.40f, 1.0f); // rock
		palette.terrainColors[3] = vec4(0.75f, 0.75f, 0.65f, 1.0f); // light rock
		palette.terrainColors[4] = vec4(1.00f, 0.98f, 0.90f, 1.0f); // snow

		// Grass
		palette.grassColor = vec4(0.5f, 0.7f, 0.8f, 1.0f);

		// Water
		palette.waterColor = vec4(0.3, 0.64, 0.69, 1.0);

		// Fog
		palette.fogColor = CLEAR_COLOR;

		palette.angleThresholds = vec4(15.0, 40.0, 120.0, 240.0);

		palette.fogDensity = 0.000002;

		glBindBuffer(GL_UNIFORM_BUFFER, colorUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(ColorPaletteUBO), &palette);
	}

	ColorPaletteUBO getPalette() {
		return palette;
	}
};