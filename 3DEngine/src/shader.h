#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class shader
{
public:
	// the program ID
	unsigned int ID;

	// constructor reads and builds the shader
	shader();
	shader(const char* vertexPath, const char* fragmentPath);
	// use/activate the shader
	shader& Use();

	void Compile(const char* vertexSource, const char* fragmentSource, const char* geometrySource = nullptr, const char* tessalationControlSource = nullptr,
				 const char* tessalationEvaluationSource = nullptr); // note: geometry source code is optional
	// utility functions
	void SetFloat(const char* name, float value, bool useShader = false);
	void SetFloatArray(const char* name, float* value, int count, bool useShader = false);
	void SetInteger(const char* name, int value, bool useShader = false);
	void SetIntegerArray(const char* name, int* value, int count, bool useShader = false);
	void SetVector2f(const char* name, float x, float y, bool useShader = false);
	void SetVector2f(const char* name, const glm::vec2& value, bool useShader = false);
	void SetVector2fArray(const char* name, float* x, float* y, int count, bool useShader = false);
	void SetVector2fArray(const char* name, glm::vec2* positions, int count, bool useShader = false);
	void SetVector3f(const char* name, float x, float y, float z, bool useShader = false);
	void SetVector3f(const char* name, const glm::vec3& value, bool useShader = false);
	void SetVector3fArray(const char* name, glm::vec3* positions, int count, bool useShader = false);
	void SetVector4f(const char* name, float x, float y, float z, float w, bool useShader = false);
	void SetVector4f(const char* name, const glm::vec4& value, bool useShader = false);
	void SetVector4b(const char* name, const glm::bvec4& value, bool useShader = false);
	void SetMatrix4(const char* name, const glm::mat4& matrix, bool useShader = false);

private:
	// checks if compilation or linking failed and if so, print the error logs
	void CheckCompileErrors(unsigned int object, std::string type);
};
