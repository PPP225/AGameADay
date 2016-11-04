#include "Game4.h"

#include <glm/gtc/matrix_transform.hpp>

#include <time.h>

Game4::~Game4()
{
	DimScreen.Delete();

	Map.Delete();
	Background.Delete();

	HomePlanet.Delete();
	HomeSun.Delete();
	HomeCloseup.Delete();

	Player.Delete();

	for (auto& var : LeftEnemies)
		var.Delete();
	LeftEnemies.clear();
	for (auto& var : LeftBosses)
		var.Delete();

	for (auto& var : RightEnemies)
		var.Delete();
	RightEnemies.clear();
	for (auto& var : RightBosses)
		var.Delete();
	RightBosses.clear();


	if (Projectiles.size() > 0)
		Projectiles[0].Delete();
	for (auto& var : Projectiles)
		var.Delete();
	Projectiles.clear();

	if (PlayerProjectiles.size() > 0)
		PlayerProjectiles[0].Delete();
	for (auto& var : PlayerProjectiles)
		var.Delete();
	PlayerProjectiles.clear();
}

void Game4::Init()
{
	// call base class method
	GameBase::Init();

	// seed the random number generator 
	time_t timev;
	time(&timev);
	srand((unsigned int)timev);

	// set projection matrix to game coordinates
	this->Projection = glm::ortho(0.0f, (GLfloat)this->Width, 0.0f, (GLfloat)this->Height, -1.0f, 1.0f);

	// create game objects
	DimScreen.Init("Data/Textures/Game4/Home/dim.png", glm::vec2(0), glm::vec2(48, 27));

	Map.Init("Data/Textures/Game4/Home/map.png", glm::vec2(-1), glm::vec2(2));
	Background.Init("Data/Textures/Game4/Home/background.png", glm::vec2(BG_LEFT, 0.0f), glm::vec2(BG_RIGHT, 27.0f));

	HomePlanet.Init("Data/Textures/Game4/Home/earth.png", glm::vec2(HOME_X, 12), glm::vec2(2, 2));
	HomeSun.Init("Data/Textures/Game4/Home/sun.png", glm::vec2(HOME_X, -28), glm::vec2(32, 32));
	HomeCloseup.Init("Data/Textures/Game4/Home/home.png", glm::vec2(0, 0), glm::vec2(48, 27));

	glm::vec2 psize = glm::vec2(5.33f, 3.0f);
	//glm::vec2 hmin = glm::vec2(0.08f, 0.22f);	// hitbox proportions
	//glm::vec2 hmax = glm::vec2(0.67f, 0.55f);	// hitbox proportions
	glm::vec2 hmin = glm::vec2(0.15f, 0.3f);	// hitbox proportions
	glm::vec2 hmax = glm::vec2(0.5f, 0.4f);	// hitbox proportions
	Player.Init("Data/Textures/Game4/Home/player.png", glm::vec2(HOME_X, 6), psize, psize * hmin, psize * hmax, this->MusicPlayer);

	OnRestartGame();
}

// ------------------------------------------------------------ Game Loop ------------------------------------------------------------

void Game4::ProcessKeys(GLfloat dt)
{
	// call base class method
	GameBase::ProcessKeys(dt);

	// ckeck if player wants to restart
	if (this->Keys[GLFW_KEY_F5])
		OnRestartGame();

	// handle input depending on game state
	switch (state_)
	{
	case Game4::GameState::GAME_ACTIVE:
		// shooting
		if (this->Keys[GLFW_KEY_ENTER]) {
			Player.Shoot(dt, Game4Player::ProjectileType::ULTI, &PlayerProjectiles);
		}
		else if (this->Keys[GLFW_KEY_SPACE]) {
			Player.Shoot(dt, Game4Player::ProjectileType::DEFAULT, &PlayerProjectiles);
			this->KeysProcessed[GLFW_KEY_SPACE] = GL_TRUE;
		}
		else if (this->Keys[GLFW_KEY_LEFT_CONTROL]) {
			Player.Shoot(dt, Game4Player::ProjectileType::SPACED, &PlayerProjectiles);
		}
		// no break;

	case Game4::GameState::GAME_HOME:
		// moving
		if (HandleKey(GLFW_KEY_P))
			state_ = GameState::GAME_PAUSE;
		else if (HandleKey(GLFW_KEY_M))
			state_ = GameState::GAME_MAP;
		if (this->Keys[GLFW_KEY_W]) {
			Player.Move(dt, Game4Player::MoveDirection::UP);
		}
		if (this->Keys[GLFW_KEY_S]) {
			Player.Move(dt, Game4Player::MoveDirection::DOWN);
		}
		if (this->Keys[GLFW_KEY_A]) {
			Player.Move(dt, Game4Player::MoveDirection::LEFT);
		}
		if (this->Keys[GLFW_KEY_D]) {
			Player.Move(dt, Game4Player::MoveDirection::RIGHT);
		}
		break;

	case Game4::GameState::GAME_MAP:
		if (HandleKey(GLFW_KEY_M) || HandleKey(GLFW_KEY_P)) {
			if (Player.Position.x < 50)
				state_ = GameState::GAME_HOME;
			else
				state_ = GameState::GAME_ACTIVE;
		}
		break;

	case Game4::GameState::GAME_PAUSE:
		if (HandleKey(GLFW_KEY_P)) {
			if (Player.Position.x < 50)
				state_ = GameState::GAME_HOME;
			else
				state_ = GameState::GAME_ACTIVE;
		}
		else if (HandleKey(GLFW_KEY_M))
			state_ = GameState::GAME_MAP;
		break;

	case Game4::GameState::GAME_OVER:
	case Game4::GameState::GAME_WIN:
		if (HandleKey(GLFW_KEY_SPACE))
			OnRestartGame();
		break;
	default:
		break;
	}

}

//#include <iostream>

void Game4::Update(GLfloat dt)
{
	// call base class method
	GameBase::Update(dt);

	// update game state, physics, collisions
	// keep the player inside of the screen
	GLfloat gameUnitHeight = this->Height / GAME_UNIT;
	GLfloat gameUnitWidth = this->Width / GAME_UNIT;

	switch (state_)
	{
	case Game4::GameState::GAME_ACTIVE:
		total_time_ += dt;
		// check if player is at home
		if (CheckIfHome())
			return;
		UpdateGameActive(dt);

		home_again_ = GL_TRUE;
		home_again_time_ = 0.0f;
		break;

	case Game4::GameState::GAME_HOME:
		Player.Update(dt);

		if (Player.Position.x < 0) {
			state_ = GameState::GAME_ACTIVE;
			Player.Position.x = HOME_X - 4;
			Player.Position.y = 12;
		}
		else if (Player.Position.x > 48) {
			state_ = GameState::GAME_ACTIVE;
			Player.Position.x = HOME_X + 4;
			Player.Position.y = 12;
		}
		else if (Player.Position.y < 0) {
			state_ = GameState::GAME_ACTIVE;
			Player.Position.x = HOME_X;
			Player.Position.y = 12 - 4;
		}
		else if (Player.Position.y > 27) {
			state_ = GameState::GAME_ACTIVE;
			Player.Position.x = HOME_X;
			Player.Position.y = 12 + 4;
		}
		if (home_again_)
		{
			home_again_time_ += dt;
			if (home_again_time_ > 5.0f)
				OnVictory();
		}
		break;

	case Game4::GameState::GAME_MAP:
		break;

	case Game4::GameState::GAME_PAUSE:
		break;

	case Game4::GameState::GAME_OVER:
		break;

	case Game4::GameState::GAME_WIN:
		break;

	default:
		break;
	}

}

void Game4::Render()
{
	// clear background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	// render game objects
	switch (state_)
	{
	case GameState::GAME_ACTIVE:
		RenderGame();
		break;
	case GameState::GAME_HOME:
		RenderHome();
		break;
	case GameState::GAME_MAP:
		RenderMap();
		break;
	case GameState::GAME_PAUSE:
		RenderPause();
		break;
	case GameState::GAME_OVER:
		RenderGameover();
		break;
	case Game4::GameState::GAME_WIN:
		RenderWin();
		break;
	default:
		break;
	}

	// call base class method (at the end. Otherwise it will be invisible. Order matters.)
	GameBase::Render();
}

// ------------------------------------------------------------ Helper methods: Rendering ------------------------------------------------------------

