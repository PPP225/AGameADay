#include "Game5.h"

#include <glm/gtc/matrix_transform.hpp>
#include <time.h>

void Game5::Init()
{
	// call base class method
	GameBase::Init();

	// seed the random number generator 
	time_t timev;
	time(&timev);
	srand((unsigned int)timev);

	// text renderer change font
	TextRenderer.Init(40,"Data/Fonts/pixel.ttf");

	// set projection matrix to game coordinates
	this->Projection = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);

	// create game objects
	Player.Init();

	glm::vec2 offset(0.75f);
	Background1.Init("Data/Textures/Game5/white.png", offset, glm::vec2(24, 12) - offset * 2.0f);
	offset = glm::vec2(1.0f);
	Background2.Init("Data/Textures/Game5/white.png", offset, glm::vec2(24, 12) - offset * 2.0f);
	Background2.Color = Color::Black;

	offset = glm::vec2(3.0f, 4.0f);
	WinBackground.Init("Data/Textures/Game5/white.png", offset, glm::vec2(24, 12) - offset * 2.0f);
	WinBackground.Color = Color::Grey25;

	Food.Init("Data/Textures/Game5/white.png", glm::vec2(5, 8), glm::vec2(1, 1));
	SpawnFood();

	SongNum = rand() % 2;
	PlayNextSong();
}

void Game5::ProcessKeys(GLfloat dt)
{
	// call base class method
	GameBase::ProcessKeys(dt);

	// handle all game input

	if (HandleKey(GLFW_KEY_N))
		PlayNextSong();

	if (game_menu_)
	{
		if (HandleKey(GLFW_KEY_W) || HandleKey(GLFW_KEY_D) || HandleKey(GLFW_KEY_RIGHT))
		{
			SetDifficulty(Difficulty + 1);
		}
		if (HandleKey(GLFW_KEY_S) || HandleKey(GLFW_KEY_A) || HandleKey(GLFW_KEY_LEFT))
		{
			SetDifficulty(Difficulty - 1);
		}
		if (HandleKey(GLFW_KEY_SPACE) || HandleKey(GLFW_KEY_ENTER))
		{
			game_menu_ = GL_FALSE;
		}
		return;
	}

	// restart game on F5
	if (this->Keys[GLFW_KEY_F5])
		OnRestart();

	// end key capture if gameover
	if (game_over_)
	{
		if (HandleKey(GLFW_KEY_SPACE))
			OnRestart();
		return;
	}

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
	if (HandleKey(GLFW_KEY_W))
		if (!Player.ChangeDirection(Game5Snake::Direction::UP))
			this->KeysProcessed[GLFW_KEY_W] = GL_FALSE;
	if (HandleKey(GLFW_KEY_S))
		if (!Player.ChangeDirection(Game5Snake::Direction::DOWN))
			this->KeysProcessed[GLFW_KEY_S] = GL_FALSE;
	if (HandleKey(GLFW_KEY_A))
		if (!Player.ChangeDirection(Game5Snake::Direction::LEFT))
			this->KeysProcessed[GLFW_KEY_A] = GL_FALSE;
	if (HandleKey(GLFW_KEY_D))
		if (!Player.ChangeDirection(Game5Snake::Direction::RIGHT))
			this->KeysProcessed[GLFW_KEY_D] = GL_FALSE;
}

void Game5::Update(GLfloat dt)
{
	// call base class method
	GameBase::Update(dt);

	// update game state, physics, collisions
	// keep the player inside of the screen

	if (game_menu_ || game_paused_)
	{

		return;
	}

	if (game_over_)
		return;

	MoveTimer += dt;

	if (MoveTimer > GameMoveSpeed)
	{
		MoveTimer -= GameMoveSpeed;
		Player.Move();
		DoCollisions();
	}
}

