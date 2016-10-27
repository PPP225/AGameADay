#include "TextEngine.h"
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>

TextEngine::TextEngine()
{
}

TextEngine::~TextEngine()
{
	glDeleteVertexArrays(1, &this->VAO);
	glDeleteBuffers(1, &this->VBO);
	this->shader.Delete();

	for (auto& it : Characters)
		glDeleteTextures(1, &it.second.TextureID);
}

void TextEngine::Init(GLuint textSize)
{
	ReInit();

	// opengl
	if (!ogl_init_)
		InitOpenGL();

	// Freetype

	FT_Library ft;
	FT_Face face;
	
	if (FT_Init_FreeType(&ft))
		std::cout << ("ERROR::FREETYPE: Could not init FreeType Library");

	if (FT_New_Face(ft, "Data/Fonts/arial.ttf", 0, &face))
		std::cout << ("ERROR::FREETYPE: Failed to load font");

	FT_Set_Pixel_Sizes(face, 0, textSize);

	if (FT_Load_Char(face, 'X', FT_LOAD_RENDER))
		std::cout << ("ERROR::FREETYTPE: Failed to load Glyph");

	this->textHeight_ = face->glyph->bitmap_top;

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); // Disable byte-alignment restriction

	for (GLubyte c = 0; c < 128; c++)
	{
		// Load character glyph 
		if (FT_Load_Char(face, c, FT_LOAD_RENDER))
		{
			std::cout << ("ERROR::FREETYTPE: Failed to load Glyph");
			continue;
		}
		// Generate texture
		GLuint texture;
		glGenTextures(1, &texture);
		glBindTexture(GL_TEXTURE_2D, texture);
		glTexImage2D(
			GL_TEXTURE_2D,
			0,
			GL_RED,
			face->glyph->bitmap.width,
			face->glyph->bitmap.rows,
			0,
			GL_RED,
			GL_UNSIGNED_BYTE,
			face->glyph->bitmap.buffer
		);
		// Set texture options
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		// Now store character for later use
		Character character = {
			texture,
			glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
			glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
			face->glyph->advance.x
		};
		Characters.insert(std::pair<GLchar, Character>(c, character));
	}

	FT_Done_Face(face);
	FT_Done_FreeType(ft);
}

GLuint TextEngine::CheckTextLength(std::string text)
{
	std::string::const_iterator c;

	GLuint totalAdvance = 0;
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];
		totalAdvance += (ch.Advance >> 6);
	}

	return totalAdvance;
}

GLuint TextEngine::CheckTextHeight()
{
	return this->textHeight_;
}

void TextEngine::RenderText(std::string text, GLfloat x, GLfloat y, glm::mat4 &projection, glm::vec3 color, Alignment alignment)
{
	GLfloat scale = 1.0f;
	// Activate corresponding render state	
	&this->shader.Use();
	this->shader.SetMatrix4("projection", projection);
	this->shader.SetVector3f("textColor", color);

	glActiveTexture(GL_TEXTURE0);
	glBindVertexArray(VAO);

	std::string::const_iterator c;
	if (alignment == Alignment::BACKWARDS)
	{
		GLuint totalAdvance = 0;
		for (c = text.begin(); c != text.end(); c++)
		{
			Character ch = Characters[*c];
			totalAdvance += (ch.Advance >> 6);
		}
		x -= totalAdvance;
	}
	else if (alignment == Alignment::CENTER) {
		GLuint totalAdvance = 0;
		for (c = text.begin(); c != text.end(); c++)
		{
			Character ch = Characters[*c];
			totalAdvance += (ch.Advance >> 6);
		}
		x -= totalAdvance / 2.0;
	}
	// Iterate through all characters
	for (c = text.begin(); c != text.end(); c++)
	{
		Character ch = Characters[*c];

		GLfloat xpos = x + ch.Bearing.x * scale;
		GLfloat ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

		GLfloat w = ch.Size.x * scale;
		GLfloat h = ch.Size.y * scale;
		// Update VBO for each character
		GLfloat vertices[6][4] = {
			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos,     ypos,       0.0, 1.0 },
			{ xpos + w, ypos,       1.0, 1.0 },

			{ xpos,     ypos + h,   0.0, 0.0 },
			{ xpos + w, ypos,       1.0, 1.0 },
			{ xpos + w, ypos + h,   1.0, 0.0 }
		};
		// Render glyph texture over quad
		glBindTexture(GL_TEXTURE_2D, ch.TextureID);
		// Update content of VBO memory
		glBindBuffer(GL_ARRAY_BUFFER, VBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		// Render quad
		glDrawArrays(GL_TRIANGLES, 0, 6);
		// Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
		x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64)
	}
	glBindVertexArray(0);
	glBindTexture(GL_TEXTURE_2D, 0);
}

void TextEngine::RenderText(std::string text, GLint x, GLint y, glm::mat4 & projection, glm::vec3 color, Alignment alignment)
{
	RenderText(text, (GLfloat)x, (GLfloat)y, projection, color, alignment);
}

void TextEngine::RenderText(std::string text, GLuint x, GLuint y, glm::mat4 & projection, glm::vec3 color, Alignment alignment)
{
	RenderText(text, (GLfloat)x, (GLfloat)y, projection, color, alignment);
}

void TextEngine::InitOpenGL()
{
	this->shader = Shader::LoadShader("Data/Shaders/text.vert", "Data/Shaders/text.frag");


	glGenVertexArrays(1, &this->VAO);

	glGenBuffers(1, &this->VBO);

	glBindVertexArray(this->VAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, NULL, GL_DYNAMIC_DRAW);

	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	ogl_init_ = GL_TRUE;
}

void TextEngine::ReInit()
{
	for (auto& it : Characters)
		glDeleteTextures(1, &it.second.TextureID);
	Characters.clear();
}