void Game4::RenderGame()
{
	// update camera position / projection matrix.
	GLfloat left;
	GLfloat right;
	GLfloat playerCenterX = Player.GetCenter().x;

	switch (scenario_)
	{
	case Game4::BattleScenario::NORMAL:
		left = playerCenterX * GAME_UNIT - this->Width * Player.CameraViewProportion;
		right = playerCenterX * GAME_UNIT + this->Width * (1.0f - Player.CameraViewProportion);
		break;
	case Game4::BattleScenario::LEFTBOSS:
		left =  BOSS_L * GAME_UNIT;
		right = left + this->Width;
		break;
	case Game4::BattleScenario::LEFTMINIBOSS:
		left = MBOSS_L * GAME_UNIT;
		right = left + this->Width;
		break;
	case Game4::BattleScenario::RIGHTBOSS:
		left = BOSS_R * GAME_UNIT;
		right = left + this->Width;
		break;
	case Game4::BattleScenario::RIGHTMINIBOSS:
		left = MBOSS_R * GAME_UNIT;
		right = left + this->Width;
		break;
	default:
		break;
	}

	this->Projection = glm::ortho(left, right, 0.0f, (GLfloat)this->Height, -1.0f, 1.0f);

	// draw background, 3 times, so it scrolls nicely. Note: works only for positive X values.
	GLfloat bg1pos = ((GLint)(playerCenterX / BG_RIGHT) - 0.0f) * BG_RIGHT;
	this->Background.Position.x = bg1pos;
	this->Background.Render(this->Renderer, this->Projection, this->GAME_UNIT);
	GLfloat bg2pos = bg1pos + BG_RIGHT;
	this->Background.Position.x = bg2pos;
	this->Background.Render(this->Renderer, this->Projection, this->GAME_UNIT);
	GLfloat bg3pos = bg1pos - BG_RIGHT;
	this->Background.Position.x = bg3pos;
	this->Background.Render(this->Renderer, this->Projection, this->GAME_UNIT);

	// draw home
	HomePlanet.Render(this->Renderer, this->Projection, this->GAME_UNIT);
	HomeSun.Render(this->Renderer, this->Projection, this->GAME_UNIT);

	// draw enemies
	for (auto& var : LeftEnemies)
	{
		if (std::abs(var.Position.x - Player.Position.x) < 50)
			var.Render(this->Renderer, this->Projection, this->GAME_UNIT);
	}
	for (auto& var : RightEnemies)
	{
		if (std::abs(var.Position.x - Player.Position.x) < 50)
			var.Render(this->Renderer, this->Projection, this->GAME_UNIT);
	}

	// draw bosses
	LeftBossHP.Render(this->Renderer, this->Projection, this->GAME_UNIT);
	for (auto& var : LeftBosses)
	{
		if (std::abs(var.Position.x - Player.Position.x) < 50)
			var.Render(this->Renderer, this->Projection, this->GAME_UNIT);
	}

	RightBossHP.Render(this->Renderer, this->Projection, this->GAME_UNIT);
	for (auto& var : RightBosses)
	{
		if (std::abs(var.Position.x - Player.Position.x) < 50)
			var.Render(this->Renderer, this->Projection, this->GAME_UNIT);
	}

	// draw projectiles
	for (auto& var : Projectiles)
	{
		var.Render(this->Renderer, this->Projection, this->GAME_UNIT);
	}
	for (auto& var : PlayerProjectiles)
	{
		var.Render(this->Renderer, this->Projection, this->GAME_UNIT);
	}

	// draw player
	this->Player.Render(this->Renderer, this->Projection, this->GAME_UNIT);
}

void Game4::RenderHome()
{
	this->Projection = glm::ortho(0.0f, (GLfloat)this->Width, 0.0f, (GLfloat)this->Height, -1.0f, 1.0f);

	HomeCloseup.Render(this->Renderer, this->Projection, this->GAME_UNIT);
	//HomeCloseup.Render(this->Renderer, glm::mat4(), 1.0f);
	this->Player.Render(this->Renderer, this->Projection, this->GAME_UNIT);

	if (Player.Position == glm::vec2(15,10))
		this->VictoryPic.Render(this->Renderer, this->Projection, this->GAME_UNIT);
}

void Game4::RenderMap()
{
	// draw map. Note: not using projection matrix, which changes depending on player position. Map always renders the same way.
	this->Map.Render(this->Renderer, glm::mat4(), 1.0f);
}

void Game4::RenderPause()
{
	GLfloat left = Player.GetCenter().x * GAME_UNIT - this->Width / 2.0f;
	GLfloat right = Player.GetCenter().x * GAME_UNIT + this->Width * 0.5f;
	this->Projection = glm::ortho(left, right, 0.0f, (GLfloat)this->Height, -1.0f, 1.0f);

	// dim screen
	DimScreen.Position.x = left / GAME_UNIT;
	DimScreen.Render(this->Renderer, this->Projection, this->GAME_UNIT);

	this->TextRenderer.RenderText("GAME PAUSED", left + 480.0f, 270.0f, this->Projection, glm::vec3(1), Alignment::CENTER);
	this->TextRenderer.RenderText("(press p to unpause)", left + 480.0f, 240.0f, this->Projection, glm::vec3(1), Alignment::CENTER);
}

void Game4::RenderGameover()
{
	RenderGame();

	//this->Projection = glm::ortho(0.0f, (GLfloat)this->Width, 0.0f, (GLfloat)this->Height, -1.0f, 1.0f);

	// update camera position / projection matrix.
	GLfloat left = Player.GetCenter().x * GAME_UNIT - this->Width / 2.0f;
	GLfloat right = Player.GetCenter().x * GAME_UNIT + this->Width * 0.5f;
	this->Projection = glm::ortho(left, right, 0.0f, (GLfloat)this->Height, -1.0f, 1.0f);

	// dim screen
	DimScreen.Position.x = left / GAME_UNIT;
	DimScreen.Render(this->Renderer, this->Projection, this->GAME_UNIT);


	glm::vec3 white(1, 1, 1);

	for (GLuint i = 0; i < 4; i++)
	{
		this->TextRenderer.RenderText("Killed by: ", left + GOValues[i * 3 + 0], GOValues[i * 3 + 1], this->Projection, glm::vec3(GOValues[i * 3 + 2]), Alignment::CENTER);
	}
	this->TextRenderer.RenderText("(Space or F5 to restart)", left + GOValues[12], GOValues[13], this->Projection, white, Alignment::CENTER);

	if (this->KillType == 1)
		KilledByI.Render(this->Renderer, this->Projection, this->GAME_UNIT);
	else if (this->KillType == 2)
		KilledByB.Render(this->Renderer, this->Projection, this->GAME_UNIT);
	else if (this->KillType == 3)
		KilledByP.Render(this->Renderer, this->Projection, this->GAME_UNIT);

}

void Game4::RenderWin()
{
	VictoryPic.Render(this->Renderer, this->Projection, this->GAME_UNIT);

	std::string result = "Score: " + std::to_string(score_) + " | Enemies destroyed: " + std::to_string(enemies_) +
		" | Time: " + std::to_string(total_time_) + "s | Total Score: " + std::to_string(timed_score_);
	this->TextRenderer.RenderText(result, 10, 510, this->Projection, glm::vec3(1), Alignment::NORMAL);
}

// ------------------------------------------------------------ Helper methods: Other ------------------------------------------------------------

void Game4::OnRestartGame()
{
	this->MusicPlayer.StopAllSounds();
	this->MusicPlayer.PlaySound("Data/Audio/Game4/bgm1.mod");

	Player.Reset();

	state_ = GameState::GAME_HOME;
	scenario_ = Game4::BattleScenario::NORMAL;
	Player.Position.x = 15.0f;
	Player.Position.y = 10.0f;

	GenerateLeft();
	GenerateRight();

	home_again_ = GL_FALSE;
	home_again_time_ = 0.0f;
	total_time_ = 0.0f;
	score_ = 0;
	timed_score_ = 0;
	enemies_ = 0;

	bgmright_ = GL_FALSE;
	bgmleft_ = GL_FALSE;

	VictoryPic.Delete();
	VictoryPic.Init("Data/Textures/Game4/Story/intro.png", glm::vec2(0, 0), glm::vec2(48, 27));
}

