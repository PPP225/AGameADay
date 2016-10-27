/**
*	Game:			Italo-Turkish Bomber Simulator 1911
*	Idea:			Assast
*	Inspired by:	"Wings of Fury" (Amiga)
*/

#pragma once
#include "GameBase.h"

#include <vector>

class Game1Plane : public GameObjectBase
{
public:
	virtual void Init(const GLchar *path, glm::vec2 position, glm::vec2 size = glm::vec2(1.0f, 1.0f))
	{
		GameObjectBase::Init(path, position, size);

		this->XSpeed = 10.0f;
		this->YSpeed = 0.0f;
		this->RegularAmmo = 4;
		this->InciAmmo = 2;


		this->Position.x = 2.0f;
		this->Position.y = 4.0f;
	};

	void ChangeSpeed(GLfloat xAcceleration, GLfloat yAcceleration) 
	{
		this->XSpeed += xAcceleration;
		if (this->XSpeed > max_x_speed_)
			this->XSpeed = max_x_speed_;
		else if (this->XSpeed < -max_x_speed_)
			this->XSpeed = -max_x_speed_;

		this->YSpeed += yAcceleration;
		if (this->YSpeed > max_y_speed_)
			this->YSpeed = max_y_speed_;
		else if (this->YSpeed < -max_y_speed_)
			this->YSpeed = -max_y_speed_;
	};

	void Update(GLfloat dt)
	{
		// update texture params
		// flip texture depending on the direction of the plane
		this->FlipTexture = this->XSpeed >= 0.0f ? GL_TRUE : GL_FALSE;

		// rotate depending on plane ySpeed (max. 30 degrees)
		if (this->YSpeed >= 0)
			this->Rotation = -this->YSpeed * 30.0f / max_y_speed_;
		else
			this->Rotation = -this->YSpeed * 30.0f / max_y_speed_;

		// change flight direction
		// TODO: here, or not here?

		// update position
		GLfloat finalYSpeed = YSpeed;
		// if plane goes too slow, it falls down
		if (XSpeed < falling_speed_ && XSpeed > 0)
			finalYSpeed -= falling_speed_ - XSpeed;
		else if (XSpeed > -falling_speed_ && XSpeed < 0)
			finalYSpeed -= falling_speed_ + XSpeed;

		this->Position += glm::vec2(XSpeed * dt, finalYSpeed * dt);

	};

	void Reset()
	{
		this->XSpeed = 10.0f;
		this->YSpeed = 0.0f;
		this->RegularAmmo = 4;
		this->InciAmmo = 2;


		this->Position.x = 2.0f;
		this->Position.y = 4.0f;
	}

	GLfloat XSpeed, YSpeed;
	GLuint RegularAmmo, InciAmmo;

	const GLfloat max_x_speed_ = 15.0f;
	const GLfloat max_y_speed_ = 4.0f;

private:
	const GLfloat falling_speed_ = 8.0f;
};

class Game1Grenade : public GameObjectBase
{
public:
	virtual void Init(const GLchar *path, glm::vec2 position, glm::vec2 size = glm::vec2(1.0f, 1.0f))
	{
		GameObjectBase::Init(path, position, size);

		this->TimeSinceExplosion = 0.0f;
		this->ExplosionTexture = Texture2D::loadTextureFromFile("Data/Textures/Game1/Explosion1.png", GL_TRUE);
		this->YSpeed = 0.0f;
		this->XSpeed = 0.0f;
	};
	virtual void Delete()
	{
		GameObjectBase::Delete();
		Texture.Delete();
	};
	virtual void Render(SpriteRenderer& renderer, glm::mat4& projection, GLfloat gameUnit)
	{
		if (this->TimeSinceExplosion == 0.0f)
		{
			GameObjectBase::Render(renderer, projection, gameUnit);
		}
		else
		{
			glm::vec2 size = glm::vec2(1.0f, 1.0f) * this->TimeSinceExplosion * explosion_size_;
			glm::vec2 pos = this->Position - size / 2.0f;

			renderer.Draw(this->ExplosionTexture, projection, pos * gameUnit, size * gameUnit, this->Rotation, this->FlipTexture, this->Color);
		}
	};
	GLfloat YSpeed, XSpeed;
	GLfloat TimeSinceExplosion;
	Texture2D ExplosionTexture;

	GLfloat explosion_radius_ = 1.9f;
private:
	const GLfloat explosion_size_ = 2 * explosion_radius_ / 0.15f;	// == 2 * radius / max time;
};

