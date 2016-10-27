#include "Game2.h"

#include <glm/gtc/matrix_transform.hpp>

void Game2::Init()
{
	// call base class method
	GameBase::Init();

	// set projection matrix to game coordinates
	this->Projection = glm::ortho(0.0f, (GLfloat)this->Width, 0.0f, (GLfloat)this->Height, -1.0f, 1.0f);

	// create game objects
	Player1.Init("Data/Textures/Game2/White.png", glm::vec2(10.0f, this->Height / 2.0f - PADDLE_SIZE.y / 2.0f), PADDLE_SIZE);
	Player2.Init("Data/Textures/Game2/White.png", glm::vec2(this->Width - 10.0f - PADDLE_SIZE.x, this->Height / 2.0f - PADDLE_SIZE.y / 2.0f), PADDLE_SIZE);
	Ball.Init("Data/Textures/Game2/White.png", glm::vec2(this->Width *0.5f, this->Height * 0.5f), BALL_SIZE);

	this->MusicPlayer.PlaySound("Data/Audio/Game2/bgm.xm", true);

	// reset all game parameters to default values.
	RestartGame();
}

void Game2::ProcessKeys(GLfloat dt)
{
	// call base class method
	GameBase::ProcessKeys(dt);

	// restart game on F5
	if (this->Keys[GLFW_KEY_F5])
		RestartGame();

	// change AI
	if (this->Keys[GLFW_KEY_I] && !this->KeysProcessed[GLFW_KEY_I])
	{
		this->AI = !this->AI;
		this->KeysProcessed[GLFW_KEY_I] = GL_TRUE;
	}

	switch (State)
	{
	case GAME_ACTIVE:
		// Move player 1
		if (this->Keys[GLFW_KEY_W])
		{
			if (Player1.Position.y <= this->Height - Player1.Size.y)
				Player1.Position.y += PADDLE_VELOCITY * dt;
		}
		if (this->Keys[GLFW_KEY_S])
		{
			if (Player1.Position.y >= 0)
				Player1.Position.y -= PADDLE_VELOCITY * dt;
		}
		// Move player 2
		// as AI
		if (AI == GL_TRUE)
		{
			GLint randLol = (GLint)Player2.Position.y % 3;
			GLfloat plannedBouncePosition = Player2.Position.y + Player2.Size.y / 2.0f;	// init to paddle center
			switch (randLol)
			{
			case 0:
				plannedBouncePosition -= Player2.Size.y / 2.0f;
				break;
			case 2:
				plannedBouncePosition += Player2.Size.y / 2.0f;
				break;
			default:
				break;
			}
			GLfloat ballCenter = Ball.Position.y + Ball.Size.y / 2.0f;

			if (ballCenter < plannedBouncePosition)
			{
				if (Player2.Position.y >= 0)
					Player2.Position.y -= PADDLE_VELOCITY * dt;
			}
			if (ballCenter > plannedBouncePosition)
			{
				if (Player2.Position.y <= this->Height - Player2.Size.y)
					Player2.Position.y += PADDLE_VELOCITY * dt;
			}

		}
		else
		{
			// as human
			if (this->Keys[GLFW_KEY_UP])
			{
				if (Player2.Position.y <= this->Height - Player2.Size.y)
					Player2.Position.y += PADDLE_VELOCITY * dt;
			}
			if (this->Keys[GLFW_KEY_DOWN])
			{
				if (Player2.Position.y >= 0)
					Player2.Position.y -= PADDLE_VELOCITY * dt;
			}
		}
		// pause game
		if (this->Keys[GLFW_KEY_SPACE] && !this->KeysProcessed[GLFW_KEY_SPACE])
		{
			this->State = GameState::GAME_PAUSE;
			this->KeysProcessed[GLFW_KEY_SPACE] = GL_TRUE;
		}
		if (this->Keys[GLFW_KEY_P] && !this->KeysProcessed[GLFW_KEY_P])
		{
			this->State = GameState::GAME_PAUSE;
			this->KeysProcessed[GLFW_KEY_P] = GL_TRUE;
		}
		break;
	case GAME_PAUSE:
		// unpause game
		if (this->Keys[GLFW_KEY_SPACE] && !this->KeysProcessed[GLFW_KEY_SPACE])
		{
			this->State = GameState::GAME_ACTIVE;
			this->KeysProcessed[GLFW_KEY_SPACE] = GL_TRUE;
		}
		if (this->Keys[GLFW_KEY_P] && !this->KeysProcessed[GLFW_KEY_P])
		{
			this->State = GameState::GAME_ACTIVE;
			this->KeysProcessed[GLFW_KEY_P] = GL_TRUE;
		}
		break;
	case GAME_OVER:
		// restart game
		if (this->Keys[GLFW_KEY_SPACE])
		{
			this->KeysProcessed[GLFW_KEY_SPACE] = GL_TRUE;
			RestartGame();
		}
		break;
	default:
		break;
	}
}