void Game4::OnGameover()
{
	state_ = GameState::GAME_OVER;

	if (this->KillType == 1)
	{
		//KilledByI.Position.x = 10;
		KilledByI.Position.y = 13;
		KilledByI.Size *= 3;
		KilledByI.Rotation /= 2;
	}
	else if (this->KillType == 2)
	{
		//KilledByB.Position.x = 10;
		KilledByB.Position.y = 13;
		KilledByB.Size *= 2;
		KilledByB.Rotation /= 2;
	}
	else if (this->KillType == 3)
	{
		//KilledByP.Position.x = 10;
		KilledByP.Position.y = 13;
		KilledByP.Size *= 5;
		KilledByP.Rotation /= 2;
	}

	for (GLuint i = 0; i < 5; i++)
	{
		GOValues[i * 3 + 0] = 50.0f + (GLfloat)(rand()) / ((GLfloat)(RAND_MAX / (900.0f - 50.0f)));
		GOValues[i * 3 + 1] = 50.0f + (GLfloat)(rand()) / ((GLfloat)(RAND_MAX / (500.0f - 50.0f)));
		GOValues[i * 3 + 2] = 0.5f + (GLfloat)(rand()) / ((GLfloat)(RAND_MAX / (1.0f - 0.5f)));
	}
}

void Game4::OnVictory()
{
	state_ = GameState::GAME_WIN;

	// calculate score and enemies destroyed
	enemies_ += 56 - (LeftEnemies.size() + RightEnemies.size());
	score_ += enemies_ * 4;	// 112/side
	enemies_ += 2 - LeftBosses.size() + 9 - RightBosses.size();

	if (LeftBosses.size() == 0)
		score_ += 88 + 300;
	else if (LeftBosses.size() == 1)
		score_ += 88;

	if (RightBosses.size() == 0)
		score_ += 88 + 300;
	else if (RightBosses.size() == 1)
		score_ += 88;

	// bonus score for goals
	if (LeftEnemies.size() == 0)
		score_ += 25;
	if (RightEnemies.size() == 0)
		score_ += 25;
	if (LeftBosses.size() == 0 && RightBosses.size() == 0)
		score_ += 100;

	// calculate timed score
	timed_score_ = (GLuint)(score_ / total_time_ * 100.0f);

	// victory screen
	if (LeftBosses.size() == 0 && RightBosses.size() == 0)
		VictoryPic.Init("Data/Textures/Game4/Story/totalwin.png", glm::vec2(0, 0), glm::vec2(48, 27));
	else if (LeftBosses.size() == 0)
		VictoryPic.Init("Data/Textures/Game4/Story/rightwin.png", glm::vec2(0, 0), glm::vec2(48, 27));
	else if (RightBosses.size() == 0)
		VictoryPic.Init("Data/Textures/Game4/Story/leftwin.png", glm::vec2(0, 0), glm::vec2(48, 27));
	else
		VictoryPic.Init("Data/Textures/Game4/Story/fail.png", glm::vec2(0, 0), glm::vec2(48, 27));
}

