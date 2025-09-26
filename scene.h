#pragma once
#pragma once
#include "framework.h"
#include "camera.h"
#include "renderstate.h"
#include "light.h"
#include "chunkmanager.h"
#include <iostream>

class Scene {
	float lastFrameTime = 0.0;
	RenderState state;
	TerrainData terrainData;
	std::vector<Light> lights;
	ChunkManager* chunkManager;
	Camera* camera;

	void updateState(RenderState& state) {
		state.lights = lights;
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
		camera = new Camera();

		// Lights
		lights.resize(1);
		lights[0].wLightPos = vec4(0, 1000, 0, 1);
		lights[0].Le = vec3(0.6, 0.6, 0.6);
		lights[0].La = vec3(0.5, 0.5, 0.5);

		// Terrain Data
		terrainData.bedrockFrequency = 0.003f;
		terrainData.bedrockAmplitude = 20.0f;
		terrainData.frequency = 0.002f;
		terrainData.frequencyMultiplier = 2.0f;
		terrainData.amplitude = 130.0f;
		terrainData.amplitudeMultiplier = 0.45f;
		terrainData.floorLevel = 10.0f;
		terrainData.blendFactor = 25.0f;

		chunkManager = new ChunkManager(200.0f, 5, terrainData);
	}



	void drawGUI(int x, int y, int w, int h) {
		ImGui::SetNextWindowPos(ImVec2(x, y));
		ImGui::SetNextWindowSize(ImVec2(w, h));
		ImGui::Begin("Settings");

		// Display FPS and Coordinates
		getFPS(fps);
		ImGui::Text("FPS: %d", fps);
		ImGui::Text("X: %.1f, Y: %.1f, Z: %.1f", camera->getEyePos().x, camera->getEyePos().y, camera->getEyePos().z);
		
		// Bedrock Frequency
		ImGui::SliderFloat("Bedrock Frequency", &terrainData.bedrockFrequency, 0.001f, 0.1f);

		// Bedrock Amplitude
		ImGui::SliderFloat("Bedrock Amplitude", &terrainData.bedrockAmplitude, 0.01f, 50.0f);

		// Terrain Frequency
		ImGui::SliderFloat("Frequency", &terrainData.frequency, 0.001f, 0.1f);
		ImGui::SliderFloat("Frequency Multiplier", &terrainData.frequencyMultiplier, 1.0f, 3.0f);

		// Terrain Amplitude
		ImGui::SliderFloat("Amplitude", &terrainData.amplitude, 0.01f, 256.0f);
		ImGui::SliderFloat("Amplitude Multiplier", &terrainData.amplitudeMultiplier, 0.1f, 1.0f);

		// Terrain Floor Level
		ImGui::SliderFloat("Floor Level", &terrainData.floorLevel, -50.0f, 50.0f);

		// Terrain Blend Factor
		ImGui::SliderFloat("Blend Factor", &terrainData.blendFactor, 0.0f, 100.0f);

		if (ImGui::Button("Reload Chunks")) {
			chunkManager->setTerrainData(terrainData);
			chunkManager->ReloadChunks();
		}

		ImGui::End();
	}

	void rotateCamera(float x, float y) {
		camera->rotate(x, y);
	}

	void setCameraFirstMouse() {
		camera->setFirstMouse();
	}
};