void Game5::Render()
{
	// clear background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (game_menu_)
	{
		TextRenderer.RenderText("Select difficulty: ", 1.0f * GAME_UNIT, (GAME_UNIT_HEIGHT + 0.0f) * GAME_UNIT, Projection, glm::vec3(Color::White), Alignment::NORMAL);
		for (GLuint i = 1; i <= 9; i++)
		{
			if (i == Difficulty)
			{
				Food.Position = glm::vec2(i * 2.0f, (GAME_UNIT_HEIGHT - 2.0f));
				Food.Render(this->Renderer, this->Projection, this->GAME_UNIT);
				TextRenderer.RenderText(std::to_string(i), 1.0f + i * 2.0f * GAME_UNIT, (GAME_UNIT_HEIGHT - 2.0f) * GAME_UNIT, Projection, glm::vec3(Color::Black), Alignment::NORMAL);
			}
			else
				TextRenderer.RenderText(std::to_string(i), 1.0f + i * 2.0f * GAME_UNIT, (GAME_UNIT_HEIGHT - 2.0f) * GAME_UNIT, Projection, glm::vec3(Color::White), Alignment::NORMAL);
		}

		TextRenderer.RenderText("Controls:", 2.0f * GAME_UNIT, 6.0f * GAME_UNIT, Projection, glm::vec3(Color::Grey50), Alignment::NORMAL);
		TextRenderer.RenderText("WASD - move.", 2.0f * GAME_UNIT, 5.0f * GAME_UNIT, Projection, glm::vec3(Color::Grey50), Alignment::NORMAL);
		TextRenderer.RenderText("Space - select/restart.", 2.0f * GAME_UNIT, 4.0f * GAME_UNIT, Projection, glm::vec3(Color::Grey50), Alignment::NORMAL);
		TextRenderer.RenderText("P - Pause.", 2.0f * GAME_UNIT, 3.0f * GAME_UNIT, Projection, glm::vec3(Color::Grey50), Alignment::NORMAL);
		TextRenderer.RenderText("F5 - Restart.", 2.0f * GAME_UNIT, 2.0f * GAME_UNIT, Projection, glm::vec3(Color::Grey50), Alignment::NORMAL);
		TextRenderer.RenderText("N - Next Song. ", 2.0f * GAME_UNIT, 1.0f * GAME_UNIT, Projection, glm::vec3(Color::Grey50), Alignment::NORMAL);

		return;
	}

	// render game objects
	Background1.Render(this->Renderer, this->Projection, this->GAME_UNIT);
	Background2.Render(this->Renderer, this->Projection, this->GAME_UNIT);

	Food.Render(this->Renderer, this->Projection, this->GAME_UNIT);
	Player.Render(this->Renderer, this->Projection, this->GAME_UNIT);

	std::string scoreText = std::to_string((Player.Length - 3) * Difficulty);
	TextRenderer.RenderText(scoreText, 1.0f * GAME_UNIT, (GAME_UNIT_HEIGHT + 0.0f) * GAME_UNIT, Projection, glm::vec3(Color::White), Alignment::NORMAL);

	if (epic_victory_)
	{
		WinBackground.Render(this->Renderer, this->Projection, this->GAME_UNIT);
		TextRenderer.RenderText("You have achieved EPIC victory.", GAME_UNIT_WIDTH / 2.0f * GAME_UNIT, (GAME_UNIT_HEIGHT / 2.0f - 0.5f) * GAME_UNIT, Projection, glm::vec3(Color::Black), Alignment::CENTER);
	}
	else if (game_over_)
	{
		TextRenderer.RenderText("You crashed. (press F5 or space)", GAME_UNIT_WIDTH / 2.0f * GAME_UNIT, (GAME_UNIT_HEIGHT / 2.0f - 0.5f) * GAME_UNIT, Projection, glm::vec3(Color::Grey50), Alignment::CENTER);
	}
	else if (game_paused_)
	{
		TextRenderer.RenderText("Paused.", GAME_UNIT_WIDTH / 2.0f * GAME_UNIT, 8.0f * GAME_UNIT, Projection, glm::vec3(Color::Grey50), Alignment::CENTER);

		TextRenderer.RenderText("Controls:", 2.0f * GAME_UNIT, 6.0f * GAME_UNIT, Projection, glm::vec3(Color::Grey50), Alignment::NORMAL);
		TextRenderer.RenderText("WASD - move.", 2.0f * GAME_UNIT, 5.0f * GAME_UNIT, Projection, glm::vec3(Color::Grey50), Alignment::NORMAL);
		TextRenderer.RenderText("Space - select/restart.", 2.0f * GAME_UNIT, 4.0f * GAME_UNIT, Projection, glm::vec3(Color::Grey50), Alignment::NORMAL);
		TextRenderer.RenderText("P - Pause.", 2.0f * GAME_UNIT, 3.0f * GAME_UNIT, Projection, glm::vec3(Color::Grey50), Alignment::NORMAL);
		TextRenderer.RenderText("F5 - Restart.", 2.0f * GAME_UNIT, 2.0f * GAME_UNIT, Projection, glm::vec3(Color::Grey50), Alignment::NORMAL);
		TextRenderer.RenderText("N - Next Song. ", 2.0f * GAME_UNIT, 1.0f * GAME_UNIT, Projection, glm::vec3(Color::Grey50), Alignment::NORMAL);
	}
	// call base class method (at the end. Otherwise it will be invisible. Order matters.)
	GameBase::Render();
}