void Game2::Update(GLfloat dt)
{
	// call base class method
	GameBase::Update(dt);

	if (this->State == GAME_ACTIVE)
	{
		// Move the ball
		Ball.Position += Ball.Velocity * dt;

		// check if ball went outside of screen - up or down
		if (Ball.Position.y <= 0.0f)
		{
			Ball.Velocity.y = -Ball.Velocity.y;
			Ball.Position.y = 0.0f;
		}
		else if (Ball.Position.y + Ball.Size.y >= this->Height)
		{
			Ball.Velocity.y = -Ball.Velocity.y;
			Ball.Position.y = this->Height - Ball.Size.y;
		}

		// ball is outside of screen - right or left
		if (Ball.Position.x < -20)
		{
			Ball.Velocity.x = -BALL_INIT_VELOCITY.x;
			Ball.Velocity.y = BALL_INIT_VELOCITY.y;
			this->Ball.Position = glm::vec2(this->Width * 0.5f, this->Height * 0.5f);
			lastPoint1 = GL_FALSE;
			++Points2;
			this->State = GameState::GAME_PAUSE;
		}
		if (Ball.Position.x > Width)
		{
			Ball.Velocity = BALL_INIT_VELOCITY;
			this->Ball.Position = glm::vec2(this->Width * 0.5f, this->Height * 0.5f);
			lastPoint1 = GL_TRUE;
			++Points1;
			this->State = GameState::GAME_PAUSE;
		}

		// ball-player collisions
		ResolveCollisions();
	}
	if ((this->State == GAME_ACTIVE || this->State == GAME_PAUSE) && (Points1 >= MAX_SCORE || Points2 >= MAX_SCORE))
	{
		this->State = GAME_OVER;
	}
}

void Game2::Render()
{
	switch (State)
	{
	case GAME_ACTIVE:
		RenderGameBackground();
		Player1.Render(this->Renderer, this->Projection, this->GAME_UNIT);
		Player2.Render(this->Renderer, this->Projection, this->GAME_UNIT);
		Ball.Render(this->Renderer, this->Projection, this->GAME_UNIT);
		break;
	case GAME_PAUSE:
		RenderPauseBackground();
		Player1.Render(this->Renderer, this->Projection, this->GAME_UNIT);
		Player2.Render(this->Renderer, this->Projection, this->GAME_UNIT);
		Ball.Render(this->Renderer, this->Projection, this->GAME_UNIT);
		break;
	case GAME_OVER:
		RenderGameoverBackground();
		break;
	default:
		break;
	}

	// call base class method (at the end. Otherwise it will be invisible. Order matters.)
	GameBase::Render();
}

// -------------------------------------------------------------------------- Collisions ----------------------------------------------