void Game4::UpdateGameActive(GLfloat dt)
{
	// update music
	if (Player.Position.x < MBOSS_L + 55.0f && bgmleft_ == GL_FALSE)
	{
		bgmleft_ = GL_TRUE;
		this->MusicPlayer.StopSound("Data/Audio/Game4/bgm1.mod");
		this->MusicPlayer.PlaySound("Data/Audio/Game4/bgmleft.xm", true);
	}
	else if (Player.Position.x > MBOSS_R && bgmright_ == GL_FALSE)
	{
		bgmright_ = GL_TRUE;
		this->MusicPlayer.StopSound("Data/Audio/Game4/bgm1.mod");
		this->MusicPlayer.PlaySound("Data/Audio/Game4/bgmright.s3m", true);
	}

	// update player
	Player.Update(dt);

	if (Player.Position.y + Player.Size.y >= 27.0f)
		Player.Position.y = 27.0f - Player.Size.y;
	if (Player.Position.y < 0.0f)
		Player.Position.y = 0.0f;

	switch (scenario_)
	{
	case Game4::BattleScenario::NORMAL:
		if (Player.Position.x < BOSS_L)
			Player.Position.x = BOSS_L;
		if (Player.Position.x + Player.Size.x > BOSS_R + 48.0f)
			Player.Position.x = BOSS_R + 48.0f - Player.Size.x;

		break;
	case Game4::BattleScenario::LEFTBOSS:
		if (Player.Position.x < BOSS_L)
			Player.Position.x = BOSS_L;
		if (Player.Position.x + Player.Size.x > BOSS_L + 48.0f)
			Player.Position.x = BOSS_L + 48.0f - Player.Size.x;
		break;
	case Game4::BattleScenario::LEFTMINIBOSS:
		if (Player.Position.x < MBOSS_L)
			Player.Position.x = MBOSS_L;
		if (Player.Position.x + Player.Size.x > MBOSS_L + 48.0f)
			Player.Position.x = MBOSS_L + 48.0f - Player.Size.x;
		break;
	case Game4::BattleScenario::RIGHTBOSS:
		if (Player.Position.x < BOSS_R)
			Player.Position.x = BOSS_R;
		if (Player.Position.x + Player.Size.x > BOSS_R + 48.0f)
			Player.Position.x = BOSS_R + 48.0f - Player.Size.x;
		break;
	case Game4::BattleScenario::RIGHTMINIBOSS:
		if (Player.Position.x < MBOSS_R)
			Player.Position.x = MBOSS_R;
		if (Player.Position.x + Player.Size.x > MBOSS_R + 48.0f)
			Player.Position.x = MBOSS_R + 48.0f - Player.Size.x;
		break;
	default:
		break;
	}

	// update enemies
	for (auto& var : LeftEnemies)
	{
		GLfloat dist = std::abs(Player.Position.x - var.Position.x);
		if (dist < 50)
		{
			if (!var.active_)
				var.Activate();
			else
				var.Update(dt);

			// collisions
			glm::vec2 p1 = Player.Position + Player.HitboxMin;
			glm::vec2 p1s = Player.HitboxMax;
			glm::vec2 e1 = var.Position;
			glm::vec2 e1s = var.Size;
			if (p1.x < e1.x + e1s.x &&
				p1.x + p1s.x > e1.x &&
				p1.y < e1.y + e1s.y &&
				p1s.y + p1.y > e1.y)
			{
				// collision detected!
				KillType = 1;
				this->KilledByI = var;
				OnGameover();
				return;
			}
		}
	}

	for (auto& var : RightEnemies)
	{
		GLfloat dist = std::abs(Player.Position.x - var.Position.x);
		if (dist < 50)
		{
			if (!var.active_)
				var.Activate();
			else
				var.Update(dt);

			// collisions
			glm::vec2 p1 = Player.Position + Player.HitboxMin;
			glm::vec2 p1s = Player.HitboxMax;
			glm::vec2 e1 = var.Position;
			glm::vec2 e1s = var.Size;
			if (p1.x < e1.x + e1s.x &&
				p1.x + p1s.x > e1.x &&
				p1.y < e1.y + e1s.y &&
				p1s.y + p1.y > e1.y)
			{
				// collision detected!
				KillType = 1;
				this->KilledByI = var;
				OnGameover();
				return;
			}
		}
	}

	// bosses
	for (auto& var : LeftBosses)
	{
		GLfloat dist = std::abs(Player.Position.x - var.Position.x);
		if (dist < 45)
		{
			// change scenario
			if (var.GetAI() == 1)
				scenario_ = Game4::BattleScenario::LEFTMINIBOSS;
			else if (var.GetAI() == 2)
				scenario_ = Game4::BattleScenario::LEFTBOSS;

			var.Update(dt, &Projectiles);

			// collisions
			glm::vec2 p1 = Player.Position + Player.HitboxMin;
			glm::vec2 p1s = Player.HitboxMax;
			glm::vec2 e1 = var.Position + var.HitboxMin;
			glm::vec2 e1s = var.HitboxMax;
			if (p1.x < e1.x + e1s.x &&
				p1.x + p1s.x > e1.x &&
				p1.y < e1.y + e1s.y &&
				p1s.y + p1.y > e1.y)
			{
				// collision detected!
				this->MusicPlayer.PlaySound("Data/Audio/Game4/boom.ogg");

				KillType = 2;
				this->KilledByB = var;
				OnGameover();
				return;
			}
		}
	}

	for (auto& var : RightBosses)
	{
		GLfloat dist = std::abs(Player.Position.x - var.Position.x);
		if (dist < 45)
		{
			// change scenario
			if (var.GetAI() == 3 || var.GetAI() == 4)
				scenario_ = Game4::BattleScenario::RIGHTMINIBOSS;
			else if (var.GetAI() == 5)
				scenario_ = Game4::BattleScenario::RIGHTBOSS;


			var.Update(dt, &Projectiles);

			// collisions
			glm::vec2 p1 = Player.Position + Player.HitboxMin;
			glm::vec2 p1s = Player.HitboxMax;
			glm::vec2 e1 = var.Position + var.HitboxMin;
			glm::vec2 e1s = var.HitboxMax;
			if (p1.x < e1.x + e1s.x &&
				p1.x + p1s.x > e1.x &&
				p1.y < e1.y + e1s.y &&
				p1s.y + p1.y > e1.y)
			{
				// collision detected!
				this->MusicPlayer.PlaySound("Data/Audio/Game4/boom.ogg");

				KillType = 2;
				this->KilledByB = var;
				OnGameover();
				return;
			}
		}
	}

	// projectiles
	for (GLuint i = 0; i < Projectiles.size(); ++i)
	{
		auto& var = Projectiles[i];
		var.Update(dt);

		if (var.expired_) {
			this->Projectiles.erase(Projectiles.begin() + i);
			--i;
			break;
		}

		// collisions
		glm::vec2 p1 = Player.Position + Player.HitboxMin;
		glm::vec2 p1s = Player.HitboxMax;
		glm::vec2 e1 = var.Position;
		glm::vec2 e1s = var.Size;
		if (p1.x < e1.x + e1s.x &&
			p1.x + p1s.x > e1.x &&
			p1.y < e1.y + e1s.y &&
			p1s.y + p1.y > e1.y)
		{
			// collision detected!
			KillType = 3;
			this->KilledByP = var;
			OnGameover();
			return;
		}
	}

	for (GLuint i = 0; i < PlayerProjectiles.size(); ++i)
	{
		auto& var = PlayerProjectiles[i];
		var.Update(dt);

		if (var.expired_) {
			this->PlayerProjectiles.erase(PlayerProjectiles.begin() + i);
			--i;
			break;
		}

		if (Player.Position.x < HOME_X)
		{
			// left enemies

			for (GLuint j = 0; j < LeftEnemies.size(); ++j) {
				auto& enemy = LeftEnemies[j];

				GLfloat dist = std::abs(var.Position.x - enemy.Position.x);
				if (dist < 4)
				{
					glm::vec2 p1 = var.Position;
					glm::vec2 p1s = var.Size;
					glm::vec2 e1 = enemy.Position;
					glm::vec2 e1s = enemy.Size;
					if (p1.x < e1.x + e1s.x &&
						p1.x + p1s.x > e1.x &&
						p1.y < e1.y + e1s.y &&
						p1s.y + p1.y > e1.y)
					{
						// if hit, erase projectile
						this->PlayerProjectiles.erase(PlayerProjectiles.begin() + i);
						--i;
						if (enemy.TakeDamage(1))
						{
							// if enemy destroyed, erase enemy
							scenario_ = Game4::BattleScenario::NORMAL;
							this->LeftEnemies[j].Delete();
							this->LeftEnemies.erase(LeftEnemies.begin() + j);
							--j;
							break;
						}
						break;
					}
				}
			}

			//bosses
			for (GLuint j = 0; j < LeftBosses.size(); ++j) {
				auto& enemy = LeftBosses[j];

				GLfloat dist = std::abs(var.Position.x - enemy.Position.x);
				if (dist < 4)
				{
					glm::vec2 p1 = var.Position;
					glm::vec2 p1s = var.Size;
					glm::vec2 e1 = enemy.Position;
					glm::vec2 e1s = enemy.Size;
					if (p1.x < e1.x + e1s.x &&
						p1.x + p1s.x > e1.x &&
						p1.y < e1.y + e1s.y &&
						p1s.y + p1.y > e1.y)
					{
						// if hit, erase projectile
						this->PlayerProjectiles.erase(PlayerProjectiles.begin() + i);
						--i;
						if (enemy.TakeDamage(1))
						{
							// if enemy destroyed, erase enemy
							this->MusicPlayer.PlaySound("Data/Audio/Game4/boom.ogg");
							scenario_ = Game4::BattleScenario::NORMAL;
							this->LeftBosses[j].Delete();
							this->LeftBosses.erase(LeftBosses.begin() + j);
							--j;
							break;
						}
						// updateHP
						if (enemy.GetAI() == 2)	// if left boss
						{
							GLfloat percentageHPdestroyed = 1.0f - ((GLfloat)enemy.HP / enemy.MaxHP);
							LeftBossHP.Position.y = -27.0f + percentageHPdestroyed * 27.0f;
						}
						break;
					}
				}
				// check if should end explosion animation
				if (enemy.TakeDamage(0))
				{
					if (enemy.GetAI() == 2) // if main boss
					{
						this->MusicPlayer.StopSound("Data/Audio/Game4/bgmleft.xm");
						this->MusicPlayer.PlaySound("Data/Audio/Game4/bgm1.mod", true);
					}

					this->MusicPlayer.PlaySound("Data/Audio/Game4/boom.ogg");
					scenario_ = Game4::BattleScenario::NORMAL;
					this->LeftBosses[j].Delete();
					this->LeftBosses.erase(LeftBosses.begin() + j);
					--j;
					break;
				}
			}
		}
		else
		{
			// right enemies
			for (GLuint j = 0; j < RightEnemies.size(); ++j) {
				auto& enemy = RightEnemies[j];

				GLfloat dist = std::abs(var.Position.x - enemy.Position.x);
				if (dist < 4)
				{
					glm::vec2 p1 = var.Position;
					glm::vec2 p1s = var.Size;
					glm::vec2 e1 = enemy.Position;
					glm::vec2 e1s = enemy.Size;
					if (p1.x < e1.x + e1s.x &&
						p1.x + p1s.x > e1.x &&
						p1.y < e1.y + e1s.y &&
						p1s.y + p1.y > e1.y)
					{
						// if hit, erase projectile
						this->PlayerProjectiles.erase(PlayerProjectiles.begin() + i);
						--i;
						if (enemy.TakeDamage(1))
						{
							// if enemy destroyed, erase enemy
							scenario_ = Game4::BattleScenario::NORMAL;
							this->RightEnemies[j].Delete();
							this->RightEnemies.erase(RightEnemies.begin() + j);
							--j;
							break;
						}
						break;
					}
				}
			}

			//bosses
			for (GLuint j = 0; j < RightBosses.size(); ++j) {
				auto& enemy = RightBosses[j];

				GLfloat dist = std::abs(var.Position.x - enemy.Position.x);
				if (dist < 4)
				{
					glm::vec2 p1 = var.Position;
					glm::vec2 p1s = var.Size;
					glm::vec2 e1 = enemy.Position;
					glm::vec2 e1s = enemy.Size;
					if (p1.x < e1.x + e1s.x &&
						p1.x + p1s.x > e1.x &&
						p1.y < e1.y + e1s.y &&
						p1s.y + p1.y > e1.y)
					{
						// if hit, erase projectile
						this->PlayerProjectiles.erase(PlayerProjectiles.begin() + i);
						--i;
						if (enemy.TakeDamage(1))
						{
							if (enemy.GetAI() == 5) // if main boss
							{
								this->MusicPlayer.StopSound("Data/Audio/Game4/bgmright.s3m");
								this->MusicPlayer.PlaySound("Data/Audio/Game4/bgm1.mod", true);
							}
							// if enemy destroyed, erase enemy
							this->MusicPlayer.PlaySound("Data/Audio/Game4/boom.ogg");
							scenario_ = Game4::BattleScenario::NORMAL;
							this->RightBosses[j].Delete();
							this->RightBosses.erase(RightBosses.begin() + j);
							--j;
							break;
						}
						// updateHP
						if (enemy.GetAI() == 5)	// if right boss
						{
							GLfloat percentageHPdestroyed = 1.0f - ((GLfloat)enemy.HP / enemy.MaxHP);
							RightBossHP.Position.y = -27.0f + percentageHPdestroyed * 27.0f;
						}
						break;
					}
				}
				// check if should end explosion animation
				if (enemy.TakeDamage(0))
				{
					if (enemy.GetAI() == 5) // if main boss
					{
						this->MusicPlayer.StopSound("Data/Audio/Game4/bgmright.s3m");
						this->MusicPlayer.PlaySound("Data/Audio/Game4/bgm1.mod", true);
					}
					// if enemy destroyed, erase enemy
					this->MusicPlayer.PlaySound("Data/Audio/Game4/boom.ogg");
					scenario_ = Game4::BattleScenario::NORMAL;
					this->RightBosses[j].Delete();
					this->RightBosses.erase(RightBosses.begin() + j);
					--j;
					break;
				}
			}
		}

	}
}

