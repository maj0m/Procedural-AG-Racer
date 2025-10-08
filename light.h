#pragma once
#include "framework.h"

struct LightData {
	vec4 dir;
	vec4 la;
	vec4 le;
};

struct Light {
	LightData data;
	GLuint lightingUBO = 0;

	void InitUBO() {
		glGenBuffers(1, &lightingUBO);
		glBindBuffer(GL_UNIFORM_BUFFER, lightingUBO);
		glBufferData(GL_UNIFORM_BUFFER, sizeof(LightData), &data, GL_DYNAMIC_DRAW);
		glBindBufferRange(GL_UNIFORM_BUFFER, 2, lightingUBO, 0, sizeof(LightData)); // Binding = 2
	}

	void UpdateUBO() const {
		glBindBuffer(GL_UNIFORM_BUFFER, lightingUBO);
		glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(LightData), &data);
	}
};
