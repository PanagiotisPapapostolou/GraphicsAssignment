#ifndef SHADER_HEADER
#define SHADER_HEADER

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>

#include <glad/glad.h> // include glad to get all the required OpenGL headers
#include <glm/glm.hpp>

#define BUFFER_SIZE 512

/* Shader Class */
class Shader {
public:
	unsigned int shaderProgramID;

	Shader(const char* vertexPath, const char* fragmentPath); // Constructor reads and builds the shader
	
	void use() { glUseProgram(this->shaderProgramID); } // Use/Activate the shader
	
	void setBool(const std::string& name, bool balue) const;
	void setInt(const std::string& name, int value) const;
	void setFloat(const std::string& name, float) const;

	void setVec2(const std::string& name, const glm::vec2& value) const;
	void setVec2(const std::string& name, float x, float y) const;

	void setVec3(const std::string& name, const glm::vec3& value) const;
	void setVec3(const std::string& name, float x, float y, float z) const;

	void setVec4(const std::string& name, const glm::vec4& value) const;
	void setVec4(const std::string& name, float x, float y, float z, float w) const;

	void setMat2(const std::string& name, const glm::mat2& mat) const;
	void setMat3(const std::string& name, const glm::mat3& mat) const;
	void setMat4(const std::string& name, const glm::mat4& mat) const;
};

Shader::Shader(const char* vertexPath, const char* fragmentPath)
{
	/* 1. Retrive the Vertex and Fragment shaders source code from filepath */
	std::string vertexCode; std::string fragmentCode;
	std::ifstream vShaderFile; std::ifstream fShaderFile;

	// Ensure ifstreams objects can throw exceptions
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try {
		// Open the files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);

		// Read file's buffer vontents into streams
		std::stringstream vShaderStream, fShaderStream;
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();

		// Close file handlers
		vShaderFile.close(); 
		fShaderFile.close();

		// Convert stream into string
		vertexCode = vShaderStream.str(); 
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure e) {
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ" << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str(); 
	const char* fShaderCode = fragmentCode.c_str();

	/* 2. Compile the shaders */
	unsigned int vertex, fragment;
	int success;
	char infoLog[BUFFER_SIZE];

	// Attach the vertex shader source code to the actual vertex shader, and compile it
	// --------------------------------------------------------------------------------------------- //
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);

	// Retrive any error (if existed) during the shader compilation and print it
	glGetShaderiv(vertex, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertex, BUFFER_SIZE, NULL, infoLog);
		std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Attach the fragment shader source code to the actual vertex shader, and compile it
	// --------------------------------------------------------------------------------------------- //
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);

	// Retrive any error (if existed) during the shader compilation and print it
	glGetShaderiv(fragment, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragment, BUFFER_SIZE, NULL, infoLog);
		std::cout << "ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n" << infoLog << std::endl;
	}

	// Initialize the shader program, attach our shaders to it, and link it
	// --------------------------------------------------------------------------------------------- //
	this->shaderProgramID = glCreateProgram();

	glAttachShader(this->shaderProgramID, vertex); 
	glAttachShader(this->shaderProgramID, fragment);
	glLinkProgram(this->shaderProgramID);

	// Retrive any error (if existed) during the program linking and print it
	glGetProgramiv(this->shaderProgramID, GL_LINK_STATUS, &success);
	if (!success) {
		glGetProgramInfoLog(this->shaderProgramID, BUFFER_SIZE, NULL, infoLog);
		std::cout << "ERROR::SHADER::PROGRAM::LINKING_FAILED\n" << infoLog << std::endl;
	}

	// Delete the shaders as they are linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}

void Shader::setBool(const std::string& name, bool value) const {
	glUniform1i(glGetUniformLocation(this->shaderProgramID, name.c_str()), (int)value);
}

void Shader::setInt(const std::string& name, int value) const {
	glUniform1i(glGetUniformLocation(this->shaderProgramID, name.c_str()), value);
}

void Shader::setFloat(const std::string& name, float value) const {
	glUniform1f(glGetUniformLocation(this->shaderProgramID, name.c_str()), value);
}

void Shader::setVec2(const std::string& name, const glm::vec2& value) const {
	glUniform2fv(glGetUniformLocation(this->shaderProgramID, name.c_str()), 1, &value[0]);
}
void Shader::setVec2(const std::string& name, float x, float y) const {
	glUniform2f(glGetUniformLocation(this->shaderProgramID, name.c_str()), x, y);
}

void Shader::setVec3(const std::string& name, const glm::vec3& value) const {
	glUniform3fv(glGetUniformLocation(this->shaderProgramID, name.c_str()), 1, &value[0]);
}
void Shader::setVec3(const std::string& name, float x, float y, float z) const {
	glUniform3f(glGetUniformLocation(this->shaderProgramID, name.c_str()), x, y, z);
}

void Shader::setVec4(const std::string& name, const glm::vec4& value) const {
	glUniform4fv(glGetUniformLocation(this->shaderProgramID, name.c_str()), 1, &value[0]);
}
void Shader::setVec4(const std::string& name, float x, float y, float z, float w) const {
	glUniform4f(glGetUniformLocation(this->shaderProgramID, name.c_str()), x, y, z, w);
}

void Shader::setMat2(const std::string& name, const glm::mat2& mat) const {
	glUniformMatrix2fv(glGetUniformLocation(this->shaderProgramID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat3(const std::string& name, const glm::mat3& mat) const {
	glUniformMatrix3fv(glGetUniformLocation(this->shaderProgramID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setMat4(const std::string& name, const glm::mat4& mat) const {
	glUniformMatrix4fv(glGetUniformLocation(this->shaderProgramID, name.c_str()), 1, GL_FALSE, &mat[0][0]);
}

#endif /* SHADER_HEADER */
