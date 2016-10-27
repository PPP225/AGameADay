/************************************************
* Shader
************************************************/
#pragma once

#include <GL/glew.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <string>

class Shader
{
public:
	// Constructor
	Shader() = default;
	~Shader() = default;
	void Delete() {
		glUseProgram(0);
		if (this->ID != NULL)
			glDeleteProgram(this->ID);
	};
	// Sets the current shader as active
	Shader& Use();
	// Loads (and generates) a shader program from file loading vertex and fragment shader's source code.
	static Shader& LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile);
	// Utility functions
	void    SetFloat(const GLchar *name, GLfloat value);
	void    SetInteger(const GLchar *name, GLint value);
	void    SetVector2f(const GLchar *name, GLfloat x, GLfloat y);
	void    SetVector2f(const GLchar *name, const glm::vec2 &value);
	void    SetVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z);
	void    SetVector3f(const GLchar *name, const glm::vec3 &value);
	void    SetVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w);
	void    SetVector4f(const GLchar *name, const glm::vec4 &value);
	void    SetMatrix4(const GLchar *name, const glm::mat4 &matrix);
private:
	// State
	GLuint ID = NULL;
	// Compiles the shader from given source code
	void    Compile(const GLchar *vertexSource, const GLchar *fragmentSource);
	// Checks if compilation or linking failed and if so, print the error logs
	void    checkCompileErrors(GLuint object, std::string type);
	// Loads and generates a shader from file
	static Shader& loadShaderFromFile(const GLchar * vShaderFile, const GLchar * fShaderFile);
};