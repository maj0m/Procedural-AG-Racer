#pragma once
#include "framework.h"
#include "ParticleShader.h"
#include "ParticleUpdateCS.h"
#include "ParticleEmitCS.h"

struct Particle {
	vec4 pos;
	vec4 vel;
	vec4 col;
	vec4 ext; // x = age, y = lifetime, z = size, w = type
};

struct Emitter {
	vec4 pos_rad;
	vec4 dir_spread;
	vec4 life_size_rate_type;
	vec4 color;
};

class ParticleSystem {
	GLuint vao = 0;
	GLuint particleSSBO = 0;
	GLuint freeListSSBO = 0;
	GLuint emittersSSBO = 0;

	uint32_t count = 0;
	const int maxEmitters = 64;
	const int maxParticles = 65536;

	ParticleShader* particleShader;
	ParticleUpdateCS* particleUpdateCS;
	ParticleEmitCS* particleEmitCS;

	std::vector<Particle> particles;
	std::vector<Emitter> emitters;

public:
	ParticleSystem() {
		particleShader = new ParticleShader();
		particleUpdateCS = new ParticleUpdateCS();
		particleEmitCS = new ParticleEmitCS();

		// Init particles
		particles.reserve(maxParticles);
		for (int i = 0; i < maxParticles; ++i) {
			Particle p;
			particles.push_back(p);
		}
		count = particles.size();

		// VAO
		glGenVertexArrays(1, &vao);

		// Particle SSBO
		glGenBuffers(1, &particleSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, particleSSBO);
		glBufferData(GL_SHADER_STORAGE_BUFFER, particles.size() * sizeof(Particle), particles.data(), GL_DYNAMIC_DRAW);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleSSBO);

		// Freelist - start full of all indices
		glGenBuffers(1, &freeListSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, freeListSSBO);
		std::vector<uint32_t> freeIndices(count + 1);
		freeIndices[0] = count; // every particle is initially free
		for (uint32_t i = 0; i < count - 1; i++) freeIndices[i + 1] = i;
		glBufferData(GL_SHADER_STORAGE_BUFFER, freeIndices.size() * sizeof(uint32_t), freeIndices.data(), GL_DYNAMIC_DRAW);

		// Emitters
		glGenBuffers(1, &emittersSSBO);
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, emittersSSBO);
		size_t header = 16;
		glBufferData(GL_SHADER_STORAGE_BUFFER, header + maxEmitters * sizeof(Emitter), nullptr, GL_DYNAMIC_DRAW);
	}

	void AddEmitter(const vec3& center, const vec3& direction, float spreadAngle, float radius, float particleLife, float particleSize, float particleType, vec4 particleColor, float ratePerSec) {
		Emitter e;
		e.pos_rad = vec4(center, radius);
		e.dir_spread = vec4(direction, spreadAngle);
		e.life_size_rate_type = vec4(particleLife, particleSize, ratePerSec, particleType);
		e.color = particleColor;
		emitters.push_back(e);
	}

	void UploadEmitters() {
		glBindBuffer(GL_SHADER_STORAGE_BUFFER, emittersSSBO);
		const size_t header = 16;
		glBufferSubData(GL_SHADER_STORAGE_BUFFER, header, emitters.size() * sizeof(Emitter), emitters.data());
	}

	void Update(float time, float dt, const vec3& playerPos, const vec3& playerForward) {
		emitters.clear();
		particleUpdateCS->Dispatch(dt, count, particleSSBO, freeListSSBO);

		// Params
		vec3 up = vec3(0.0f, 1.0f, 0.0f);
		vec3 right = normalize(cross(up, normalize(playerForward)));
		vec3 heightOffset = vec3(0.0f, -1.0f, 0.0f);
		
		// Ambient
		AddEmitter(playerPos, up, M_PI, 500.0f, 20.0f, 0.4f, 0.0f, vec4(0.85f, 0.8f, 0.7f, 0.3f), 5000.0f);

		// Trail
		AddEmitter(playerPos + right + heightOffset, -playerForward, 0.0f, 0.0f, 10.0f, 0.5f, 1.0f, vec4(1.0f, 0.9f, 0.4f, 0.85f), 2000.0f);
		AddEmitter(playerPos - right + heightOffset, -playerForward, 0.0f, 0.0f, 10.0f, 0.5f, 1.0f, vec4(1.0f, 0.9f, 0.4f, 0.85f), 2000.0f);

		// Upload to SSBO
		UploadEmitters();

		// Dispatch
		particleEmitCS->Dispatch(time, dt, emitters.size(), particleSSBO, freeListSSBO, emittersSSBO);
	}
	
	void Draw(RenderState& state) {
		particleShader->Bind(state);

		glBindVertexArray(vao);
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, particleSSBO);
		glDrawArrays(GL_POINTS, 0, count);
	}

	~ParticleSystem() {
		if (emittersSSBO) glDeleteBuffers(1, &emittersSSBO);
		if (freeListSSBO) glDeleteBuffers(1, &freeListSSBO);
		if (particleSSBO) glDeleteBuffers(1, &particleSSBO);
		if (vao) glDeleteVertexArrays(1, &vao);
		delete particleEmitCS;
		delete particleUpdateCS;
		delete particleShader;
	}
};