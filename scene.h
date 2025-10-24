#pragma once
#include "framework.h"
#include "camera.h"
#include "renderstate.h"
#include "light.h"
#include "chunkmanager.h"
#include "colorpalette.h"
#include "SkyDome.h"
#include "Player.h"
#include "material.h"
#include "trunkshader.h"
#include "leafshader.h"
#include "InstanceShader.h"
#include "PostProcessShader.h"
#include "watershader.h"
#include "terrainshader.h"
#include "plane.h"
#include "cactus.h"
#include "trunk.h"
#include "leaves.h"
#include "SharedResources.h"
#include "WorldConfig.h"
#include "FPSCounter.h"
#include "ShadowMap.h"

enum class ControlMode { Freecam, Player };

class Scene {
	RenderState state;
	FPSCounter fpsCounter;
	WorldConfig cfg;
	TerrainData terrainData;
	ChunkManager* chunkManager;
	Camera* camera;
	Player* player;
	ControlMode controlMode = ControlMode::Freecam;
	
	Light sun;
	SkyDome* skyDome;
	ColorPalette* palette;
	MaterialUBO materialsUBO;
	SharedResources resources;
	
	PostProcessShader* postShader;
	GLuint sceneFBO = 0;
	GLuint sceneColor = 0;
	GLuint sceneDepth = 0;
	GLuint fsVAO = 0, fsVBO = 0;

	ShadowMap shadow;
	mat4 lightV, lightP, lightVP;

	GLuint prevSceneColor = 0;   // reflection source
	GLuint sceneDepthCopy = 0;   // depth readback for SSR

	void updateState(RenderState& state) {
		state.M = mat4();
		state.V = camera->V();
		state.P = camera->P();
		state.invP = Inverse(state.P);

		state.cameraPos = camera->getPos();
		state.cameraDir = camera->getDir();
		state.time = glfwGetTime();
	}

	void createFullscreenTriangle() {
		if (fsVAO) return;
		// Fullscreen triangle
		const float verts[] = {
			-1.0f, -1.0f,   0.0f, 0.0f,
			 3.0f, -1.0f,   2.0f, 0.0f,
			-1.0f,  3.0f,   0.0f, 2.0f
		};
		glGenVertexArrays(1, &fsVAO);
		glGenBuffers(1, &fsVBO);
		glBindVertexArray(fsVAO);
		glBindBuffer(GL_ARRAY_BUFFER, fsVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
		glEnableVertexAttribArray(0); // position
		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(1); // uv
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
		glBindVertexArray(0);
	}

	void destroyRenderTarget() {
		if (sceneDepth) { glDeleteTextures(1, &sceneDepth); sceneDepth = 0; }
		if (sceneColor) { glDeleteTextures(1, &sceneColor); sceneColor = 0; }
		if (prevSceneColor) { glDeleteTextures(1, &prevSceneColor); prevSceneColor = 0; }
		if (sceneDepthCopy) { glDeleteTextures(1, &sceneDepthCopy); sceneDepthCopy = 0; }
		if (sceneFBO) { glDeleteFramebuffers(1, &sceneFBO); sceneFBO = 0; }
	}

	void createRenderTarget(int w, int h) {
		destroyRenderTarget();
		glGenFramebuffers(1, &sceneFBO);
		glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);

		// Color
		glGenTextures(1, &sceneColor);
		glBindTexture(GL_TEXTURE_2D, sceneColor);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, sceneColor, 0);

		// Depth
		glGenTextures(1, &sceneDepth);
		glBindTexture(GL_TEXTURE_2D, sceneDepth);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, sceneDepth, 0);

		// Previous frame for water reflection
		glGenTextures(1, &prevSceneColor);
		glBindTexture(GL_TEXTURE_2D, prevSceneColor);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		// Depth copy for water reflection
		glGenTextures(1, &sceneDepthCopy);
		glBindTexture(GL_TEXTURE_2D, sceneDepthCopy);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, w, h, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		GLenum drawBuffer = GL_COLOR_ATTACHMENT0;
		glDrawBuffers(1, &drawBuffer);

		if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) printf("Scene FBO error!\n");
	}

	void updateLightMatrices() {
		// Focus the shadow box around the camera
		vec3 camPos = camera->getPos();
		vec3 center = vec3(floor(camPos.x / cfg.chunkSize), 0.0f, floor(camPos.z / cfg.chunkSize)) * cfg.chunkSize;
		vec3 lightDir = -vec3(sun.data.dir.x, sun.data.dir.y, sun.data.dir.z);
		vec3 lightPos = center - lightDir * 1200.0f;     // pull back enough to see the box
		float halfSize = 2000.0f;     // covers this much around camera
		float halfHeight = 2000.0f;
		float nearPlane = 0.1f;
		float farPlane = 2000.0f;

		// Build matrices
		lightV = LookAt(lightPos, center, vec3(0.0f, 1.0f, 0.0f));
		lightP = Ortho(-halfSize, halfSize, -halfHeight, halfHeight, nearPlane, farPlane);
		lightVP = lightP * lightV;
	}

