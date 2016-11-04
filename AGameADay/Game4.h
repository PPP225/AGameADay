#pragma once
#include "GameBase.h"

#include <vector>


// -------------------------------------------------- Entities --------------------------------------------------

class Game4Issue : public GameObjectBase {
public:
	void Activate();
	void Update(GLfloat dt);
	GLboolean TakeDamage(GLint dmg);

	GLboolean active_ = GL_FALSE;

private:
	GLint HP;
	glm::vec2 InitPos;
	GLfloat time_;
	GLfloat multipl_;
	GLfloat lean_;

	GLuint AI = 0;
	void AIStatic(GLfloat dt);
	void AICircling(GLfloat dt);
	void AITeleporting(GLfloat dt);
};

class Game4Projectile : public GameObjectBase {
public:
	virtual void Init(Texture2D& texture, glm::vec2 position, glm::vec2 size, glm::vec2 velocity, GLfloat timeToLive);
	void Update(GLfloat dt);
	GLboolean expired_ = GL_FALSE;
private:
	glm::vec2 velocity_ = glm::vec2(0, 0);
	GLfloat time_;
};

class Game4Boss : public GameObjectBase {
public:
	virtual void Init(const GLchar *path, glm::vec2 position, glm::vec2 size, GLuint ai, glm::vec2 hitboxMin, glm::vec2 hitboxMax, SoundEngine& musicPlayer);
	virtual void Delete();
	virtual void Render(SpriteRenderer& renderer, glm::mat4& projection, GLfloat gameUnit);
	void Update(GLfloat dt, std::vector<Game4Projectile>* projectiles);
	
	GLboolean TakeDamage(GLint dmg);

	glm::vec2 HitboxMin;
	glm::vec2 HitboxMax;

	glm::vec2 StartPos;

	GLuint GetAI() { return this->AI; };
	GLint MaxHP;
	GLint HP;

private:
	glm::vec2 velocity_ = glm::vec2(0, 0);
	GLfloat timer_;
	GLfloat TimeSinceExplosion = 0.0f;
	Texture2D ProjectileTexture;
	Texture2D ExplosionTexture;

	GLuint AI = 0;
	void AI_L_M(GLfloat dt, std::vector<Game4Projectile>* projectiles);
	void AI_L_B(GLfloat dt, std::vector<Game4Projectile>* projectiles);
	void AI_R_M1(GLfloat dt, std::vector<Game4Projectile>* projectiles);
	void AI_R_M2(GLfloat dt, std::vector<Game4Projectile>* projectiles);
	void AI_R_B(GLfloat dt, std::vector<Game4Projectile>* projectiles);

	SoundEngine* MusicPlayer = nullptr;	// pointer to the engine, since we want only one engine.
};

class Game4Player : public GameObjectBase {
public:
	enum class MoveDirection {
		UP, DOWN, LEFT, RIGHT,
	};
	enum class ProjectileType {
		DEFAULT, SPACED, ULTI,
	};
public:
	virtual void Init(const GLchar *path, glm::vec2 position, glm::vec2 size, glm::vec2 hitboxMin, glm::vec2 hitboxMax, SoundEngine& musicPlayer);
	void Move(GLfloat dt, MoveDirection dir);
	void Shoot(GLfloat dt, ProjectileType dir, std::vector<Game4Projectile>* playerProjectiles);
	void Update(GLfloat dt);
	void Reset();

	glm::vec2 HitboxMin;
	glm::vec2 HitboxMax;

	GLboolean BossBattle;
	GLfloat CameraViewProportion = 0.5f;

private:
	// hb
	glm::vec2 HitboxMinRightFacing;
	glm::vec2 HitboxMaxRightFacing;

	// v
	glm::vec2 velocity_ = glm::vec2(0, 0);
	GLfloat MAX_V_X = 25.0f;
	GLfloat MAX_V_Y = 15.0f;

	//cam
	GLfloat CameraTimeLeft = 0.0f;
	GLfloat CameraTimeRight = 0.0f;

	// shooting things
	Texture2D ProjectileTexture1;
	GLfloat shot_cooldown_ = 0.0f;
	GLuint ulti_ammo_ = 1;

	// sound
	SoundEngine* MusicPlayer = nullptr;	// pointer to the engine, since we want only one engine.
};

// -------------------------------------------------- Game --------------------------------------------------

class Game4 :
	public GameBase
{
public:
	enum class GameState {
		GAME_ACTIVE,	// game is going on
		GAME_HOME,		// player is on his home planet
		GAME_MAP,		// player views map
		GAME_PAUSE,		// paused
		GAME_OVER,		// game ended
		GAME_WIN		// game ended with victory
	};
	enum class BattleScenario {
		NORMAL,
		LEFTBOSS,
		LEFTMINIBOSS,
		RIGHTBOSS,
		RIGHTMINIBOSS
	};

public:
	Game4() = default;
	virtual ~Game4();

	// Inherited via GameBase
	virtual void Init() override;
	virtual void ProcessKeys(GLfloat dt) override;
	virtual void Update(GLfloat dt) override;
	virtual void Render() override;

private:
	// matrix that transforms game coordinates into screen coordinates
	glm::mat4 Projection;
	GameState state_;
	BattleScenario scenario_ = BattleScenario::NORMAL;

	// game objects
	GameObjectBase Map;
	GameObjectBase Background;
	GameObjectBase DimScreen;

	GameObjectBase HomeSun;
	GameObjectBase HomePlanet;
	GameObjectBase HomeCloseup;

	Game4Player Player;

	std::vector<Game4Issue> LeftEnemies;
	std::vector<Game4Boss> LeftBosses;
	std::vector<Game4Issue> RightEnemies;
	std::vector<Game4Boss> RightBosses;

	std::vector<Game4Projectile> Projectiles;
	std::vector<Game4Projectile> PlayerProjectiles;

	GameObjectBase LeftBossHP;
	GameObjectBase RightBossHP;

	GameObjectBase VictoryPic;

	// gameover vars
	GLuint KillType = 0;
	Game4Issue KilledByI;
	Game4Boss KilledByB;
	Game4Projectile KilledByP;
	GLfloat GOValues[15];
	GLboolean home_again_ = GL_FALSE;
	GLfloat home_again_time_ = 0.0f;
	// win vars
	GLfloat total_time_ = 0.0f;
	GLuint score_ = 0;
	GLuint timed_score_ = 0;
	GLuint enemies_ = 0;

	// music vars
	GLboolean bgmright_ = GL_FALSE;
	GLboolean bgmleft_ = GL_FALSE;

	// constants
	const GLfloat GAME_UNIT = 20.0f;	// in pixels, 48x27
	const GLfloat PLAYER_SPEED = 25.0f;
	const GLfloat BG_LEFT = 0.0f;
	const GLfloat BG_RIGHT = 1024.0f / this->Width * 48.0f;	// bg image size is 1024, game width is 960, 48 is unit width
	const GLfloat HOME_X = 400.0f;

	const GLfloat BOSS_L = 100.0f;
	const GLfloat MBOSS_L = 150.0f;
	const GLfloat BOSS_R = 700.0f;
	const GLfloat MBOSS_R = 650.0f;

	// render helper functions
	void RenderGame();
	void RenderHome();
	void RenderMap();
	void RenderPause();
	void RenderGameover();
	void RenderWin();

	// update helper functions
	void UpdateGameActive(GLfloat dt);

	// other helper functions
	void OnRestartGame();
	void OnGameover();
	void OnVictory();

	void GenerateLeft();
	void GenerateRight();
	GLboolean CheckIfHome();

	// deeper helper functions
	GLboolean HandleKey(GLint key);
};
