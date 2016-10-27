#pragma once
#include "GameBase.h"

class Game0 : public GameBase
{
public:
	Game0() = default;
	~Game0() = default;

	// Inherited via GameBase
	virtual void Init() override;
	virtual void ProcessKeys(GLfloat dt) override;
	virtual void Update(GLfloat dt) override;
	virtual void Render() override;

private:
	// matrix that transforms game coordinates into screen coordinates
	glm::mat4 Projection;

	// game objects
	GameObjectBase Player;

	// switches
	GLboolean game_paused_ = GL_FALSE;

	// constants
	const GLfloat GAME_UNIT = 120.0f;	// in pixels
	const GLfloat PLAYER_SPEED = 5.0f;
};
