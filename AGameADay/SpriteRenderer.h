#pragma once

#include <glm/glm.hpp>

#include "Shader.h"
#include "Texture2D.h"

class SpriteRenderer
{
public:
	// init/fini
	SpriteRenderer() = default;
	~SpriteRenderer() = default;
	void Init();
	void Delete();

	void Draw(Texture2D &texture, glm::mat4& projection, glm::vec2 position, glm::vec2 size, GLfloat rotate = 0.0f, GLboolean flipY = GL_FALSE, glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

private:
	// state
	Shader SRShader;
	GLuint VAO = NULL;

	// helper functions 
	void InitRenderData();
};