class Game1IncendiaryGrenade : public GameObjectBase
{
public:
	virtual void Init(const GLchar *path, glm::vec2 position, glm::vec2 size = glm::vec2(1.0f, 1.0f))
	{
		GameObjectBase::Init(path, position, size);

		this->TimeSinceExplosion = 0.0f;
		this->TimeSinceFireStarted = 0.0f;
		this->ExplosionTexture = Texture2D::loadTextureFromFile("Data/Textures/Game1/Explosion2.png", GL_TRUE);
		this->FireTexture = Texture2D::loadTextureFromFile("Data/Textures/Game1/Fire.png", GL_TRUE);
		this->YSpeed = 0.0f;
		this->XSpeed = 0.0f;
	};
	virtual void Delete()
	{
		GameObjectBase::Delete();
		Texture.Delete();
	};
	virtual void Render(SpriteRenderer& renderer, glm::mat4& projection, GLfloat gameUnit)
	{
		if (this->TimeSinceFireStarted == 0.0f) {
			if (this->TimeSinceExplosion == 0.0f)
			{
				GameObjectBase::Render(renderer, projection, gameUnit);
			}
			else
			{
				glm::vec2 size = glm::vec2(1.0f, 1.0f) * this->TimeSinceExplosion * explosion_size_;
				glm::vec2 pos = glm::vec2(this->Position.x - size.x / 2.0f, this->Position.y);

				renderer.Draw(this->ExplosionTexture, projection, pos * gameUnit, size * gameUnit, this->Rotation, this->FlipTexture, this->Color);
			}
		}
		else
		{
			GLfloat currentFireSize = (((GLint)(this->TimeSinceFireStarted / this->MaxBurnTime * 15.0f) % 3) + 3) / 5.0f; // result is 0.6, 0.8 or 1.0, depending on the time;
			glm::vec2 size = glm::vec2(1.0f * fire_xsize_, 5.0f * currentFireSize);
			glm::vec2 pos = glm::vec2(this->Position.x - size.x / 2.0f, this->Position.y);

			renderer.Draw(this->FireTexture, projection, pos * gameUnit, size * gameUnit, this->Rotation, this->FlipTexture, this->Color);
		}
	};
	GLfloat YSpeed, XSpeed;

	GLfloat TimeSinceExplosion;
	const GLfloat MaxExplosionTime = 0.15f;
	Texture2D ExplosionTexture;

	GLfloat TimeSinceFireStarted;
	const GLfloat MaxBurnTime = 2.0f;
	Texture2D FireTexture;

	GLfloat explosion_radius_ = 1.9f;
	GLfloat fire_xsize_ = 4.5f;
private:
	const GLfloat explosion_size_ = 2 * explosion_radius_ / MaxExplosionTime;	// == 2 * radius / max time;
};

class Game1EnemyObject : public GameObjectBase {
public:
	virtual void Render(SpriteRenderer& renderer, glm::mat4& projection, GLfloat gameUnit)
	{
		if (KIA == GL_FALSE)
			GameObjectBase::Render(renderer, projection, gameUnit);
		else
			; // do not render.
	};
	GLboolean KIA = GL_FALSE;
	GLint Points = 0;
};

class Game1 : public GameBase
{
public:
	Game1() = default;
	~Game1() = default;

	// Inherited via GameBase
	virtual void Init() override;
	virtual void ProcessKeys(GLfloat dt) override;
	virtual void Update(GLfloat dt) override;
	virtual void Render() override;

private:

	// points
	GLint Score;
	GLuint SoldiersKilled, SuppliesDestroyed;
	GLfloat GameUnitsScouted;

	// screen data
	const GLfloat GAME_UNIT = 60.0f;	// in pixels - 16x9 screen dimensions
	const GLfloat WidthUnits = 300.0f;
	const GLfloat HeightUnits = 9.0f;

	// matrix that transforms game coordinates into screen coordinates
	glm::mat4 Projection;
	// game objects
	Game1Plane Player;
	GameObjectBase Background;
	Game1Grenade Grenade;
	Game1IncendiaryGrenade InciGrenade;
	Game1EnemyObject ItalyFlag;
	Game1EnemyObject OttomanFlag;
	std::vector<Game1EnemyObject> Soldiers;
	std::vector<Game1EnemyObject> Supplies;

	// sound references
	irrklang::ISound* engineSound;


	// sound flags
	GLboolean SoundPlayingEffect = GL_FALSE;

	// helper functions
	void RestartGame();

	void GenerateEnemies();

	void DoRegularGrenadePhysics(GLfloat dt);
	void DoRegularGrenadeCollisions();

	void DoIncendiaryGrenadePhysics(GLfloat dt);
	void DoIncendiaryGrenadeCollisions();

	GLboolean Paused = GL_TRUE;
	void GameStart();

	GLboolean Win = GL_FALSE;
	GLboolean Crash = GL_FALSE;
	void GameOver_Victory();
	void GameOver_Crash();
	
};

