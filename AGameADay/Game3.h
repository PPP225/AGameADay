/**
*	Game:			Basic Tetris
*	Idea:			d3m4n
*	Inspired by:	Tetris
*/

#pragma once
#include "GameBase.h"
#include <vector>

// declarations
struct Game3LevelContainer;

// enums
enum class EmplaceEvent {
	NONE,
	ROW1,
	ROW2,
	ROW3,
	ROW4,
	GAME,
};

// Stone ---------------------------------------------------------------------------------------

class Game3Stone : public GameObjectBase {
public:
	void Init(Texture2D & texture, SoundEngine& musicPlayer);	// overloads the other Init methods, so they are now hidden.
	virtual void Render(SpriteRenderer& renderer, glm::mat4& projection, GLfloat gameUnit, glm::vec2 levelPosition);
	void RenderNext(SpriteRenderer& renderer, glm::mat4& projection, GLfloat gameUnit, glm::vec2 position);

	void Spawn();
	static glm::vec4 GetTypeColor(GLbyte type);

	void MoveLeft(Game3LevelContainer& level);
	void MoveRight(Game3LevelContainer& level);
	// returns Event
	EmplaceEvent MoveDown(Game3LevelContainer& level);
	void RotateRight(Game3LevelContainer& level);
	void RotateLeft(Game3LevelContainer& level);

	GLboolean Collision(Game3LevelContainer& level);

	glm::ivec2 Position;	// position of stone (override)
	GLbyte Type;	// empty, I, T, L, J, S, Z, O
	GLbyte Rotation;	// Rotation. 0, 1, 2, 3. (override)

private:
	SoundEngine* MusicPlayer = nullptr;	// pointer to the engine, since we want only one engine.
	GLbyte next_;
};

// Level ---------------------------------------------------------------------------------------

struct Game3LevelRow { 
	friend struct Game3LevelContainer;
	friend class Game3Level;

public:
	Game3LevelRow();
	~Game3LevelRow() = default;

	GLboolean Full();

private:
	std::vector<GLbyte> data_;

	GLbyte& operator[](GLuint idx);
};

struct Game3LevelContainer {
	friend class Game3Level;

public:
	Game3LevelContainer();
	~Game3LevelContainer() = default;

	GLbyte Get(GLint row, GLint column);
	// returns Event what happened
	EmplaceEvent Emplace(GLuint x1, GLuint x2, GLuint x3, GLuint x4, GLuint y1, GLuint y2, GLuint y3, GLuint y4, GLbyte type);
	EmplaceEvent ClearFullRows();
	void ClearAll();

private:
	std::vector<Game3LevelRow> data_;
	const GLuint size_ = 20;
};

class Game3Level : public GameObjectBase {
public:
	Game3Level();
	~Game3Level() = default;

	void Init(Texture2D & blockTexture, Texture2D & backgroundTexture, glm::vec2 levelTexturePosition, glm::vec2 levelTextureSize);	// overloads the other `Init` methods, so they are now hidden.
	virtual void Render(SpriteRenderer& renderer, glm::mat4& projection, GLfloat gameUnit, glm::vec2 levelPosition);

	Game3LevelContainer& GetData();
	void Clear();

private:
	Game3LevelContainer data_;

	glm::vec2 LevelTexPosition;
	glm::vec2 LevelTexSize;
	Texture2D BackgroundTexture;
};

// Game ---------------------------------------------------------------------------------------

class Game3 :
	public GameBase
{
public:
	Game3() = default;
	~Game3() = default;

	// Inherited via GameBase
	virtual void Init() override;
	virtual void ProcessKeys(GLfloat dt) override;
	virtual void Update(GLfloat dt) override;
	virtual void Render() override;

private:
	// matrix that transforms game coordinates into screen coordinates
	glm::mat4 Projection;

	// game objects
	GameObjectBase Background;
	Game3Level Level;
	Game3Stone Stone;

	// score
	GLint Score;
	GLint LevelScore; 	// used to control level progress.
	GLint LevelNum;

	// time things
	GLfloat TimeElapsed;
	GLfloat LevelSpeed;

	// switches
	GLboolean game_paused_ = GL_TRUE;
	GLboolean game_over_ = GL_FALSE;

	// constants
	const GLfloat GAME_UNIT = 20.0f;	// in pixels, 48x27
	const glm::vec2 LEVEL_POSITION = glm::vec2(19, 2);
	const GLint POINTS_PER_LEVEL = 100;

	// helper classes
	void RestartGame();
	void LevelAdvancement();
	void AddScore(EmplaceEvent e);

	void RenderRegularBackground();
	void RenderPauseBackground();
	void RenderGameoverBackground();
};