void Game2::ResolveCollisions()
{
	GLfloat ballPosX = Ball.Position.x;
	GLfloat ballPosY = Ball.Position.y;

	// check Ball-Player1 collision
	GLfloat paddlePosX = Player1.Position.x;
	GLfloat paddleSizeX = Player1.Size.x;
	// check, if the ball collides with player1 on the X axis
	if (ballPosX < paddlePosX + paddleSizeX) {
		GLfloat paddlePosY = Player1.Position.y;
		GLfloat paddleSizeY = Player1.Size.y;

		// check, if the ball collides with player1 on the Y axis
		if (ballPosY < paddlePosY + paddleSizeY && ballPosY + Ball.Size.y > paddlePosY) {
			IncreaseBallSpeed();
			// play sound
			MusicPlayer.PlaySound("Data/Audio/Game2/pongblip.ogg");

			GLfloat Xpen = paddlePosX + paddleSizeX - ballPosX;
			GLfloat YpenBottom = paddlePosY + paddleSizeY - ballPosY;
			GLfloat YpenUp = ballPosY + Ball.Size.y - paddlePosY;
			GLfloat Ypen = std::fmin(YpenBottom, YpenUp);
			GLboolean Xaxis = Xpen < Ypen ? true : false;
			if (Xaxis) {
				Ball.Position += Xpen;
				glm::vec2 oldVel = Ball.Velocity;
				Ball.Velocity.x = -Ball.Velocity.x;
				//adjust speed based on position hit
				GLfloat paddleCenterY = paddlePosY + paddleSizeY / 2.0f;
				GLfloat ballCenterY = ballPosY + Ball.Size.y / 2.0f;
				GLfloat distance = ballCenterY - paddleCenterY;
				GLfloat percentage = distance / (paddleSizeY / 2.0f);
				Ball.Velocity.y = (std::abs(Ball.Velocity.y) + 0.2f * glm::length(oldVel)) * percentage * 1.0f;
				Ball.Velocity = glm::normalize(Ball.Velocity) * glm::length(oldVel);
			}
			else {	//Yaxis
				if (Ypen == YpenUp)
					Ypen = -Ypen;
				Ball.Position.y += Ypen;
				Ball.Velocity.y = -Ball.Velocity.y;
			}
		}
	}
	else {	// check Ball-Player2 collision
		paddlePosX = Player2.Position.x;
		paddleSizeX = Player2.Size.x;
		// check, if the ball collides with player2 on the X axis
		if (ballPosX + Ball.Size.x > paddlePosX) {
			GLfloat paddlePosY = Player2.Position.y;
			GLfloat paddleSizeY = Player2.Size.y;

			// check, if the ball collides with player2 on the Y axis
			if (ballPosY < paddlePosY + paddleSizeY && ballPosY + Ball.Size.y > paddlePosY) {
				IncreaseBallSpeed();
				// play sound
				MusicPlayer.PlaySound("Data/Audio/Game2/pongblip.ogg");

				GLfloat Xpen = (ballPosX + Ball.Size.x) - paddlePosX;
				GLfloat YpenBottom = paddlePosY + paddleSizeY - ballPosY;
				GLfloat YpenUp = ballPosY + Ball.Size.y - paddlePosY;
				GLfloat Ypen = std::fmin(YpenBottom, YpenUp);
				GLboolean Xaxis = Xpen < Ypen ? true : false;
				if (Xaxis) {
					Ball.Position -= Xpen;
					glm::vec2 oldVel = Ball.Velocity;
					Ball.Velocity.x = -Ball.Velocity.x;
					//adjust speed based on position hit
					GLfloat paddleCenterY = paddlePosY + paddleSizeY / 2.0f;
					GLfloat ballCenterY = ballPosY + Ball.Size.y / 2.0f;
					GLfloat distance = ballCenterY - paddleCenterY;
					GLfloat percentage = distance / (paddleSizeY / 2.0f);
					Ball.Velocity.y = (std::abs(Ball.Velocity.y) + 0.2f * glm::length(oldVel)) * percentage * 1.0f;
					Ball.Velocity = glm::normalize(Ball.Velocity) * glm::length(oldVel);
				}
				else {	//Yaxis
					if (Ypen == YpenUp)
						Ypen = -Ypen;
					Ball.Position.y += Ypen;
					Ball.Velocity.y = -Ball.Velocity.y;
				}
			}
		}
	}
}

void Game2::IncreaseBallSpeed()
{
	Ball.Velocity = glm::normalize(Ball.Velocity) * glm::length(Ball.Velocity) * 1.1f;
}