void Game4::GenerateLeft()
{
	// clear
	for (auto& var : LeftEnemies)
		var.Delete();
	LeftEnemies.clear();
	for (auto& var : LeftBosses)
		var.Delete();

	LeftBosses.clear();
	// issues
	GLfloat left = 200.0f;
	GLfloat right = 300.0f;
	for (GLuint i = 1; i < 29; i++)
	{
		//GLuint rTex = rand() % 28 + 1;
		GLuint rTex = i;
		GLfloat rx = left + (GLfloat)(rand()) / ((GLfloat)(RAND_MAX / (right - left)));
		GLfloat ry = 5.0f + (GLfloat)(rand()) / ((GLfloat)(RAND_MAX / (22.0f - 5.0f)));
		
		std::string texPath = std::string("Data/Textures/Game4/Left/lk") + std::to_string(rTex) + ".png";
		Game4Issue enemy;
		enemy.Init(texPath.c_str(), glm::vec2(rx, ry), glm::vec2(6, 1));

		LeftEnemies.push_back(enemy);
	}

	// miniboss
	left = MBOSS_L;

	Game4Boss lmb; lmb.Init("Data/Textures/Game4/Left/leftminiboss.png", glm::vec2(left + 2.0f, 10.0f), glm::vec2(5, 5), 1, glm::vec2(0, 0), glm::vec2(3.75f, 5.0f), this->MusicPlayer);
	LeftBosses.push_back(lmb);
	// boss
	left = BOSS_L;

	Game4Boss lb; lb.Init("Data/Textures/Game4/Left/leftboss.png", glm::vec2(left + 2.0f, 10.0f), glm::vec2(7, 7), 2, glm::vec2(0, 0), glm::vec2(7, 7), this->MusicPlayer);
	LeftBosses.push_back(lb);

	LeftBossHP.Init("Data/Textures/Game4/Right/wall.png", glm::vec2(left, -27.0f), glm::vec2(48, 27));

}

void Game4::GenerateRight()
{
	// clear
	for (auto& var : RightEnemies)
		var.Delete();
	RightEnemies.clear();
	for (auto& var : RightBosses)
		var.Delete();
	RightBosses.clear();

	// issues
	GLfloat left = 500.0f;
	GLfloat right = 600.0f;
	for (GLuint i = 1; i < 29; i++)
	{
		//GLuint rTex = rand() % 28 + 1;
		GLuint rTex = i;
		GLfloat rx = left + (GLfloat)(rand()) / ((GLfloat)(RAND_MAX / (right - left)));
		GLfloat ry = 5.0f + (GLfloat)(rand()) / ((GLfloat)(RAND_MAX / (22.0f - 5.0f)));

		std::string texPath = std::string("Data/Textures/Game4/Right/rk") + std::to_string(rTex) + ".png";
		Game4Issue enemy;
		enemy.Init(texPath.c_str(), glm::vec2(rx, ry), glm::vec2(6, 1));

		RightEnemies.push_back(enemy);
	}

	// miniboss
	left = MBOSS_R;

	Game4Boss lmb; lmb.Init("Data/Textures/Game4/Right/rightminiboss1.png", glm::vec2(left + 46.0f - 3.0f, 1.0f), glm::vec2(3, 3), 3, glm::vec2(0, 0), glm::vec2(3, 3), this->MusicPlayer);
	RightBosses.push_back(lmb);
	lmb.Init("Data/Textures/Game4/Right/rightminiboss2.png", glm::vec2(left + 46.0f - 3.0f, 10.0f), glm::vec2(3, 3), 3, glm::vec2(0, 0), glm::vec2(3, 3), this->MusicPlayer);
	RightBosses.push_back(lmb);
	lmb.Init("Data/Textures/Game4/Right/rightminiboss3.png", glm::vec2(left + 46.0f - 3.0f, 23.0f), glm::vec2(3, 3), 3, glm::vec2(0, 0), glm::vec2(3, 3), this->MusicPlayer);
	RightBosses.push_back(lmb);

	lmb.Init("Data/Textures/Game4/Right/rightminiboss11.png", glm::vec2(left + 46.0f - 3.0f, 4.0f), glm::vec2(3, 3), 4, glm::vec2(0, 0), glm::vec2(3, 3), this->MusicPlayer);
	RightBosses.push_back(lmb);
	lmb.Init("Data/Textures/Game4/Right/rightminiboss12.png", glm::vec2(left + 46.0f - 3.0f, 7.0f), glm::vec2(3, 3), 4, glm::vec2(0, 0), glm::vec2(3, 3), this->MusicPlayer);
	RightBosses.push_back(lmb);
	lmb.Init("Data/Textures/Game4/Right/rightminiboss13.png", glm::vec2(left + 46.0f - 3.0f, 13.0f), glm::vec2(3, 3), 4, glm::vec2(0, 0), glm::vec2(3, 3), this->MusicPlayer);
	RightBosses.push_back(lmb);

	lmb.Init("Data/Textures/Game4/Right/rightminiboss21.png", glm::vec2(left + 46.0f - 3.0f, 16.0f), glm::vec2(3, 3), 4, glm::vec2(0, 0), glm::vec2(3, 3), this->MusicPlayer);
	RightBosses.push_back(lmb);
	lmb.Init("Data/Textures/Game4/Right/rightminiboss31.png", glm::vec2(left + 46.0f - 3.0f, 19.0f), glm::vec2(3, 3), 4, glm::vec2(0, 0), glm::vec2(3, 3), this->MusicPlayer);
	RightBosses.push_back(lmb);

	// boss
	left = BOSS_R;

	Game4Boss lb; lb.Init("Data/Textures/Game4/Right/rightboss.png", glm::vec2(left + 46.0f - 7.0f, 16.0f), glm::vec2(7, 7), 5, glm::vec2(0, 0), glm::vec2(7, 7), this->MusicPlayer);
	RightBosses.push_back(lb);

	RightBossHP.Init("Data/Textures/Game4/Right/emailspam.png", glm::vec2(left, -27.0f), glm::vec2(48, 27));

}

GLboolean Game4::CheckIfHome()
{
	// level is the size of 48x27

	// if too far, return
	if (std::abs(Player.Position.x - HomePlanet.Position.x) > 3.0f)
		return GL_FALSE;
	// else

	if (glm::distance(Player.GetCenter(), HomePlanet.GetCenter()) < 2.5f)
	{
		GLfloat playerx = Player.GetCenter().x;
		GLfloat playery = Player.GetCenter().y;
		GLfloat homex = HomePlanet.GetCenter().x;
		GLfloat homey = HomePlanet.GetCenter().y;
		GLfloat xratio = playerx - homex;
		GLfloat yratio = playery - homey;
		xratio /= 2.5f;
		yratio /= 2.5f;
		GLfloat newxpos = (xratio + 1.0f) * 0.5f * 48.0f;
		GLfloat newypos = (yratio + 1.0f) * 0.5f * 27.0f;
		Player.Position = glm::vec2(newxpos, newypos);

		state_ = GameState::GAME_HOME;
		return GL_TRUE;
	}

	return GL_FALSE;
}

GLboolean Game4::HandleKey(GLint key)
{
	if (this->Keys[key] && !this->KeysProcessed[key]) {
		this->KeysProcessed[key] = GL_TRUE;
		return GL_TRUE;
	}
	return GL_FALSE;
}

// ------------------------------------------------------------ Entities ------------------------------------------------------------

// -------------------------------- Issue --------------------------------

void Game4Issue::Activate()
{
	active_ = GL_TRUE;

	// select AI
	this->AI = (GLuint)(rand() % 3) + 1u;
	multipl_ = 0.3f + (GLfloat)(rand()) / ((GLfloat)(RAND_MAX / (2.3f - 0.3f)));
	lean_ = -2.0f + (GLfloat)(rand()) / ((GLfloat)(RAND_MAX / (2.0f + 2.0f)));

	this->HP = 3;

	this->InitPos = this->Position;
	time_ = 0;
	glm::vec4 red(1.0f, 0.2f, 0.2f, 1.0f);
	glm::vec4 green(0.2f, 1.0f, 0.2f, 1.0f);
	glm::vec4 blue(0.2f, 0.2f, 1.0f, 1.0f);

	switch (AI)
	{
	case 0:
		break;
	case 1:
		this->Color = red;
		break;
	case 2:
		this->Color = green;
		break;
	case 3:
		this->Color = blue;
		break;
	default:
		break;
	}
}

