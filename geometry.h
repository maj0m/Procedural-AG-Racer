#pragma once
#include "framework.h"


class Geometry {
protected:
	unsigned int vao = 0, vbo = 0;
	int vertexCount = 0;

public:
	Geometry() {}

	void init(const std::vector<vec3> vtxData) {
		vertexCount = vtxData.size();

		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);

		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, vtxData.size() * sizeof(vec3), vtxData.data(), GL_STATIC_DRAW);
		glEnableVertexAttribArray(0); // AttArr 0 = POSITION
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(vec3), (void*)0);
	}

	void Draw() {
		glBindVertexArray(vao);
		glDrawArrays(GL_TRIANGLES, 0, vertexCount);
	}

	virtual ~Geometry() {
		glDeleteBuffers(1, &vbo);
		glDeleteVertexArrays(1, &vao);
	}

	// Getters
	GLuint getVAO() const { return vao; }
	GLuint getVBO()  const { return vbo; }
	GLsizei getVertexCount() const { return vertexCount; }
};
