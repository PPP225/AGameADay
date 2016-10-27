#include "SpriteRenderer.h"

#include <glm/gtc/matrix_transform.hpp>

void SpriteRenderer::Init()
{
	InitRenderData();
	this->SRShader = Shader::LoadShader("Data/Shaders/genericSprite.vert", "Data/Shaders/genericSprite.frag");
}

void SpriteRenderer::Delete()
{
	glDeleteVertexArrays(1, &this->VAO);
	this->SRShader.Delete();
}

void SpriteRenderer::Draw(Texture2D & texture, glm::mat4& projection, glm::vec2 position, glm::vec2 size, GLfloat rotate, GLboolean flipY, glm::vec4 color)
{
	this->SRShader.Use();

	// set Projection matrix
	this->SRShader.SetMatrix4("projection", projection);

	// calculate object transformations
	glm::mat4 model;
	model = glm::translate(model, glm::vec3(position, 0.0f));  // translate (transformation order: scale -> rotation -> translation; reversed order because of matrix math)

	// flip  on Y axis
	if (flipY)
	{
		model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move, so it rotates the way we want
		model = glm::rotate(model, glm::radians(180.0f), glm::vec3(0.0f, 1.0f, 0.0f)); // flip
		model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));
	}

	model = glm::translate(model, glm::vec3(0.5f * size.x, 0.5f * size.y, 0.0f)); // move, so it rotates the way we want
	model = glm::rotate(model, glm::radians(rotate), glm::vec3(0.0f, 0.0f, 1.0f)); // rotate
	model = glm::translate(model, glm::vec3(-0.5f * size.x, -0.5f * size.y, 0.0f));


	model = glm::scale(model, glm::vec3(size, 1.0f)); // scale

	// set model matrix
	this->SRShader.SetMatrix4("model", model);

	// set color
	this->SRShader.SetVector4f("spriteColor", color);

	// render
	glActiveTexture(GL_TEXTURE0);
	texture.Bind();

	glBindVertexArray(this->VAO);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glBindVertexArray(0);
}

void SpriteRenderer::InitRenderData()
{
	GLfloat vertices[] = {
		// pos      // tex
		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 0.0f,

		0.0f, 1.0f, 0.0f, 1.0f,
		1.0f, 1.0f, 1.0f, 1.0f,
		1.0f, 0.0f, 1.0f, 0.0f
	};

	// generate
	glGenVertexArrays(1, &this->VAO);
	GLuint VBO;
	glGenBuffers(1, &VBO);

	// bind
	glBindVertexArray(this->VAO);

	// buffer data
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	// unbind and cleanup
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	glDeleteBuffers(1, &VBO);
}