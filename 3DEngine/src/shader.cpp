#include "shader.h"

#include <fstream>
#include <glad/glad.h>
#include <iostream>
#include <sstream>
#include <string>

shader::shader() { }

shader::shader(const char* vertexPath, const char* fragmentPath)
{
	// 1. retrieve the vertex/fragment source code from filePath
	std::string vertexCode;
	std::string fragmentCode;
	std::ifstream vShaderFile;
	std::ifstream fShaderFile;
	// ensure ifstream objects can throw exceptions:
	vShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	fShaderFile.exceptions(std::ifstream::failbit | std::ifstream::badbit);
	try
	{
		// open files
		vShaderFile.open(vertexPath);
		fShaderFile.open(fragmentPath);
		std::stringstream vShaderStream, fShaderStream;
		// read file's buffer contents into streams
		vShaderStream << vShaderFile.rdbuf();
		fShaderStream << fShaderFile.rdbuf();
		// close file handlers
		vShaderFile.close();
		fShaderFile.close();
		// convert stream into string
		vertexCode = vShaderStream.str();
		fragmentCode = fShaderStream.str();
	}
	catch (std::ifstream::failure& e)
	{
		std::cout << "ERROR::SHADER::FILE_NOT_SUCCESSFULLY_READ: " << e.what() << std::endl;
	}
	const char* vShaderCode = vertexCode.c_str();
	const char* fShaderCode = fragmentCode.c_str();
	// 2. compile shaders
	unsigned int vertex, fragment;
	// vertex shader
	vertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(vertex, 1, &vShaderCode, NULL);
	glCompileShader(vertex);
	CheckCompileErrors(vertex, "VERTEX");
	// fragment shader
	fragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(fragment, 1, &fShaderCode, NULL);
	glCompileShader(fragment);
	CheckCompileErrors(fragment, "FRAGMENT");
	// shader Program
	ID = glCreateProgram();
	glAttachShader(ID, vertex);
	glAttachShader(ID, fragment);
	glLinkProgram(ID);
	CheckCompileErrors(ID, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(vertex);
	glDeleteShader(fragment);
}
// activate the shader
// ------------------------------------------------------------------------
shader& shader::Use()
{
	glUseProgram(ID);
	return *this;
}
void shader::Compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource, const char* tessalationControlSource, const char* tessalationEvaluationSource)
{
	unsigned int sVertex, sFragment, gShader, sTessalationControl, sTessalationEvaluation;
	// vertex shader
	sVertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(sVertex, 1, &vertexSource, NULL);
	glCompileShader(sVertex);
	CheckCompileErrors(sVertex, "VERTEX");
	// fragment shader
	sFragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(sFragment, 1, &fragmentSource, NULL);
	glCompileShader(sFragment);
	CheckCompileErrors(sFragment, "FRAGMENT");
	// if geometry shader source code is given, also compile geometry shader
	if (geometrySource != nullptr)
	{
		gShader = glCreateShader(GL_GEOMETRY_SHADER);
		glShaderSource(gShader, 1, &geometrySource, NULL);
		glCompileShader(gShader);
		CheckCompileErrors(gShader, "GEOMETRY");
	}
	if (tessalationControlSource != nullptr)
	{
		sTessalationControl = glCreateShader(GL_TESS_CONTROL_SHADER);
		glShaderSource(sTessalationControl, 1, &tessalationControlSource, NULL);
		glCompileShader(sTessalationControl);
		CheckCompileErrors(sTessalationControl, "TESSALATION_CONTROL");
	}
	if (tessalationEvaluationSource != nullptr)
	{
		sTessalationEvaluation = glCreateShader(GL_TESS_EVALUATION_SHADER);
		glShaderSource(sTessalationEvaluation, 1, &tessalationEvaluationSource, NULL);
		glCompileShader(sTessalationEvaluation);
		CheckCompileErrors(sTessalationEvaluation, "TESSALATION_EVALUATION");
	}
	// shader program
	this->ID = glCreateProgram();
	glAttachShader(this->ID, sVertex);
	glAttachShader(this->ID, sFragment);
	if (geometrySource != nullptr) glAttachShader(this->ID, gShader);
	if (tessalationControlSource != nullptr) glAttachShader(this->ID, sTessalationControl);
	if (tessalationEvaluationSource != nullptr) glAttachShader(this->ID, sTessalationEvaluation);
	glLinkProgram(this->ID);
	CheckCompileErrors(this->ID, "PROGRAM");
	// delete the shaders as they're linked into our program now and no longer necessary
	glDeleteShader(sVertex);
	glDeleteShader(sFragment);
	if (geometrySource != nullptr) glDeleteShader(gShader);
	if (tessalationControlSource != nullptr) glDeleteShader(sTessalationControl);
	if (tessalationEvaluationSource != nullptr) glDeleteShader(sTessalationEvaluation);
}

