#include "GameBase.h"

#include <iostream>

// ------------------------------------------------------------------------- GameObject Base -------------------------------------------------------------------------


void GameObjectBase::Init(Texture2D & texture, glm::vec2 position, glm::vec2 size)
{
	this->Texture = texture;
	this->Position = position;
	this->Size = size;

	this->Rotation = 0.0f;
	this->FlipTexture = GL_FALSE;
	this->Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	initialized_ = GL_TRUE;
}

void GameObjectBase::Init(const GLchar * path, glm::vec2 position, glm::vec2 size)
{
	this->Texture = Texture2D::loadTextureFromFile(path, GL_TRUE);
	this->Position = position;
	this->Size = size;

	this->Rotation = 0.0f;
	this->FlipTexture = GL_FALSE;
	this->Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	initialized_ = GL_TRUE;
}

void GameObjectBase::Delete()
{
	Texture.Delete();
}

void GameObjectBase::Render(SpriteRenderer & renderer, glm::mat4 & projection, GLfloat gameUnit)
{
	if (!initialized_)
	{
		std::cout << "ERROR::GAMEBASE: GameObjectBase not initialized!";
		return;
	}
	renderer.Draw(this->Texture, projection, this->Position * gameUnit, this->Size * gameUnit, this->Rotation, this->FlipTexture, this->Color);
}

glm::vec2 GameObjectBase::GetCenter()
{
	return (this->Position + this->Size / 2.0f);
}

// ------------------------------------------------------------------------- Game Base -------------------------------------------------------------------------

GameBase::~GameBase()
{
	this->Renderer.Delete();
	this->MusicPlayer.Delete();
}

void GameBase::Init()
{
	this->Renderer.Init();
	this->MusicPlayer.Init();
	this->TextRenderer.Init(TextSize);
}

void GameBase::ProcessKeys(GLfloat dt)
{
	// control volume (with F9/F10 or -/+ or numpad -/+)
	if (this->Keys[GLFW_KEY_F9] && !this->KeysProcessed[GLFW_KEY_F9] || 
		this->Keys[GLFW_KEY_MINUS] && !this->KeysProcessed[GLFW_KEY_MINUS] ||
		this->Keys[GLFW_KEY_KP_SUBTRACT] && !this->KeysProcessed[GLFW_KEY_KP_SUBTRACT])
	{
		this->MusicPlayer.VolumeDown();
		this->KeysProcessed[GLFW_KEY_F9] = GL_TRUE;
		this->KeysProcessed[GLFW_KEY_MINUS] = GL_TRUE;
		this->KeysProcessed[GLFW_KEY_KP_SUBTRACT] = GL_TRUE;
	}
	if (this->Keys[GLFW_KEY_F10] && !this->KeysProcessed[GLFW_KEY_F10] ||
		this->Keys[GLFW_KEY_EQUAL] && !this->KeysProcessed[GLFW_KEY_EQUAL] ||
		this->Keys[GLFW_KEY_KP_ADD] && !this->KeysProcessed[GLFW_KEY_KP_ADD])
	{
		this->MusicPlayer.VolumeUp();
		this->KeysProcessed[GLFW_KEY_F10] = GL_TRUE;
		this->KeysProcessed[GLFW_KEY_EQUAL] = GL_TRUE;
		this->KeysProcessed[GLFW_KEY_KP_ADD] = GL_TRUE;
	}
}

void GameBase::Update(GLfloat dt)
{
	// currently nothing here.
}

void GameBase::Render()
{
	// currently nothing here.
	// things like an FPS counter may be added here.
}