public:

	void Render() {
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		// FPS and state
		fpsCounter.update();
		float deltaTime = fpsCounter.getDeltaTime();
		updateState(state);

		// Physics + movement
		chunkManager->Update(camera->getPos());
		switch (controlMode) {
			case ControlMode::Freecam: camera->move(deltaTime); break;
			case ControlMode::Player:  player->Update(deltaTime); break;
		}
		
		if (fpsCounter.getFrameCount() % 10 == 0) {
			// Shadow pass
			glViewport(0, 0, shadow.width, shadow.height);
			glBindFramebuffer(GL_FRAMEBUFFER, shadow.fbo);
			glClear(GL_DEPTH_BUFFER_BIT);
			glEnable(GL_POLYGON_OFFSET_FILL);
			glPolygonOffset(10.0f, 1.0f); // removes shadow acne

			// Render with light matrices, save camera matrices
			mat4 prevV = state.V, prevP = state.P;

			// Shadow uniforms
			updateLightMatrices();
			state.V = lightV;
			state.P = lightP;
			state.lightVP = lightVP;
			state.shadowTexel = vec2(1.0f / shadow.width, 1.0f / shadow.height);
			state.shadowBias = 0.0025f;
			state.shadowTex = shadow.depthTex;

			// Draw shadow casters (skip skydome)
			chunkManager->DrawChunks(state, *camera);
			if (controlMode == ControlMode::Player) player->Draw(state);

			// Restore state
			glDisable(GL_POLYGON_OFFSET_FILL);
			state.V = prevV;
			state.P = prevP;
		}

		// Render scene to off-screen FBO
		glBindFramebuffer(GL_FRAMEBUFFER, sceneFBO);
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Draw calls (normal camera)
		skyDome->Draw(state);
		chunkManager->DrawChunks(state, *camera);
		if (controlMode == ControlMode::Player) player->Draw(state);

		// Copy depth for SSR so water can safely sample it
		glCopyImageSubData(
			sceneDepth, GL_TEXTURE_2D, 0, 0, 0, 0,
			sceneDepthCopy, GL_TEXTURE_2D, 0, 0, 0, 0,
			WINDOW_WIDTH, WINDOW_HEIGHT, 1
		);

		// Draw water and set SSR textures
		state.sceneDepthCopy = sceneDepthCopy;
		state.sceneColorPrev = prevSceneColor;
		chunkManager->DrawWater(state);

		// Post-process
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		postShader->Bind(state);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, sceneColor);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, sceneDepth);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, shadow.depthTex);
		glBindVertexArray(fsVAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		// Update prevSceneColor for next frame
		glCopyImageSubData(
			sceneColor, GL_TEXTURE_2D, 0, 0, 0, 0,
			prevSceneColor, GL_TEXTURE_2D, 0, 0, 0, 0,
			WINDOW_WIDTH, WINDOW_HEIGHT, 1
		);

		// Draw GUI
		drawGUI(WINDOW_WIDTH - GUI_WIDTH, 0, GUI_WIDTH, GUI_HEIGHT);
	}

	void Build() {
		postShader = new PostProcessShader();
		createFullscreenTriangle();
		createRenderTarget(WINDOW_WIDTH, WINDOW_HEIGHT);

		shadow.init();

		// Color palette
		palette = new ColorPalette();

		// Materials
		Material terrainMat{ vec4(0.5f,0.5f,0.5f,0.0f), vec4(0.4f,0.4f,0.4f,0.0f), vec4(0.4f,0.4f,0.4f,0.0f), vec4(1.0f) };
		Material grassMat{ vec4(0.5f,0.5f,0.5f,0.0f), vec4(0.4f,0.4f,0.4f,0.0f), vec4(0.4f,0.4f,0.4f,0.0f), vec4(8.0f) };
		Material waterMat{ vec4(0.5f,0.5f,0.5f,0.0f), vec4(0.8f,0.8f,0.8f,0.0f), vec4(0.4f,0.4f,0.4f,0.0f), vec4(100.0f) };
		Material objMat{ vec4(0.5f,0.5f,0.5f,0.0f), vec4(0.4f,0.4f,0.4f,0.0f), vec4(0.4f,0.4f,0.4f,0.0f), vec4(1.0f) };

		materialsUBO.init();
		materialsUBO.set(0, terrainMat);
		materialsUBO.set(1, grassMat);
		materialsUBO.set(2, waterMat);
		materialsUBO.set(3, objMat);

		// Sun
		sun.data.dir = vec4(normalize(vec3(0.5f, 0.6f, -0.2f)), 0.0f);
		sun.data.la = vec4(0.8f, 0.8f, 0.8f, 0.0f);
		sun.data.le = vec4(0.6f, 0.6f, 0.6f, 0.0f);
		sun.InitUBO();

		// Terrain Data
		terrainData.bedrockFrequency = 0.003f;
		terrainData.bedrockAmplitude = 16.0f;
		terrainData.frequency = 0.0012f;
		terrainData.frequencyMultiplier = 2.0f;
		terrainData.amplitude = 200.0f;
		terrainData.amplitudeMultiplier = 0.45f;
		terrainData.floorLevel = 20.0f;
		terrainData.waterLevel = 16.0f;
		terrainData.blendFactor = 32.0f;
		terrainData.warpFreq = 0.001f;
		terrainData.warpAmp = 12.0f;
		terrainData.warpStrength = 30.0f;
		terrainData.warpFreqMult = 2.0f;
		terrainData.warpAmpMult = 0.5f;
		terrainData.warpOctaves = 6;
		terrainData.seed = 310;

		// World Config
		cfg.chunkSize = 256.0f;
		cfg.renderDist = 8;
		cfg.tesselation = 32;
		cfg.terrain = terrainData;

		// Shared Shaders
		resources.terrainShader		= new TerrainShader();
		resources.waterShader		= new WaterShader();
		resources.instanceShader	= new InstanceShader();
		resources.treeTrunkShader	= new TrunkShader();
		resources.treeLeafShader	= new LeafShader();
		resources.marchingCubesCS	= new MarchingCubesCS();
		resources.groundDistanceCS	= new GroundDistanceCS();
		resources.terrainHeightCS	= new TerrainHeightCS();

		// Shared Geometries
		resources.waterGeom		= new PlaneGeometry(cfg.chunkSize * (2 * cfg.renderDist + 1), cfg.tesselation * (2 * cfg.renderDist + 1));

		for (int i = 0; i < 10; i++) {
			TreeParams treeParams(i);
			resources.treeTrunkGeoms.push_back(new TrunkGeometry(128.0f, 32, treeParams));
			resources.treeCrownGeoms.push_back(new LeavesGeometry(200.0f, 64, 3600, treeParams));
		}

		skyDome = new SkyDome();
		chunkManager = new ChunkManager(&cfg, &resources);
		camera = new Camera();
		camera->setPos(chunkManager->getSpawnPoint());
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
		ImGui::Text("FPS: %d, AVG: %.1f", fpsCounter.getFPS(), fpsCounter.getAverageFPS());
		ImGui::Text("X: %.1f, Y: %.1f, Z: %.1f", camera->getPos().x, camera->getPos().y, camera->getPos().z);

		// Color Palette
		if (palette) {
			if (palette->DrawImGui("Colors")) {}
		}

		// Terrain Params
		ImGui::SeparatorText("Terrain Params");
		ImGui::SliderFloat("Bedrock Frequency", &terrainData.bedrockFrequency, 0.001f, 0.1f);
		ImGui::SliderFloat("Bedrock Amplitude", &terrainData.bedrockAmplitude, 0.01f, 50.0f);
		ImGui::SliderFloat("Frequency", &terrainData.frequency, 0.0001f, 0.005f, "%.4f");
		ImGui::SliderFloat("Frequency Multiplier", &terrainData.frequencyMultiplier, 1.0f, 3.0f);
		ImGui::SliderFloat("Amplitude", &terrainData.amplitude, 0.01f, 500.0f);
		ImGui::SliderFloat("Amplitude Multiplier", &terrainData.amplitudeMultiplier, 0.1f, 1.0f);
		ImGui::SliderFloat("Floor Level", &terrainData.floorLevel, -50.0f, 50.0f);
		ImGui::SliderFloat("Blend Factor", &terrainData.blendFactor, 0.0f, 100.0f);
		ImGui::SliderFloat("Water Level", &terrainData.waterLevel, 0.0f, 50.0f);

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
				camera->setPos(chunkManager->getSpawnPoint());
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

	~Scene() {
		if (player) { delete player; player = nullptr; }
		if (chunkManager) { delete chunkManager; chunkManager = nullptr; }

		if (skyDome) { delete skyDome; skyDome = nullptr; }
		if (camera) { delete camera; camera = nullptr; }
		if (palette) { delete palette; palette = nullptr; }

		if (resources.waterGeom) { delete resources.waterGeom; resources.waterGeom = nullptr; }
		if (resources.cactusGeom) { delete resources.cactusGeom; resources.cactusGeom = nullptr; }

		if (resources.terrainShader) { delete resources.terrainShader; resources.terrainShader = nullptr; }
		if (resources.waterShader) { delete resources.waterShader; resources.waterShader = nullptr; }
		if (resources.instanceShader) { delete resources.instanceShader; resources.instanceShader = nullptr; }

		if (resources.marchingCubesCS) { delete resources.marchingCubesCS; resources.marchingCubesCS = nullptr; }
		if (resources.groundDistanceCS) { delete resources.groundDistanceCS; resources.groundDistanceCS = nullptr; }
		if (resources.terrainHeightCS) { delete resources.terrainHeightCS; resources.terrainHeightCS = nullptr; }
	}
};