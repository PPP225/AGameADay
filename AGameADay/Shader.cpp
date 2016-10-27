#include "Shader.h"

#include <sstream>
#include <fstream>
#include <iostream>

Shader& Shader::Use()
{
	glUseProgram(this->ID);
	return *this;
}

void Shader::Compile(const GLchar* vertexSource, const GLchar* fragmentSource)
{
	GLuint sVertex, sFragment, gShader;
	// Vertex Shader
	sVertex = glCreateShader(GL_VERTEX_SHADER);
	glShaderSource(sVertex, 1, &vertexSource, NULL);
	glCompileShader(sVertex);
	checkCompileErrors(sVertex, "VERTEX");
	// Fragment Shader
	sFragment = glCreateShader(GL_FRAGMENT_SHADER);
	glShaderSource(sFragment, 1, &fragmentSource, NULL);
	glCompileShader(sFragment);
	checkCompileErrors(sFragment, "FRAGMENT");

	// Shader Program
	this->ID = glCreateProgram();
	glAttachShader(this->ID, sVertex);
	glAttachShader(this->ID, sFragment);

	glLinkProgram(this->ID);
	checkCompileErrors(this->ID, "PROGRAM");
	// Delete the shaders as they're linked into our program now and no longer necessery
	glDeleteShader(sVertex);
	glDeleteShader(sFragment);
}

void Shader::SetFloat(const GLchar *name, GLfloat value)
{
	glUniform1f(glGetUniformLocation(this->ID, name), value);
}
void Shader::SetInteger(const GLchar *name, GLint value)
{
	glUniform1i(glGetUniformLocation(this->ID, name), value);
}
void Shader::SetVector2f(const GLchar *name, GLfloat x, GLfloat y)
{
	glUniform2f(glGetUniformLocation(this->ID, name), x, y);
}
void Shader::SetVector2f(const GLchar *name, const glm::vec2 &value)
{
	glUniform2f(glGetUniformLocation(this->ID, name), value.x, value.y);
}
void Shader::SetVector3f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z)
{
	glUniform3f(glGetUniformLocation(this->ID, name), x, y, z);
}
void Shader::SetVector3f(const GLchar *name, const glm::vec3 &value)
{
	glUniform3f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z);
}
void Shader::SetVector4f(const GLchar *name, GLfloat x, GLfloat y, GLfloat z, GLfloat w)
{
	glUniform4f(glGetUniformLocation(this->ID, name), x, y, z, w);
}
void Shader::SetVector4f(const GLchar *name, const glm::vec4 &value)
{
	glUniform4f(glGetUniformLocation(this->ID, name), value.x, value.y, value.z, value.w);
}
void Shader::SetMatrix4(const GLchar *name, const glm::mat4 &matrix)
{
	glUniformMatrix4fv(glGetUniformLocation(this->ID, name), 1, GL_FALSE, glm::value_ptr(matrix));
}


void Shader::checkCompileErrors(GLuint object, std::string type)
{
	GLint success;
	GLchar infoLog[1024];
	if (type != "PROGRAM")
	{
		glGetShaderiv(object, GL_COMPILE_STATUS, &success);
		if (!success)
		{
			glGetShaderInfoLog(object, 1024, NULL, infoLog);
			std::cout << "| ERROR::SHADER: Compile-time error: Type: " + type + "\n"
				+ infoLog + "\n -- --------------------------------------------------- -- ";
		}
	}
	else
	{
		glGetProgramiv(object, GL_LINK_STATUS, &success);
		if (!success)
		{
			glGetProgramInfoLog(object, 1024, NULL, infoLog);
			std::cout << "| ERROR::Shader: Link-time error: Type: " + type + "\n"
				+ infoLog + "\n -- --------------------------------------------------- -- ";
		}
	}
}

Shader& Shader::LoadShader(const GLchar *vShaderFile, const GLchar *fShaderFile)
{
	return loadShaderFromFile(vShaderFile, fShaderFile);
}

Shader& Shader::loadShaderFromFile(const GLchar *vShaderFile, const GLchar *fShaderFile)
{
		// 1. Retrieve the vertex/fragment source code from filePath
		std::string vertexCode;
		std::string fragmentCode;
		std::ifstream vShaderFileStream;
		std::ifstream fShaderFileStream;
		// ensures ifstream objects can throw exceptions:
		vShaderFileStream.exceptions(std::ifstream::badbit);
		fShaderFileStream.exceptions(std::ifstream::badbit);
		try
		{
			// Open files
			vShaderFileStream.open(vShaderFile);
			fShaderFileStream.open(fShaderFile);
			std::stringstream vShaderStream, fShaderStream;
			// Read file's buffer contents into streams
			vShaderStream << vShaderFileStream.rdbuf();
			fShaderStream << fShaderFileStream.rdbuf();
			// close file handlers
			vShaderFileStream.close();
			fShaderFileStream.close();
			// Convert stream into string
			vertexCode = vShaderStream.str();
			fragmentCode = fShaderStream.str();
		}
		catch (std::ifstream::failure e)
		{
			std::cout << "ERROR::SHADER::FILE_NOT_SUCCESFULLY_READ";
		}
	const GLchar *vShaderCode = vertexCode.c_str();
	const GLchar *fShaderCode = fragmentCode.c_str();
	// 2. Now create shader object from source code
	Shader shader;
	shader.Compile(vShaderCode, fShaderCode);
	return shader;
}
