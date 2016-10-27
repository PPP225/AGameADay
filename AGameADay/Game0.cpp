#include "Game0.h"

#include <glm/gtc/matrix_transform.hpp>

void Game0::Init()
{
	// call base class method
	GameBase::Init();

	// set projection matrix to game coordinates
	this->Projection = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);

	// create game objects
	Player.Init("Data/Textures/Game0/awesomeface.png", glm::vec2(0), glm::vec2(1));
}

void Game0::ProcessKeys(GLfloat dt)
{
	// call base class method
	GameBase::ProcessKeys(dt);

	// handle all game input

	// check if player pauses or unpauses game
	if (this->Keys[GLFW_KEY_P] && !this->KeysProcessed[GLFW_KEY_P])
	{
		game_paused_ = !game_paused_;
		this->KeysProcessed[GLFW_KEY_P] = GL_TRUE;
	}

	// if game is paused, don't handle any input
	if (game_paused_)
		return;

	GLfloat currentSpeed = PLAYER_SPEED;
	// is shift being pressed?
	if (this->Keys[GLFW_KEY_LEFT_SHIFT]) {
		currentSpeed *= 4;
	}

	// handle wsad input
	if (this->Keys[GLFW_KEY_W]) {
		Player.Position.y += dt * currentSpeed;
	}
	if (this->Keys[GLFW_KEY_S]) {
		Player.Position.y -= dt * currentSpeed;
	}
	if (this->Keys[GLFW_KEY_A]) {
		Player.Position.x -= dt * currentSpeed;
	}
	if (this->Keys[GLFW_KEY_D]) {
		Player.Position.x += dt * currentSpeed;
	}

	// effects if CTRL is clicked
	if (this->Keys[GLFW_KEY_LEFT_CONTROL]) {
		if (this->Keys[GLFW_KEY_W])
			this->Player.Rotation += 1000.0f * dt;
		if (this->Keys[GLFW_KEY_S])
			this->Player.Rotation -= 1000.0f * dt;
		if (this->Keys[GLFW_KEY_A])
			this->Player.Rotation += 500.0f * dt;
		if (this->Keys[GLFW_KEY_D])
			this->Player.Rotation -= 500.0f * dt;

		if (this->Keys[GLFW_KEY_W]) {
			this->Player.Color.g -= 1.0f * dt;
			if (this->Player.Color.g < 0.0f)
				this->Player.Color.g = 0.0f;
		}
		if (this->Keys[GLFW_KEY_S]) {
			this->Player.Color.g += 1.0f * dt;
			if (this->Player.Color.g > 1.0f)
				this->Player.Color.g = 1.0f;
		}
		if (this->Keys[GLFW_KEY_A]) {
			this->Player.Color.r += 10.0f * dt;
			if (this->Player.Color.r > 1.0f)
				this->Player.Color.r = 1.0f;
		}
		if (this->Keys[GLFW_KEY_D]) {
			this->Player.Color.r -= 10.0f * dt;
			if (this->Player.Color.r < 0.0f)
				this->Player.Color.r = 0.0f;
		}
	}
}

void Game0::Update(GLfloat dt)
{
	// call base class method
	GameBase::Update(dt);

	// update game state, physics, collisions
	// keep the player inside of the screen
	GLfloat gameUnitHeight = this->Height / GAME_UNIT;
	GLfloat gameUnitWidth = this->Width / GAME_UNIT;
	if (Player.Position.y + Player.Size.y > gameUnitHeight)
		Player.Position.y = gameUnitHeight - Player.Size.y;
	if (Player.Position.y < 0.0f)
		Player.Position.y = 0.0f;
	if (Player.Position.x < 0.0f)
		Player.Position.x = 0.0f;
	if (Player.Position.x + Player.Size.x > gameUnitWidth)
		Player.Position.x = gameUnitWidth - Player.Size.x;
}

void Game0::Render()
{
	// clear background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// render game objects
	Player.Render(this->Renderer, this->Projection, this->GAME_UNIT);

	// call base class method (at the end. Otherwise it will be invisible. Order matters.)
	GameBase::Render();
}
