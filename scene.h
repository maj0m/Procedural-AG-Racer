#pragma once
#pragma once
#include "framework.h"
#include "camera.h"
#include "renderstate.h"
#include "light.h"
#include "chunkmanager.h"
#include <iostream>
#include "colorpalette.h"

class Scene {
	float lastFrameTime = 0.0;
	RenderState state;
	TerrainData terrainData;
	ChunkManager* chunkManager;
	Camera* camera;
	ColorPalette* palette;

	void updateState(RenderState& state) {
		state.time = getTime();

		state.M = mat4(	1, 0, 0, 0,
						0, 1, 0, 0,
						0, 0, 1, 0,
						0, 0, 0, 1);
		state.V = camera->V();
		state.P = camera->P();

		state.wEye = camera->getEyePos();
		state.wFront = camera->getEyeDir();
		state.wUp = camera->getEyeUp();
		state.time = glfwGetTime();
	}

public:

	void Render() {
		float currentTime = static_cast<float>(glfwGetTime());
		if (lastFrameTime == 0.0) {lastFrameTime = currentTime;	}
		float deltaTime = currentTime - lastFrameTime;
		lastFrameTime = currentTime;

		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		updateState(state);
		drawGUI(WINDOW_WIDTH - GUI_WIDTH, 0, GUI_WIDTH, GUI_HEIGHT);

		// Physics Step
		camera->move();
		chunkManager->Update(camera->getEyePos());

		// Draw calls
		chunkManager->DrawChunks(state, *camera);
	}


	void Build() {
		// Color palette
		palette = new ColorPalette();

		// Sun
		Light sun;
		sun.dir = normalize(vec3(0.3f, 1.0f, -0.2f));
		sun.Le = vec3(0.6, 0.6, 0.6);
		sun.La = vec3(0.2, 0.2, 0.2);
		state.light = sun;

		// Terrain Data
		terrainData.bedrockFrequency = 0.003f;
		terrainData.bedrockAmplitude = 20.0f;
		terrainData.frequency = 0.0014f;
		terrainData.frequencyMultiplier = 2.0f;
		terrainData.amplitude = 200.0f;
		terrainData.amplitudeMultiplier = 0.45f;
		terrainData.floorLevel = 10.0f;
		terrainData.blendFactor = 32.0f;
		terrainData.warpFreq = 0.001;
		terrainData.warpAmp = 12.0;
		terrainData.warpStrength = 30.0;
		terrainData.warpFreqMult = 2.0;
		terrainData.warpAmpMult = 0.5;
		terrainData.warpOctaves = 6;

		chunkManager = new ChunkManager(200.0f, 5, terrainData);
		camera = new Camera();
		camera->setEyePos(chunkManager->getSpawnPoint() + vec3(0.0, 20.0, 0.0));
	}



	void drawGUI(int x, int y, int w, int h) {
		ImGui::SetNextWindowPos(ImVec2(x, y));
		ImGui::SetNextWindowSize(ImVec2(w, h));
		ImGui::Begin("Settings");

		// FPS and Coordinates
		getFPS(fps);
		ImGui::Text("FPS: %d", fps);
		ImGui::Text("X: %.1f, Y: %.1f, Z: %.1f", camera->getEyePos().x, camera->getEyePos().y, camera->getEyePos().z);

		// Color Palette
		if (palette) {
			if (palette->DrawImGui("Colors")) {}
		}

		// Terrain Params
		ImGui::SeparatorText("Terrain Params");
		ImGui::SliderFloat("Bedrock Frequency", &terrainData.bedrockFrequency, 0.001f, 0.1f);
		ImGui::SliderFloat("Bedrock Amplitude", &terrainData.bedrockAmplitude, 0.01f, 50.0f);
		ImGui::SliderFloat("Frequency", &terrainData.frequency, 0.001f, 0.01f, "%.4f");
		ImGui::SliderFloat("Frequency Multiplier", &terrainData.frequencyMultiplier, 1.0f, 3.0f);
		ImGui::SliderFloat("Amplitude", &terrainData.amplitude, 0.01f, 500.0f);
		ImGui::SliderFloat("Amplitude Multiplier", &terrainData.amplitudeMultiplier, 0.1f, 1.0f);
		ImGui::SliderFloat("Floor Level", &terrainData.floorLevel, -50.0f, 50.0f);
		ImGui::SliderFloat("Blend Factor", &terrainData.blendFactor, 0.0f, 100.0f);

		ImGui::SeparatorText("Warp");
		ImGui::SliderFloat("Warp Frequency", &terrainData.warpFreq, 0.0001f, 0.004f, "%.4f");
		ImGui::SliderFloat("Warp Amplitude", &terrainData.warpAmp, 0.0f, 100.0f);
		ImGui::SliderFloat("Warp Strength", &terrainData.warpStrength, 0.0f, 100.0f);
		ImGui::SliderFloat("Warp Freq. Mul.", &terrainData.warpFreqMult, 0.0f, 1.0f);
		ImGui::SliderFloat("Warp Ampl. Mul.", &terrainData.warpAmpMult, 0.0f, 2.0f);
		ImGui::SliderInt("Warp Octaves", &terrainData.warpOctaves, 1, 8);
		
		ImGui::SeparatorText("Seed");
		ImGui::SliderInt("Seed", &terrainData.seed, 1, 500);

		if (ImGui::Button("Reload Chunks")) {
			chunkManager->setTerrainData(terrainData);
			chunkManager->ReloadChunks();
		}

		ImGui::End();
	}

	void rotateCamera(float x, float y) {
		if (x < WINDOW_WIDTH - GUI_WIDTH) camera->rotate(x, y);
	}

	void setCameraFirstMouse() {
		camera->setFirstMouse();
	}
};