// ------------------------------------------------------------------------- Helper Functions -------------------------------------------------------------------------

void Game5::DoCollisions()
{
	// pre
	std::deque<glm::vec2> data = Player.SnakeTilePositions;
	glm::vec2 head = data.back();
	data.pop_back();

	// body
	for (auto& var : data)
	{
		if (head == var)
			OnGameOver();
	}

	if (head.y > GAME_UNIT_HEIGHT - 2)
		OnGameOver();
	else if (head.y < 1)
		OnGameOver();
	else if (head.x > GAME_UNIT_WIDTH - 2)
		OnGameOver();
	else if (head.x < 1)
		OnGameOver();

	if (FoodCollision())
	{
		Player.Eat();
		MusicPlayer.PlaySound("Data/Audio/Game5/munch.ogg");
		if (Player.Length >= 220)
			OnWin();

		SpawnFood();
	}
}

GLboolean Game5::FoodCollision()
{
	for (auto& var : Player.SnakeTilePositions)
		if (Food.Position == var)
			return GL_TRUE;

	return GL_FALSE;
}

void Game5::SpawnFood()
{
	do
	{
		GLuint ry = 1 + (GLuint)(rand() % 10);
		GLuint rx = 1 + (GLuint)(rand() % 22);
		glm::vec2 pos(rx, ry);
		Food.Position = pos;
	} while (FoodCollision());
}

void Game5::SetDifficulty(GLuint hardness)
{
	if (hardness < 1 || hardness > 9)
		return;

	Difficulty = hardness;
	GameMoveSpeed = 0.5f - ((hardness) * 0.05f);
}

void Game5::PlayNextSong()
{
	if (++SongNum >= 2)
		SongNum = 0;

	switch (SongNum)
	{
	case 0:
		MusicPlayer.StopAllSounds();
		MusicPlayer.PlaySound("Data/Audio/Game5/bgm.mod", true);
		break;
	case 1:
		MusicPlayer.StopAllSounds();
		MusicPlayer.PlaySound("Data/Audio/Game5/bgm2.mod", true);
		break;
	default:
		break;
	}
}

void Game5::OnRestart()
{
	game_menu_ = GL_TRUE;
	game_over_ = GL_FALSE;
	epic_victory_ = GL_FALSE;
	game_paused_ = GL_FALSE;

	Player.Reset();
}

void Game5::OnWin()
{
	epic_victory_ = GL_TRUE;
	game_over_ = GL_TRUE;
}

void Game5::OnGameOver()
{
	MusicPlayer.PlaySound("Data/Audio/Game5/munch.ogg");

	game_over_ = GL_TRUE;

	Player.GameOver();
}

// ------------------------------------------------------------------------- Snake -------------------------------------------------------------------------

void Game5Snake::Init()
{
	Tile.Init("Data/Textures/Game5/white.png", glm::vec2(0), glm::vec2(1));
	Reset();
}

GLboolean Game5Snake::ChangeDirection(Direction dir)
{
	if (dirChanged)
	{
		return GL_FALSE;
	}
	if (FacingDirection.x)
	{
		if (dir == Direction::UP)
			Up();
		else if (dir == Direction::DOWN)
			Down();
	}
	else if (FacingDirection.y)
	{
		if (dir == Direction::RIGHT)
			Right();
		else if (dir == Direction::LEFT)
			Left();
	}
	return GL_TRUE;
}

glm::vec2 Game5Snake::Move()
{
	dirChanged = GL_FALSE;

	HeadPosition += FacingDirection;
	SnakeTilePositions.push_back(HeadPosition);
	if (SnakeTilePositions.size() > Length)
		SnakeTilePositions.pop_front();

	return HeadPosition;
}

void Game5Snake::Render(SpriteRenderer & renderer, glm::mat4 & projection, GLfloat gameUnit)
{
	Tile.Color = Color::White;

	for (auto& t : SnakeTilePositions)
	{
		Tile.Position = glm::vec2(t.x, t.y);
		Tile.Render(renderer, projection, gameUnit);
	}

	if (gameover_)
	{
		Tile.Color = Color::Red;
		Tile.Position = glm::vec2(SnakeTilePositions.back());
		Tile.Render(renderer, projection, gameUnit);
	}
}

void Game5Snake::Eat()
{
	Length++;
}

void Game5Snake::Reset()
{
	gameover_ = GL_FALSE;
	HeadPosition = glm::vec2(5, 5);
	FacingDirection = glm::ivec2(1, 0);
	Length = 3;
	SnakeTilePositions.clear();
	SnakeTilePositions.push_back(HeadPosition);
}

void Game5Snake::GameOver()
{
	gameover_ = GL_TRUE;
}