// ------------------------------------------------------------------------- State Change -------------------------------------------------------------------------

void Game2::RestartGame()
{
	// set new game state
	this->State = GameState::GAME_PAUSE;

	// reset points
	this->Points1 = 0;
	this->Points2 = 0;

	// reset player and ball positions and velocity
	this->Player1.Position = glm::vec2(10.0f, this->Height / 2.0f - PADDLE_SIZE.y / 2.0f);
	this->Player2.Position = glm::vec2(this->Width - 10.0f - PADDLE_SIZE.x, this->Height / 2.0f - PADDLE_SIZE.y / 2.0f);
	this->Ball.Position = glm::vec2(this->Width *0.5f, this->Height * 0.5f);
	this->Ball.Velocity = BALL_INIT_VELOCITY;
}

// ------------------------------------------------------------------------- Helper Functions -------------------------------------------------------------------------

void Game2::RenderGameBackground()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	std::string p1 = std::to_string(this->Points1);
	std::string p2 = std::to_string(this->Points2);
	glm::vec3 colour(0.5, 0.5, 0.5);
	for (GLuint i = 0; i < this->Height; i += 48) {
		this->TextRenderer.RenderText(p1, this->Width / 2, i, this->Projection, colour, Alignment::BACKWARDS);
	}
	for (GLuint i = 24; i < this->Height; i += 48) {
		this->TextRenderer.RenderText(p2, this->Width / 2, i, this->Projection, colour, Alignment::NORMAL);
	}
}

void Game2::RenderPauseBackground()
{
	RenderGameBackground();

	glm::vec3 white(1, 1, 1);
	glm::vec3 grey(0.5f, 0.5f, 0.5f);
	this->TextRenderer.RenderText("GAME PAUSED", this->Width / 2 - 30, this->Height - 200, this->Projection, white, Alignment::BACKWARDS);
	this->TextRenderer.RenderText("(press p or space to unpause)", this->Width / 2 + 30, this->Height - 200, this->Projection, white, Alignment::NORMAL);

	this->TextRenderer.RenderText("W", 10, (GLint)this->Height - 30, this->Projection, grey, Alignment::NORMAL);
	this->TextRenderer.RenderText("S", 10, 30, this->Projection, grey, Alignment::NORMAL);
	this->TextRenderer.RenderText("Up Arrow", (GLint)this->Width - 10, (GLint)this->Height - 30, this->Projection, grey, Alignment::BACKWARDS);
	this->TextRenderer.RenderText("Down Arrow", (GLint)this->Width - 10, 30, this->Projection, grey, Alignment::BACKWARDS);

	if (AI)
	{
		this->TextRenderer.RenderText("Player 2 AI is ON", this->Width / 2 + 50, this->Height - 300, this->Projection, white, Alignment::NORMAL);
		this->TextRenderer.RenderText("Press i to turn off AI.", this->Width / 2 + 50, this->Height - 330, this->Projection, white, Alignment::NORMAL);
	}
	else
	{
		this->TextRenderer.RenderText("Player 2 AI is OFF", this->Width / 2 + 50, this->Height - 300, this->Projection, white, Alignment::NORMAL);
		this->TextRenderer.RenderText("Press i to turn on AI.", this->Width / 2 + 50, this->Height - 330, this->Projection, white, Alignment::NORMAL);
	}
}

void Game2::RenderGameoverBackground()
{
	std::string result;
	if (lastPoint1)
		result = "GAME OVER: Player 1 WON!";
	else
		result = "GAME OVER: Player 2 WON!";

	std::string score = std::to_string(Points1) + " : " + std::to_string(Points2);

	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	glm::vec3 colour(1, 1, 1);
	this->TextRenderer.RenderText(result, this->Width / 2, this->Height / 2, this->Projection, colour, Alignment::CENTER);
	this->TextRenderer.RenderText(score, this->Width / 2, this->Height / 2 - 24, this->Projection, colour, Alignment::CENTER);
	this->TextRenderer.RenderText("(press space or F5 to restart)", this->Width / 2, this->Height / 2 - 48, this->Projection, colour, Alignment::CENTER);
}
