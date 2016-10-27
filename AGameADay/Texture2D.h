#pragma once
#define GLEW_STATIC
#include <GL/glew.h>

// Texture2D is able to store and configure a texture in OpenGL.
// It also hosts utility functions for easy management.
class Texture2D
{
public:
	// Holds the ID of the texture object, used for all texture operations to reference to this particlar texture
	GLuint ID;
	// Texture image dimensions
	GLuint Width, Height; // Width and height of loaded image in pixels
	// Texture Format
	//GLuint Internal_Format; // Format of texture object
	//GLuint Image_Format; // Format of loaded image
	// Texture configuration
	GLuint Wrap_S; // Wrapping mode on S axis
	GLuint Wrap_T; // Wrapping mode on T axis
	GLuint Filter_Min; // Filtering mode if texture pixels < screen pixels
	GLuint Filter_Max; // Filtering mode if texture pixels > screen pixels

public:
	// init/fini
	Texture2D();
	~Texture2D() = default;
	void Delete();

	// binds texture as the current active GL_TEXTURE_2D
	void Bind() const;
	static Texture2D& Texture2D::loadTextureFromFile(const GLchar *file, GLboolean alpha);

	// reloads parameters
	void Regen();

private:
	// generate texture using SOIL2. Types comment: SOIL2 does not use GL types.
	void Generate(int width, int height, unsigned char* data, bool alpha);
};