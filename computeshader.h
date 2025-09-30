#pragma once
#include "framework.h"
#include <string>
#include <fstream>
#include <sstream>
#include "tabletexture.h"

class ComputeShader {


	std::string readShaderCodeFromFile(const std::string& filePath) {
		std::ifstream shaderFile(filePath);
		std::stringstream shaderStream;

		// Check if file opening succeeded
		if (!shaderFile.is_open()) {
			printf("Failed to open shader file: %s\n", filePath.c_str());
			return "";
		}

		shaderStream << shaderFile.rdbuf();
		shaderFile.close();
		return shaderStream.str();
	}

public:
	unsigned int shaderProgramId = 0;

	ComputeShader() { shaderProgramId = 0; }


	unsigned int getId() { return shaderProgramId; }

	void create(const std::string& computeShaderFilePath) {
		std::string computeShaderCode = readShaderCodeFromFile(computeShaderFilePath);
		const char* computeSource = computeShaderCode.c_str();
		GLuint computeShader = glCreateShader(GL_COMPUTE_SHADER);
		glShaderSource(computeShader, 1, &computeSource, NULL);
		glCompileShader(computeShader);
		shaderProgramId = glCreateProgram();
		glAttachShader(shaderProgramId, computeShader);
		glLinkProgram(shaderProgramId);
	}

	void setUniform(int i, const std::string& name) {
		int location = glGetUniformLocation(shaderProgramId, name.c_str());
		if (location >= 0) glUniform1i(location, i);
	}

	void setUniform(float f, const std::string& name) {
		int location = glGetUniformLocation(shaderProgramId, name.c_str());
		if (location >= 0) glUniform1f(location, f);
	}

	void setUniform(const vec2& v, const std::string& name) {
		int location = glGetUniformLocation(shaderProgramId, name.c_str());
		if (location >= 0) glUniform2fv(location, 1, &v.x);
	}

	void setUniform(const vec3& v, const std::string& name) {
		int location = glGetUniformLocation(shaderProgramId, name.c_str());
		if (location >= 0) glUniform3fv(location, 1, &v.x);
	}

	void setUniform(const vec4& v, const std::string& name) {
		int location = glGetUniformLocation(shaderProgramId, name.c_str());
		if (location >= 0) glUniform4fv(location, 1, &v.x);
	}

	void setUniform(const mat4& mat, const std::string& name) {
		int location = glGetUniformLocation(shaderProgramId, name.c_str());
		if (location >= 0) glUniformMatrix4fv(location, 1, GL_FALSE, mat);
	}

	void setUniform(const EdgeTableTexture& texture, const std::string& name, unsigned int textureUnit = 0) {
		int location = glGetUniformLocation(shaderProgramId, name.c_str());
		if (location >= 0) {
			glUniform1i(location, textureUnit);
			glActiveTexture(GL_TEXTURE0 + textureUnit);
			glBindTexture(GL_TEXTURE_2D, texture.textureId);
		}
	}

	~ComputeShader() { if (shaderProgramId > 0) glDeleteProgram(shaderProgramId); }
};
