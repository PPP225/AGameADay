#pragma once

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "SpriteRenderer.h"
#include "SoundEngine.h"
#include "TextEngine.h"

class GameObjectBase
{
public:
	GameObjectBase() = default;
	~GameObjectBase() = default;
	virtual void Init(Texture2D& texture, glm::vec2 position, glm::vec2 size = glm::vec2(1.0f, 1.0f));
	virtual void Init(const GLchar *path, glm::vec2 position, glm::vec2 size = glm::vec2(1.0f, 1.0f));
	virtual void Delete();
	virtual void Render(SpriteRenderer& renderer, glm::mat4& projection, GLfloat gameUnit);

	glm::vec2 Position;
	glm::vec2 Size;
	GLfloat Rotation;
	glm::vec4 Color;
	Texture2D Texture;
	GLboolean FlipTexture;

	glm::vec2 GetCenter();

private:
	GLboolean initialized_ = GL_FALSE;
};

class GameBase
{
public:
	// Game state
	GLboolean	Keys[1024];
	GLboolean	KeysProcessed[1024];
	const GLuint Width = 960;
	const GLuint Height = 540;
	GLuint TextSize = 20;

	// Init/Deinit
	GameBase() = default;
	virtual ~GameBase();

	virtual void Init() = 0;

	// gameloop
	virtual void ProcessKeys(GLfloat dt) = 0;
	virtual void Update(GLfloat dt) = 0;
	virtual void Render() = 0;

protected:
	// objects
	SpriteRenderer Renderer;
	SoundEngine MusicPlayer;
	TextEngine TextRenderer;
};

