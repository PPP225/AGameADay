#include "Texture2D.h"

#include <SOIL2/SOIL2.h>

Texture2D::Texture2D()
	: Width(0), Height(0), Wrap_S(GL_CLAMP_TO_EDGE), Wrap_T(GL_CLAMP_TO_EDGE), Filter_Min(GL_LINEAR), Filter_Max(GL_LINEAR)
{
}

void Texture2D::Delete()
{
	glDeleteTextures(1, &this->ID);
}

void Texture2D::Generate(int width, int height, unsigned char* data, bool alpha)
{
	this->Width = width;
	this->Height = height;
	this->ID = SOIL_create_OGL_texture(data, &width, &height, alpha ? 4 : 3, 0, SOIL_FLAG_INVERT_Y);

	// Create Texture
	glBindTexture(GL_TEXTURE_2D, this->ID);
	// Set Texture wrap and filter modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);
	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture2D::Bind() const
{
	glBindTexture(GL_TEXTURE_2D, this->ID);
}

Texture2D& Texture2D::loadTextureFromFile(const GLchar *file, GLboolean alpha)
{
	Texture2D* texture = new Texture2D();

	int width, height;
	unsigned char* image;

	image = SOIL_load_image(file, &width, &height, 0, alpha ? SOIL_LOAD_RGBA : SOIL_LOAD_RGB);
	texture->Generate(width, height, image, alpha);
	SOIL_free_image_data(image);

	return *texture;
}

void Texture2D::Regen()
{
	// Create Texture
	glBindTexture(GL_TEXTURE_2D, this->ID);
	// Set Texture wrap and filter modes
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, this->Wrap_S);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, this->Wrap_T);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, this->Filter_Min);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, this->Filter_Max);
	// Unbind texture
	glBindTexture(GL_TEXTURE_2D, 0);
}