// utility uniform functions
// ------------------------------------------------------------------------
void shader::SetFloat(const char* name, float value, bool useShader)
{
	if (useShader) this->Use();
	glUniform1f(glGetUniformLocation(this->ID, name), value);
}
void shader::SetFloatArray(const char* name, float* value, int count, bool useShader)
{
	if (useShader) this->Use();
	glUniform1fv(glGetUniformLocation(this->ID, name), count, value);
}
void shader::SetInteger(const char* name, int value, bool useShader)
{
	if (useShader) this->Use();
	glUniform1i(glGetUniformLocation(this->ID, name), value);
}
void shader::SetIntegerArray(const char* name, int* value, int count, bool useShader)
{
	if (useShader) this->Use();
	glUniform1iv(glGetUniformLocation(this->ID, name), count, value);
}
void shader::SetVector2f(const char* name, float x, float y, bool useShader)
{
	if (useShader) this->Use();
	glUniform2f(glGetUniformLocation(this->ID, name), x, y);
}
void shader::SetVector2f(const char* name, const glm::vec2& value, bool useShader)
{
	if (useShader) this->Use();
	glUniform2f(glGetUniformLocation(this->ID, name), value.x, value.y);
}
void shader::SetVector2fArray(const char* name, float* x, float* y, int count, bool useShader)
{
	float* finalVec = new float[count * 2];
	for (int i = 0; i < count; i++)
	{
		finalVec[i * 2] = x[i];
		finalVec[i * 2 + 1] = y[i];
	}
	if (useShader) this->Use();
	glUniform2fv(glGetUniformLocation(this->ID, name), count, finalVec);

	delete[] finalVec;
}

void shader::SetVector2fArray(const char* name, glm::vec2* positions, int count, bool useShader)
{
	float* finalVec = new float[count * 2];
	for (int i = 0; i < count; i++)
	{
		finalVec[i * 2] = positions[i].x;
		finalVec[i * 2 + 1] = positions[i].y;
	}
	if (useShader) this->Use();
	glUniform2fv(glGetUniformLocation(this->ID, name), count, finalVec);

	delete[] finalVec;
}

void shader::SetVector3f(const char* name, float x, float y, float z, bool useShader)
{
	if (useShader) this->Use();
	glUniform3f(glGetUniformLocation(this->ID, name), x, y, z);
}
void shader::SetVector3f(const char* name, const glm::vec3& value, bool useShader)
{
	if (useShader) this->Use();
	glUniform3f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z);
}
void shader::SetVector3fArray(const char* name, glm::vec3* positions, int count, bool useShader)
{
	float* finalVec = new float[count * 3];
	for (int i = 0; i < count; i++)
	{
		finalVec[i * 3] = positions[i].x;
		finalVec[i * 3 + 1] = positions[i].y;
		finalVec[i * 3 + 2] = positions[i].y;
	}
	if (useShader) this->Use();
	glUniform3fv(glGetUniformLocation(this->ID, name), count, finalVec);

	delete[] finalVec;
}
void shader::SetVector4f(const char* name, float x, float y, float z, float w, bool useShader)
{
	if (useShader) this->Use();
	glUniform4f(glGetUniformLocation(this->ID, name), x, y, z, w);
}
void shader::SetVector4f(const char* name, const glm::vec4& value, bool useShader)
{
	if (useShader) this->Use();
	glUniform4f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z, value.w);
}
void shader::SetVector4b(const char* name, const glm::bvec4& value, bool useShader)
{
	if (useShader) this->Use();
	glUniform4i(glGetUniformLocation(this->ID, name), value.x, value.y, value.z, value.w);
}
void shader::SetMatrix4(const char* name, const glm::mat4& matrix, bool useShader)
{
	if (useShader) this->Use();
	glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, false, glm::value_ptr(matrix));
}

// utility function for checking shader compilation/linking errors.
// ------------------------------------------------------------------------
void shader::CheckCompileErrors(unsigned int shader, std::string type)
{
	int success;
	char infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::SHADER_COMPILATION_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
	else
	{
		glGetProgramiv(shader, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(shader, 1024, NULL, infoLog);
			std::cout << "ERROR::PROGRAM_LINKING_ERROR of type: " << type << "\n" << infoLog << "\n -- --------------------------------------------------- -- " << std::endl;
		}
	}
}