void Game4Issue::Update(GLfloat dt)
{
	switch (AI)
	{
	case 0:
		break;
	case 1:
		AIStatic(dt);
		break;
	case 2:
		AICircling(dt);
		break;
	case 3:
		AITeleporting(dt);
		break;
	default:
		break;
	}
}

GLboolean Game4Issue::TakeDamage(GLint dmg)
{
	this->HP -= dmg;
	if (HP <= 0)
		return GL_TRUE;
	return GL_FALSE;
}

void Game4Issue::AIStatic(GLfloat dt)
{
	return;
}

void Game4Issue::AICircling(GLfloat dt)
{
	time_ += dt * multipl_;
	GLfloat value = std::sin(time_);
	this->Position.y = this->InitPos.y + (value * 10.0f);
	this->Rotation = value * 15.0f * lean_;
}

void Game4Issue::AITeleporting(GLfloat dt)
{
	time_ += dt * multipl_;
	GLfloat value = std::sin(time_);
	if (value < 0)
		value = -1.0f;
	else
		value = 1.0f;
	this->Position.y = this->InitPos.y + (value * 5.0f);
	this->Rotation = value * 15.0f * lean_;
}

// -------------------------------- Boss --------------------------------

void Game4Boss::Init(const GLchar * path, glm::vec2 position, glm::vec2 size, GLuint ai, glm::vec2 hitboxMin, glm::vec2 hitboxMax, SoundEngine& musicPlayer)
{
	GameObjectBase::Init(path, position, size);
	this->AI = ai;
	velocity_.y = 10.0f;

	timer_ = 3;	// bonus moves at the start

	this->MusicPlayer = &musicPlayer;

	this->StartPos = position;
	this->HitboxMin = hitboxMin;
	this->HitboxMax = hitboxMax;

	this->ExplosionTexture = Texture2D::loadTextureFromFile("Data/Textures/Game4/Home/explosion.png", GL_TRUE);

	switch (AI)
	{
	case 0:
		break;
	case 1:
		this->ProjectileTexture = Texture2D::loadTextureFromFile("Data/Textures/Game4/Left/leftmissile3.png", GL_TRUE);
		this->MaxHP = 100;
		this->HP = 100;
		break;
	case 2:
		this->ProjectileTexture = Texture2D::loadTextureFromFile("Data/Textures/Game4/Left/leftmissile1.png", GL_TRUE);
		this->MaxHP = 250;
		this->HP = 250;
		break;
	case 3:
		this->ProjectileTexture = Texture2D::loadTextureFromFile("Data/Textures/Game4/Right/rightmissile1.png", GL_TRUE);
		this->MaxHP = 40;
		this->HP = 40;
		break;
	case 4:
		this->ProjectileTexture = Texture2D::loadTextureFromFile("Data/Textures/Game4/Right/rightmissile1.png", GL_TRUE);
		this->MaxHP = 15;
		this->HP = 15;
		break;
	case 5:
		this->ProjectileTexture = Texture2D::loadTextureFromFile("Data/Textures/Game4/Right/rightmissile1.png", GL_TRUE);
		this->MaxHP = 250;
		this->HP = 250;
		timer_ = 0;	// no bonus, rightleader op
		break;
	default:
		break;
	}
}

void Game4Boss::Delete()
{
	GameObjectBase::Delete();
	ExplosionTexture.Delete();

	// vram memory leak :D this is to avoid errors with the textures - projectiles live longer then bosses
	//ProjectileTexture.Delete();
}

inline void Game4Boss::Render(SpriteRenderer & renderer, glm::mat4 & projection, GLfloat gameUnit)
{
	if (this->TimeSinceExplosion == 0.0f)
	{
		GameObjectBase::Render(renderer, projection, gameUnit);
	}
	else
	{
		glm::vec2 size = glm::vec2(1.0f, 1.0f) * this->TimeSinceExplosion / 0.2f * 10.0f;
		glm::vec2 pos = this->GetCenter() - size / 2.0f;

		renderer.Draw(this->ExplosionTexture, projection, pos * gameUnit, size * gameUnit, this->Rotation, this->FlipTexture, this->Color);
	}
}

void Game4Boss::Update(GLfloat dt, std::vector<Game4Projectile>* projectiles)
{
	if (HP <= 0) {
		TimeSinceExplosion += dt;
		return;
	}
	
	switch (AI)
	{
	case 0:
		break;
	case 1:
		AI_L_M(dt, projectiles);
		break;
	case 2:
		AI_L_B(dt, projectiles);
		break;
	case 3:
		AI_R_M1(dt, projectiles);
		break;
	case 4:
		AI_R_M2(dt, projectiles);
		break;
	case 5:
		AI_R_B(dt, projectiles);
		break;
	default:
		break;
	}
}

GLboolean Game4Boss::TakeDamage(GLint dmg)
{
	this->HP -= dmg;
	if (HP <= 0 && TimeSinceExplosion > 0.2f)
		return GL_TRUE;
	return GL_FALSE;
}

void Game4Boss::AI_L_M(GLfloat dt, std::vector<Game4Projectile>* projectiles)
{
	timer_ += dt;

	// update
	this->Position += velocity_ * dt;

	// check if position is okay, and correct velocity
	if (this->Position.y > 24)
		velocity_.y = -10.0f;
	else if (this->Position.y < 0.0f)
		velocity_.y = 10.0f;
	else if (this->Position.x < StartPos.x)
		velocity_.x = 0.0f;
	else if (this->Position.x > StartPos.x + 20.0f)
		velocity_.x = -8.0f;

	// every second, draw a random number
	if (timer_ > 1.0f)
	{
		timer_ -= 1.0f;
		GLuint r = (GLuint)(rand() % 100);

		glm::vec2 pos = glm::vec2(this->Position.x + this->HitboxMax.x, this->Position.y + this->Size.y / 2.0f);
		glm::vec2 size = glm::vec2(3.0f, 0.5f);

		if (r < 1)
			velocity_.x = 20.0f;
		else if (r < 6)
			velocity_.x = 10.0f;
		else if (r < 8)
			velocity_.x = 8.0f;
		else if (r < 10)
			velocity_.x = 5.0f;
		else if (r < 40) {
			GLuint xspeed = 5 + (GLuint)(rand() % 6);	// 5-10
			GLuint yspeed = -3 + (GLuint)(rand() % 7);	// -3-3
			Game4Projectile pr;
			pr.Init(ProjectileTexture, pos, size, glm::vec2(xspeed, yspeed), 5.0f);
			projectiles->push_back(pr);
		}
		else if (r < 70) {
			GLuint xspeed = 5 + (GLuint)(rand() % 6);	// 5-10
			for (GLuint i = 0; i < 4; i++)
			{
				GLuint yspeed = -6 + (GLuint)(rand() % 13);	// -6-6
				Game4Projectile pr;
				pr.Init(ProjectileTexture, pos, size, glm::vec2(xspeed, yspeed), 5.0f);
				projectiles->push_back(pr);
			}
		}
		else if (r < 80) {
			GLuint xspeed = 5 + (GLuint)(rand() % 6);	// 5-10
			for (GLuint i = 0; i < 15; i++)
			{
				GLfloat yspeed = -3.0f + (GLfloat)(rand()) / ((GLfloat)(RAND_MAX / (3.0f - -3.0f)));	//-6-6
				Game4Projectile pr;
				pr.Init(ProjectileTexture, pos, size, glm::vec2(xspeed, yspeed), 5.0f);
				projectiles->push_back(pr);
			}
		}
		else if (r < 100)
			;
	}
}

