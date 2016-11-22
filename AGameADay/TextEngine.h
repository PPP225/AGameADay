#pragma once
#define GLEW_STATIC
#include <GL/glew.h>

#include <ft2build.h>
#include FT_FREETYPE_H

#include <glm/glm.hpp>
#include <map>
#include "Shader.h"

enum class Alignment {
	NORMAL,
	BACKWARDS,
	CENTER
};
struct Character {
	GLuint     TextureID;  // ID handle of the glyph texture
	glm::ivec2 Size;       // Size of glyph
	glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
	GLuint     Advance;    // Offset to advance to next glyph
};

class TextEngine
{
public:
	TextEngine();
	~TextEngine();
	/** USAGE TIP: May be initialized many times to different values */
	void Init(GLuint textSize = 24, std::string fontPath = "Data/Fonts/arial.ttf");
	/** Returns text length in pixels */
	GLuint CheckTextLength(std::string text);
	/** Returns text height in pixels. (i.e. height of char "I", as opposed to "g", "j" etc.) */
	GLuint CheckTextHeight();
	/** Renders text */
	void RenderText(std::string text, GLfloat x, GLfloat y, glm::mat4 &projection, glm::vec3 color = glm::vec3(1.0, 1.0, 1.0), Alignment alignment = Alignment::NORMAL);
	void RenderText(std::string text, GLint x, GLint y, glm::mat4 &projection, glm::vec3 color = glm::vec3(1.0, 1.0, 1.0), Alignment alignment = Alignment::NORMAL);
	void RenderText(std::string text, GLuint x, GLuint y, glm::mat4 &projection, glm::vec3 color = glm::vec3(1.0, 1.0, 1.0), Alignment alignment = Alignment::NORMAL);

private:
	std::map<GLchar, Character> Characters;
	GLuint VAO, VBO;
	Shader shader;
	glm::mat4 Projection;
	//helper stuff
	GLboolean ogl_init_ = GL_FALSE;
	void InitOpenGL();
	void ReInit();
	GLuint textHeight_ = -1;
};

