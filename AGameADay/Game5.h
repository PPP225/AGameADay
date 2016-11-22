#pragma once
#include "GameBase.h"

#include <deque>

class Game5Snake
{
public:
	enum class Direction
	{
		UP, DOWN, LEFT, RIGHT,
	};

public:
	Game5Snake() = default;
	virtual ~Game5Snake() = default;

	void Init();

	std::deque<glm::vec2> SnakeTilePositions;

	glm::vec2 HeadPosition;
	glm::ivec2 FacingDirection;
	GLint Length;

	GameObjectBase Tile;
	GLboolean dirChanged = GL_FALSE;
	GLboolean gameover_ = GL_FALSE;

	// returns FALSE if direction changed already
	GLboolean ChangeDirection(Direction dir);
	glm::vec2 Move();
	void Render(SpriteRenderer & renderer, glm::mat4 & projection, GLfloat gameUnit);
	
	void Eat();
	void Reset();
	void GameOver();

private:
	void Up() { FacingDirection = glm::ivec2(0, 1); dirChanged = GL_TRUE; }
	void Down() { FacingDirection = glm::ivec2(0, -1); dirChanged = GL_TRUE; }
	void Right() { FacingDirection = glm::ivec2(1, 0); dirChanged = GL_TRUE; }
	void Left() { FacingDirection = glm::ivec2(-1, 0); dirChanged = GL_TRUE; }
};

class Game5 :
	public GameBase
{
public:
	Game5() = default;
	virtual ~Game5() = default;

	// Inherited via GameBase
	virtual void Init() override;
	virtual void ProcessKeys(GLfloat dt) override;
	virtual void Update(GLfloat dt) override;
	virtual void Render() override;

private:
	// matrix that transforms game coordinates into screen coordinates
	glm::mat4 Projection;

	// game objects
	Game5Snake Player;
	GameObjectBase Food;

	GameObjectBase Background1;
	GameObjectBase Background2;
	GameObjectBase WinBackground;

	GLuint SongNum;

	// misc
	GLuint Difficulty = 5;
	GLfloat GameMoveSpeed = 0.25f;
	GLfloat MoveTimer = 0.0f;

	// switches
	GLboolean game_menu_ = GL_TRUE;
	GLboolean game_paused_ = GL_FALSE;
	GLboolean game_over_ = GL_FALSE;
	GLboolean epic_victory_ = GL_FALSE;

	// constants
	const GLfloat GAME_UNIT = 40.0f;	// in pixels - 24x(12 + 2gui)
	const GLfloat GAME_UNIT_HEIGHT = 12;
	const GLfloat GAME_UNIT_WIDTH = 24;
	const GLfloat PLAYER_SPEED = 5.0f;

	// helper fx
	void DoCollisions();
	GLboolean FoodCollision();
	void SpawnFood();
	void SetDifficulty(GLuint hardness);
	void PlayNextSong();

	void OnRestart();
	void OnWin();
	void OnGameOver();
};