void Game4Boss::AI_L_B(GLfloat dt, std::vector<Game4Projectile>* projectiles)
{
	timer_ += dt;

	// update
	this->Position += velocity_ * dt;

	// check if position is okay, and correct velocity
	if (this->Position.y > 21)
		velocity_.y = -10.0f;
	else if (this->Position.y < -1.0f)
		velocity_.y = 10.0f;
	else if (this->Position.x < StartPos.x)
		velocity_.x = 0.0f;
	else if (this->Position.x > StartPos.x + 20.0f)
		velocity_.x = -8.0f;

	// every 0.5 second, draw a random number
	if (timer_ > 0.5f)
	{
		timer_ -= 0.5f;
		GLuint r = (GLuint)(rand() % 100);

		glm::vec2 pos = glm::vec2(this->Position.x + this->HitboxMax.x, this->Position.y + this->Size.y / 2.0f);
		glm::vec2 size = glm::vec2(2.0f, 2.0f);

		if (r < 1)
			velocity_.x = 15.0f;
		else if (r < 3)
			velocity_.x = 9.0f;
		else if (r < 6)
			velocity_.x = 5.0f;
		else if (r < 10)
			velocity_.x = 2.0f;
		else if (r < 70) {
			GLfloat xspeed = 4.0f + (GLfloat)(rand()) / ((GLfloat)(RAND_MAX / (8.0f - 4.0f)));	// 4-8
			GLfloat yspeed = -6.0f + (GLfloat)(rand()) / ((GLfloat)(RAND_MAX / (6.0f - -6.0f)));	//-6-6
			Game4Projectile pr;
			pr.Init(ProjectileTexture, pos, size, glm::vec2(xspeed, yspeed), 5.0f);
			projectiles->push_back(pr);
		}
		else if (r < 80) {
			for (GLuint i = 0; i < 3; i++)
			{
				GLfloat xspeed = 4.0f + (GLfloat)(rand()) / ((GLfloat)(RAND_MAX / (8.0f - 4.0f)));	// 4-8
				GLfloat yspeed = -6.0f + (GLfloat)(rand()) / ((GLfloat)(RAND_MAX / (6.0f - -6.0f)));	//-6-6
				Game4Projectile pr;
				pr.Init(ProjectileTexture, pos, size, glm::vec2(xspeed, yspeed), 5.0f);
				projectiles->push_back(pr);
			}
		}
		else if (r < 87) {
			GLint xspeed = -1 + (GLuint)(rand() % 12);	// -1-11
			GLuint multiplier = 4 + (GLuint)(rand() % 4);
			for (GLuint i = 0; i < 10; i++)
			{
				GLfloat yspeed = -6.0f + (GLfloat)(rand()) / ((GLfloat)(RAND_MAX / (6.0f - -6.0f)));	//-6-6
				Game4Projectile pr;
				glm::vec2 velocity(xspeed, yspeed);
				pr.Init(ProjectileTexture, pos, size, glm::normalize(velocity) * (GLfloat)multiplier, 5.0f);
				projectiles->push_back(pr);
			}
		}
		else if (r < 100)
			;
	}
}

void Game4Boss::AI_R_M1(GLfloat dt, std::vector<Game4Projectile>* projectiles)
{
	timer_ += dt;

	// update
	this->Position += velocity_ * dt;

	// check if position is okay, and correct velocity
	if (this->Position.y > 24)
		velocity_.y = -velocity_.y;
	else if (this->Position.y < 0.0f)
		velocity_.y = -velocity_.y;
	else if (this->Position.x > StartPos.x)
		velocity_.x = -velocity_.x;
	else if (this->Position.x < StartPos.x - 45.0f)
		velocity_.x = -velocity_.x;

	if (velocity_.x > 18.0f)
		velocity_.x = 15.0f;
	else if (velocity_.x < -18.0f)
		velocity_.x = -15.0f;
	else if (velocity_.y > 18.0f)
		velocity_.y = 15.0f;
	else if (velocity_.y < -18.0f)
		velocity_.y = -15.0f;

	// every second, draw a random number
	if (timer_ > 0.5f)
	{
		timer_ -= 0.5f;
		GLuint r = (GLuint)(rand() % 100);

		glm::vec2 pos = glm::vec2(this->Position.x + this->HitboxMax.x, this->Position.y + this->Size.y / 2.0f);
		glm::vec2 size = glm::vec2(3.0f, 0.5f);

		if (r < 1)
			velocity_.x += 20.0f;
		else if (r < 2)
			velocity_.x -= 20.0f;
		else if (r < 3)
			velocity_.y += 20.0f;
		else if (r < 4)
			velocity_.y -= 20.0f;
		else if (r < 7)
			velocity_.x += 11.0f;
		else if (r < 10)
			velocity_.x -= 11.0f;
		else if (r < 11)
			velocity_.y += 10.0f;
		else if (r < 12)
			velocity_.y -= 10.0f;
		else if (r < 17)
			velocity_.x += 7.0f;
		else if (r < 22)
			velocity_.x -= 7.0f;
		else if (r < 25)
			velocity_.y += 5.0f;
		else if (r < 28)
			velocity_.y -= 5.0f;
		else if (r < 48)
			velocity_.x += 2.0f;
		else if (r < 68)
			velocity_.x -= 2.0f;
		else if (r < 88)
			velocity_.y += 1.0f;
		else if (r < 98)
			velocity_.y -= 1.0f;
	}
}

void Game4Boss::AI_R_M2(GLfloat dt, std::vector<Game4Projectile>* projectiles)
{
	timer_ += dt;

	// update
	this->Position += velocity_ * dt;

	// check if position is okay, and correct velocity
	if (this->Position.y > 24)
		velocity_.y = -velocity_.y;
	else if (this->Position.y < 0.0f)
		velocity_.y = -velocity_.y;
	else if (this->Position.x > StartPos.x)
		velocity_.x = -velocity_.x;
	else if (this->Position.x < StartPos.x - 45.0f)
		velocity_.x = -velocity_.x;

	if (velocity_.x > 18.0f)
		velocity_.x = 15.0f;
	else if (velocity_.x < -18.0f)
		velocity_.x = -15.0f;
	else if (velocity_.y > 18.0f)
		velocity_.y = 15.0f;
	else if (velocity_.y < -18.0f)
		velocity_.y = -15.0f;

	// every second, draw a random number
	if (timer_ > 1.0f)
	{
		timer_ -= 1.0f;
		GLuint r = (GLuint)(rand() % 100);

		glm::vec2 pos = glm::vec2(this->Position.x + this->HitboxMax.x, this->Position.y + this->Size.y / 2.0f);
		glm::vec2 size = glm::vec2(3.0f, 0.5f);

		if (r < 1)
			velocity_.x += 20.0f;
		else if (r < 2)
			velocity_.x -= 20.0f;
		else if (r < 3)
			velocity_.y += 20.0f;
		else if (r < 4)
			velocity_.y -= 20.0f;
		else if (r < 7)
			velocity_.x += 11.0f;
		else if (r < 10)
			velocity_.x -= 11.0f;
		else if (r < 11)
			velocity_.y += 10.0f;
		else if (r < 12)
			velocity_.y -= 10.0f;
		else if (r < 17)
			velocity_.x += 7.0f;
		else if (r < 22)
			velocity_.x -= 7.0f;
		else if (r < 25)
			velocity_.y += 5.0f;
		else if (r < 28)
			velocity_.y -= 5.0f;
		else if (r < 48)
			velocity_.x += 2.0f;
		else if (r < 68)
			velocity_.x -= 2.0f;
		else if (r < 88)
			velocity_.y += 1.0f;
		else if (r < 98)
			velocity_.y -= 1.0f;
	}

}

void Game4Boss::AI_R_B(GLfloat dt, std::vector<Game4Projectile>* projectiles)
{
	timer_ += dt;

	// update
	this->Position += velocity_ * dt;

	// check if position is okay, and correct velocity
	if (this->Position.y > 24.0f)
		velocity_.y = -15.0f;
	else if (this->Position.y < -4.0f)
		velocity_.y = 15.0f;
	else if (this->Position.x > StartPos.x)
		velocity_.x = 0.0f;
	else if (this->Position.x < StartPos.x - 10.0f)
		velocity_.x = 8.0f;

	// every second, draw a random number
	if (timer_ > 0.5f)
	{
		timer_ -= 0.5f;
		GLuint r = (GLuint)(rand() % 100);

		glm::vec2 pos = glm::vec2(this->Position.x + this->HitboxMin.x, this->Position.y + this->Size.y / 2.0f);
		glm::vec2 size = glm::vec2(6.0f, 2.0f);

		if (r < 80) {
			GLfloat xspeed = -(40.0f + (GLfloat)(rand()) / ((GLfloat)(RAND_MAX / (50.0f - 40.0f))));
			GLfloat yspeed = 0.0f;
			Game4Projectile pr;
			pr.Init(ProjectileTexture, pos, size, glm::vec2(xspeed, yspeed), 1.0f);
			projectiles->push_back(pr);
		}
		else if (r < 90)
			velocity_.x -= 20.0f;

	}
}


// -------------------------------- Player --------------------------------

