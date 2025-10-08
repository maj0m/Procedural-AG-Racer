#pragma once
#include "framework.h"
#include "renderstate.h"
#include "terraindata.h"
#include <string>
#include <fstream>
#include <sstream>

class Shader {
	unsigned int shaderProgramId = 0;
	unsigned int vertexShader = 0, geometryShader = 0, fragmentShader = 0;

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

	// get the address of a GPU uniform variable
	int getLocation(const std::string& name) {
		int location = glGetUniformLocation(shaderProgramId, name.c_str());
		if (location < 0) printf("uniform %s cannot be set\n", name.c_str());
		return location;
	}

public:
	Shader() { shaderProgramId = 0; }

	virtual void Bind(RenderState state) = 0;

	unsigned int getId() { return shaderProgramId; }


	bool create(const std::string& vertexShaderFilePath,
		const std::string& fragmentShaderFilePath,
		const std::string& fragmentShaderOutputName,
		const std::string& geometryShaderFilePath = "") {

		// Load and compile vertex shader
		std::string vertexShaderCode = readShaderCodeFromFile(vertexShaderFilePath);
		const char* vertexSource = vertexShaderCode.c_str();
		vertexShader = glCreateShader(GL_VERTEX_SHADER);
		glShaderSource(vertexShader, 1, &vertexSource, NULL);
		glCompileShader(vertexShader);

		// Load and compile geometry shader if provided
		if (!geometryShaderFilePath.empty()) {
			std::string geometryShaderCode = readShaderCodeFromFile(geometryShaderFilePath);
			const char* geometrySource = geometryShaderCode.c_str();
			geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
			glShaderSource(geometryShader, 1, &geometrySource, NULL);
			glCompileShader(geometryShader);
		}

		// Load and compile fragment shader
		std::string fragmentShaderCode = readShaderCodeFromFile(fragmentShaderFilePath);
		const char* fragmentSource = fragmentShaderCode.c_str();
		fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
		glShaderSource(fragmentShader, 1, &fragmentSource, NULL);
		glCompileShader(fragmentShader);

		// Create shader program and attach shaders
		shaderProgramId = glCreateProgram();
		glAttachShader(shaderProgramId, vertexShader);
		if (!geometryShaderFilePath.empty()) {
			glAttachShader(shaderProgramId, geometryShader);
		}
		glAttachShader(shaderProgramId, fragmentShader);

		// Link shader program
		glBindFragDataLocation(shaderProgramId, 0, fragmentShaderOutputName.c_str());
		glLinkProgram(shaderProgramId);

		return true;
	}


	void Use() {
		glUseProgram(shaderProgramId);	// make this program run
	}

	void setUniform(int i, const std::string& name) {
		int location = getLocation(name);
		if (location >= 0) glUniform1i(location, i);
	}

	void setUniform(float f, const std::string& name) {
		int location = getLocation(name);
		if (location >= 0) glUniform1f(location, f);
	}

	void setUniform(const vec2& v, const std::string& name) {
		int location = getLocation(name);
		if (location >= 0) glUniform2fv(location, 1, &v.x); 
	}

	void setUniform(const vec3& v, const std::string& name) {
		int location = getLocation(name);
		if (location >= 0) glUniform3fv(location, 1, &v.x);
	}

	void setUniform(const vec4& v, const std::string& name) {
		int location = getLocation(name);
		if (location >= 0) glUniform4fv(location, 1, &v.x);
	}

	void setUniform(const mat4& mat, const std::string& name) {
		int location = getLocation(name);
		if (location >= 0) glUniformMatrix4fv(location, 1, GL_FALSE, mat);
	}

	void setUniformMaterial(const Material& material, const std::string& name) {
		setUniform(material.kd, name + ".kd");
		setUniform(material.ks, name + ".ks");
		setUniform(material.ka, name + ".ka");
		setUniform(material.shininess, name + ".shininess");
	}

	void setUniformLight(const Light& L, const std::string& name) {
		setUniform(L.data.la, name + ".La");
		setUniform(L.data.le, name + ".Le");
		setUniform(L.data.dir, name + ".dir");
	}

	~Shader() { if (shaderProgramId > 0) glDeleteProgram(shaderProgramId); }
};
