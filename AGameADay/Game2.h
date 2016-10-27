/**
*	Game:			Pong
*	Idea:			PPP225
*	Inspired by:	Pong
*/

#pragma once
#include "GameBase.h"

class Game2Object : public GameObjectBase
{
public:
	glm::vec2 Velocity;
};


enum GameState {
	GAME_ACTIVE,	// game is going on
	GAME_PAUSE,		// game paused
	GAME_OVER		// game ended
};

class Game2 : public GameBase
{
public:
	Game2() = default;
	~Game2() = default;

	// Inherited via GameBase
	virtual void Init() override;
	virtual void ProcessKeys(GLfloat dt) override;
	virtual void Update(GLfloat dt) override;
	virtual void Render() override;

private:
	// game state
	GameState State;
	// matrix that transforms game coordinates into screen coordinates
	glm::mat4 Projection;

	// how many points does each player have
	GLuint	Points1, Points2;
	// did P1 get the last point, or not?
	GLboolean lastPoint1;

	// objects
	Game2Object Player1;
	Game2Object Player2;
	Game2Object Ball;

	// switches
	GLboolean AI = GL_FALSE;

	// constants
	const GLfloat GAME_UNIT = 1.0f;	// in pixels - 960x540 screen dimensions
	const GLfloat PADDLE_VELOCITY = 500.0f;
	const glm::vec2 BALL_INIT_VELOCITY = glm::vec2(300, -100);
	const glm::vec2 PADDLE_SIZE = glm::vec2(20, 100);
	const glm::vec2 BALL_SIZE = glm::vec2(20, 20);
	const GLuint MAX_SCORE = 10;

	// functions
	// collisions
	void ResolveCollisions();
	void IncreaseBallSpeed();

	// state change
	void RestartGame();

	// helper functions
	void RenderGameBackground();
	void RenderPauseBackground();
	void RenderGameoverBackground();
};