void Game4Player::Init(const GLchar * path, glm::vec2 position, glm::vec2 size, glm::vec2 hitboxMin, glm::vec2 hitboxMax, SoundEngine& musicPlayer)
{
	GameObjectBase::Init(path, position, size);

	this->MusicPlayer = &musicPlayer;

	this->HitboxMin = hitboxMin;
	this->HitboxMax = hitboxMax;
	this->HitboxMinRightFacing = hitboxMin;
	this->HitboxMaxRightFacing = hitboxMax;

	this->ProjectileTexture1 = Texture2D::loadTextureFromFile("Data/Textures/Game4/Home/playermissile1.png", GL_TRUE);

	this->BossBattle = GL_TRUE;
}

void Game4Player::Move(GLfloat dt, MoveDirection dir)
{
	switch (dir)
	{
	case Game4Player::MoveDirection::UP:
		velocity_.y = MAX_V_Y;
		break;
	case Game4Player::MoveDirection::DOWN:
		velocity_.y = -MAX_V_Y;
		break;
	case Game4Player::MoveDirection::LEFT:
		if (BossBattle)
			velocity_.x = -MAX_V_X;
		else
			velocity_.x -= MAX_V_X * dt;
		break;
	case Game4Player::MoveDirection::RIGHT:
		if (BossBattle)
			velocity_.x = MAX_V_X;
		else
			velocity_.x += MAX_V_X * dt;
		break;
	default:
		break;
	}
}

void Game4Player::Shoot(GLfloat dt, ProjectileType dir, std::vector<Game4Projectile>* playerProjectiles)
{
	shot_cooldown_ -= dt;

	//don't shoot too often
	if (shot_cooldown_ > 0.0f)
		return;
	shot_cooldown_ += 0.1f;

	glm::vec2 pc = this->GetCenter();

	glm::vec2 pos = glm::vec2(this->Position.x, pc.y - 0.5f);
	if (this->FlipTexture == GL_FALSE)
		pos = glm::vec2(this->Position.x + this->Size.x, pc.y - 0.5f);


	if (dir == Game4Player::ProjectileType::DEFAULT)
	{
		glm::vec2 size = glm::vec2(1.0f, 1.0f);

		GLint xspeed = 32;
		if (this->FlipTexture)
			xspeed = -32;

		GLint yspeed = 0;
		if (this->Rotation > 0)
			yspeed = 2;
		else if (this->Rotation < 0)
			yspeed = -2;

		{
			Game4Projectile pr;
			pr.Init(ProjectileTexture1, pos, size, glm::vec2(xspeed, yspeed + 1), 1.1f);
			playerProjectiles->push_back(pr);
		}
		{
			Game4Projectile pr;
			pr.Init(ProjectileTexture1, pos, size, glm::vec2(xspeed, yspeed), 1.1f);
			playerProjectiles->push_back(pr);
		}
		{
			Game4Projectile pr;
			pr.Init(ProjectileTexture1, pos, size, glm::vec2(xspeed, yspeed - 1), 1.1f);
			playerProjectiles->push_back(pr);
		}

		this->MusicPlayer->PlaySound("Data/Audio/Game4/laser.ogg");

	}
	else if (dir == Game4Player::ProjectileType::SPACED)
	{
		glm::vec2 size = glm::vec2(1.0f, 1.0f);

		GLint xspeed = 4;
		if (this->FlipTexture)
			xspeed = -4;

		glm::vec2 tempVel1 = glm::normalize(glm::vec2(xspeed, 4));
		glm::vec2 tempVel2 = glm::normalize(glm::vec2(xspeed, 2));
		glm::vec2 tempVel3 = glm::normalize(glm::vec2(xspeed, 0));
		glm::vec2 tempVel4 = glm::normalize(glm::vec2(xspeed, -2));
		glm::vec2 tempVel5 = glm::normalize(glm::vec2(xspeed, -4));

		GLfloat speed = 32;


		{
			Game4Projectile pr;
			pr.Init(ProjectileTexture1, pos, size, tempVel1 * speed, 0.9f);
			playerProjectiles->push_back(pr);
		}
		{
			Game4Projectile pr;
			pr.Init(ProjectileTexture1, pos, size, tempVel2 * speed, 0.9f);
			playerProjectiles->push_back(pr);
		}
		{
			Game4Projectile pr;
			pr.Init(ProjectileTexture1, pos, size, tempVel3 * speed, 0.9f);
			playerProjectiles->push_back(pr);
		}
		{
			Game4Projectile pr;
			pr.Init(ProjectileTexture1, pos, size, tempVel4 * speed, 0.9f);
			playerProjectiles->push_back(pr);
		}
		{
			Game4Projectile pr;
			pr.Init(ProjectileTexture1, pos, size, tempVel5 * speed, 0.9f);
			playerProjectiles->push_back(pr);
		}

		this->MusicPlayer->PlaySound("Data/Audio/Game4/laser.ogg");

	}
	else if (dir == Game4Player::ProjectileType::ULTI)
	{
		if (ulti_ammo_ <= 0)
			return;

		ulti_ammo_ -= 1;

		glm::vec2 size = glm::vec2(1.0f, 1.0f);
		for (GLuint i = 0; i < 100; ++i)
		{
			GLfloat xspeed = -50.0f + (GLfloat)(rand()) / ((GLfloat)(RAND_MAX / (50.0f - -50.0f)));
			GLfloat yspeed = -50.0f + (GLfloat)(rand()) / ((GLfloat)(RAND_MAX / (50.0f - -50.0f)));

			Game4Projectile pr;
			pr.Init(ProjectileTexture1, pos, size, glm::vec2(xspeed, yspeed - 1), 1.2f);
			playerProjectiles->push_back(pr);
		}
		for (GLuint i = 0; i < 100; ++i)
		{
			GLfloat xspeed = 0.0f + (GLfloat)(rand()) / ((GLfloat)(RAND_MAX / (50.0f - 0.0f)));
			GLfloat yspeed = -10.0f + (GLfloat)(rand()) / ((GLfloat)(RAND_MAX / (10.0f - -10.0f)));

			if (this->FlipTexture)
				xspeed = -xspeed;

			Game4Projectile pr;
			pr.Init(ProjectileTexture1, pos, size, glm::vec2(xspeed, yspeed - 1), 1.2f);
			playerProjectiles->push_back(pr);
		}
		this->MusicPlayer->PlaySound("Data/Audio/Game4/laser.ogg");
	}


}

void Game4Player::Update(GLfloat dt)
{
	// make sure texture points the right direction
	if (velocity_.x < 0)
	{
		// check how long it's been (needed to update camera)
		this->CameraTimeLeft += dt;
		this->CameraTimeRight = 0;

		// update texture and hitbox
		this->FlipTexture = GL_TRUE;
		this->HitboxMin.x = this->Size.x - (this->HitboxMaxRightFacing.x + this->HitboxMinRightFacing.x);
		this->HitboxMax = this->HitboxMaxRightFacing;
	}
	else if (velocity_.x > 0)
	{
		this->CameraTimeLeft = 0;
		this->CameraTimeRight += dt;

		this->FlipTexture = GL_FALSE;
		this->HitboxMin = this->HitboxMinRightFacing;
		this->HitboxMax = this->HitboxMaxRightFacing;
	}

	// update camera proportions
	if (this->CameraTimeRight > 1.0f)
	{
		this->CameraViewProportion -= 0.1f * dt;
		if (this->CameraViewProportion < 0.2f)
			this->CameraViewProportion = 0.2f;
	}
	else if (this->CameraTimeLeft > 1.0f)
	{
		this->CameraViewProportion += 0.1f * dt;
		if (this->CameraViewProportion > 0.8f)
			this->CameraViewProportion = 0.8f;
	}


	// if going up or down, rotate a bit.
	if (velocity_.y == 0)
		this->Rotation = 0.0f;
	else if (velocity_.y > 0)
		this->Rotation = 8.0f;
	else
		this->Rotation = -8.0f;

	// update position
	this->Position += velocity_ * dt;

	// reset y v.
	velocity_.y = 0.0f;

	if (BossBattle)
		velocity_.x = 0.0f;
}

void Game4Player::Reset()
{
	ulti_ammo_ = 1;
}

// -------------------------------- Projectile --------------------------------

void Game4Projectile::Init(Texture2D& texture, glm::vec2 position, glm::vec2 size, glm::vec2 velocity, GLfloat timeToLive)
{
	GameObjectBase::Init(texture, position, size);

	velocity_ = velocity;
	time_ = timeToLive;
}

void Game4Projectile::Update(GLfloat dt)
{
	time_ -= dt;
	if (time_ < 0)
		expired_ = GL_TRUE;

	// update position
	this->Position += velocity_ * dt;
}
