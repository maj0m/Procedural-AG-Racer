#pragma once

#include "framework.h"
#include "camera.h"
#include "renderstate.h"
#include "light.h"
#include "chunkmanager.h"
#include <iostream>
#include "colorpalette.h"
#include "SkyDome.h"
#include "Player.h"

enum class ControlMode { Freecam, Player };

class Scene {
	float lastFrameTime = 0.0;
	RenderState state;
	TerrainData terrainData;
	ChunkManager* chunkManager;
	Camera* camera;
	ColorPalette* palette;
	SkyDome* skyDome;
	Player* player;
	ControlMode controlMode = ControlMode::Freecam;

	void updateState(RenderState& state) {
		state.time = getTime();

		state.M = mat4();
		state.V = camera->V();
		state.P = camera->P();

		state.wEye = camera->getEyePos();
		state.wFront = camera->getEyeDir();
		state.wUp = camera->getEyeUp();
		state.time = glfwGetTime();
	}

public:

	void Render() {
		float currentTime = glfwGetTime();
		if (lastFrameTime == 0.0f) lastFrameTime = currentTime;
		float deltaTime = currentTime - lastFrameTime;
		lastFrameTime = currentTime;

		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		// Update state
		updateState(state);

		// Physics Step
		chunkManager->Update(camera->getEyePos());
		switch (controlMode) {
			case ControlMode::Freecam:	camera->move(deltaTime);	break;
			case ControlMode::Player:	player->Update(deltaTime);	break;
		}
		
		// Draw calls
		skyDome->Draw(state);
		chunkManager->DrawChunks(state, *camera);
		if(controlMode == ControlMode::Player) player->Draw(state);
		drawGUI(WINDOW_WIDTH - GUI_WIDTH, 0, GUI_WIDTH, GUI_HEIGHT);
	}

	void Build() {
		// Color palette
		palette = new ColorPalette();

		// Sun
		Light sun;
		sun.data.dir = vec4(normalize(vec3(0.5f, 0.6f, -0.2f)), 0.0f);
		sun.data.la = vec4(0.8f, 0.8f, 0.8f, 0.0f);
		sun.data.le = vec4(0.6f, 0.6f, 0.6f, 0.0f);
		sun.InitUBO();

		// Terrain Data
		terrainData.bedrockFrequency = 0.003f;
		terrainData.bedrockAmplitude = 16.0f;
		terrainData.frequency = 0.0016f;
		terrainData.frequencyMultiplier = 2.0f;
		terrainData.amplitude = 180.0f;
		terrainData.amplitudeMultiplier = 0.45f;
		terrainData.floorLevel = 25.0f;
		terrainData.blendFactor = 24.0f;
		terrainData.warpFreq = 0.001f;
		terrainData.warpAmp = 12.0f;
		terrainData.warpStrength = 30.0f;
		terrainData.warpFreqMult = 2.0f;
		terrainData.warpAmpMult = 0.5f;
		terrainData.warpOctaves = 6;
		terrainData.seed = 310;
		terrainData.waterLevel = 16.0f;

		skyDome = new SkyDome();
		chunkManager = new ChunkManager(256.0f, 8, terrainData);
		camera = new Camera();
		camera->setEyePos(chunkManager->getSpawnPoint() + vec3(0.0, 20.0, 0.0));
		player = new Player(camera, chunkManager);
	}

	void drawGUI(int x, int y, int w, int h) {
		ImGui::SetNextWindowPos(ImVec2(x, y));
		ImGui::SetNextWindowSize(ImVec2(w, h));
		ImGui::Begin("Settings");

		ImGui::SeparatorText("Control");
		bool isFreecam = (controlMode == ControlMode::Freecam);
		if (ImGui::Button(isFreecam ? "Switch to Player" : "Switch to Freecam")) {
			controlMode = isFreecam ? ControlMode::Player : ControlMode::Freecam;
		}

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
		ImGui::SliderFloat("Water Level", &terrainData.waterLevel, 0.0f, 20.0f);

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

		if (ImGui::Button("Respawn")) {
			if (controlMode == ControlMode::Player) {
				player->Respawn();
			}
			else {
				camera->setEyePos(chunkManager->getSpawnPoint() + vec3(0.0f, 20.0f, 0.0f));
			}
		}

		ImGui::End();
	}

	void rotateCamera(float x, float y) {
		if (controlMode == ControlMode::Freecam && x < WINDOW_WIDTH - GUI_WIDTH) camera->rotate(x, y);
	}

	void setCameraFirstMouse() {
		if (controlMode == ControlMode::Freecam) camera->setFirstMouse();
	}
};