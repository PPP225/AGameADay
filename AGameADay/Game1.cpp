#include "Game1.h"

#include <glm/gtc/matrix_transform.hpp>

void Game1::Init()
{
	GameBase::Init();

	this->Projection = glm::ortho(0.0f, (GLfloat)this->Width, 0.0f, (GLfloat)this->Height, -1.0f, 1.0f);

	Player.Init("Data/Textures/Game1/Plane.png", glm::vec2(0.0f, 0.0f), glm::vec2(2.92f * 1.1f, 1.1f));
	Background.Init("Data/Textures/Game1/BG.png", glm::vec2(0.0f, 0.0f), glm::vec2(this->WidthUnits, this->HeightUnits));
	Grenade.Init("Data/Textures/Game1/Grenade.png", glm::vec2(-1.0f, -1.0f), glm::vec2(0.3f, 0.3f));
	InciGrenade.Init("Data/Textures/Game1/Grenade.png", glm::vec2(-1.0f, -1.0f), glm::vec2(0.3f, 0.3f));
	ItalyFlag.Init("Data/Textures/Game1/ItalyFlag.png", glm::vec2(50.0f, 1.0f), glm::vec2(1.0f, 3.0f));
	ItalyFlag.Points = -99998;

	GenerateEnemies();

	this->Score = 0;
	SoldiersKilled = 0;
	SuppliesDestroyed = 0;
	GameUnitsScouted = 0;

	//this->MusicPlayer.PlaySound("Data/Audio/Game1/bgm.flac", true);
}

void Game1::ProcessKeys(GLfloat dt)
{
	// call base class method
	GameBase::ProcessKeys(dt);

	if (Paused)
	{
		if (this->Keys[GLFW_KEY_W] || this->Keys[GLFW_KEY_A] || this->Keys[GLFW_KEY_S] || this->Keys[GLFW_KEY_D]) {
			this->Paused = GL_FALSE;

			// start engine sound, and keep a reference for future volume change
			engineSound = this->MusicPlayer.PlaySound("Data/Audio/Game1/engine.ogg", true);
		}
	}
	else
	{
		GLfloat accMultiplier = 5.0f;

		// handle wsad input
		if (this->Keys[GLFW_KEY_W])
			Player.ChangeSpeed(0.0f, dt * accMultiplier);
		if (this->Keys[GLFW_KEY_S])
			Player.ChangeSpeed(0.0f, -dt * accMultiplier);
		if (this->Keys[GLFW_KEY_A])
			Player.ChangeSpeed(-dt * accMultiplier, 0.0f);
		if (this->Keys[GLFW_KEY_D])
			Player.ChangeSpeed(dt * accMultiplier, 0.0f);

		if (this->Keys[GLFW_KEY_SPACE]) {
			// throw grenade, if any ammo left
			if (Player.RegularAmmo > 0 && Grenade.Position.y == -1.0f)
			{
				// spawn in at player location
				Grenade.Position = Player.GetCenter();
				// set the horizontal speed
				Grenade.XSpeed = Player.XSpeed;
				// set the vertical speed
				Grenade.YSpeed = 7.0f;
				// change ammo count
				Player.RegularAmmo -= 1;
			}
		}
		if (this->Keys[GLFW_KEY_ENTER]) {
			// throw incendiary grenade
			if (Player.InciAmmo > 0 && InciGrenade.Position.y == -1.0f)
			{
				// spawn in at player location
				InciGrenade.Position = Player.GetCenter();
				// set the horizontal speed
				InciGrenade.XSpeed = Player.XSpeed;
				// set the vertical speed
				InciGrenade.YSpeed = 5.0f;
				// change ammo count
				Player.InciAmmo -= 1;
			}
		}


		if (this->Keys[GLFW_KEY_F5])
		{
			RestartGame();
		}

	}
}

void Game1::Update(GLfloat dt)
{
	// call base class method
	GameBase::Update(dt);

	// ending conditions
	if (Player.Position.x < 1.0f)
	{
		// do ending things, that need to be done once, like stop all sounds etc.
		if (this->Win == GL_FALSE)
		{
			// stop sounds
			//this->MusicPlayer.StopSound("Data/Audio/Game1/bgm.flac");
			this->MusicPlayer.StopSound("Data/Audio/Game1/engine.ogg");
			// play gameover sounds
			this->MusicPlayer.PlaySound("Data/Audio/Game1/victory.ogg", false);
			// set flag
			this->Win = GL_TRUE;
		}
		// end execution of Update();
		return;
	}
	if (Player.Position.y < 1.0f)
	{
		// do ending things, that need to be done once, like stop all sounds etc.
		if (this->Crash == GL_FALSE)
		{
			//this->MusicPlayer.StopSound("Data/Audio/Game1/bgm.flac");
			this->MusicPlayer.StopSound("Data/Audio/Game1/engine.ogg");

			this->MusicPlayer.PlaySound("Data/Audio/Game1/fail.ogg", false);

			this->Crash = GL_TRUE;
		}
		return;
	}

	// actual game

	// don't do anything, if game is paused
	if (Paused)
		return;

	// update camera position... I mean... projection matrix.
	this->Projection = glm::ortho(Player.GetCenter().x * GAME_UNIT - this->Width / 2.0f, Player.GetCenter().x * GAME_UNIT + this->Width * 0.5f, 0.0f, (GLfloat)this->Height, -1.0f, 1.0f);

	// update player
	Player.Update(dt);

	// update plane engine sound volume
	if (engineSound != NULL)
		this->MusicPlayer.SetSoundVolume(engineSound, 0.2f + 0.7f * (float)(std::abs(this->Player.XSpeed) / this->Player.max_x_speed_));

	// check how far the player scouted
	GLfloat maxView = Player.GetCenter().x + this->Width / GAME_UNIT * 0.5f;
	if (maxView > GameUnitsScouted)
	{
		if (maxView > this->WidthUnits)
			maxView = this->WidthUnits;
		GameUnitsScouted = maxView;
	}

	// check if player is outside of the screen, and correct his position
	if (Player.Position.y + Player.Size.y > this->HeightUnits)
		Player.Position.y = this->HeightUnits - Player.Size.y;
	if (Player.Position.y < 0.0f)
		Player.Position.y = 0.0f;
	if (Player.Position.x < 0.0f)
		Player.Position.x = 0.0f;
	if (Player.Position.x + Player.Size.x >this->WidthUnits)
		Player.Position.x = this->WidthUnits - Player.Size.x;

	DoRegularGrenadePhysics(dt);

	DoIncendiaryGrenadePhysics(dt);
}

void Game1::Render()
{
	// background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	if (Paused)
	{
		GameStart();
	}
	else if (Win == GL_TRUE)
	{
		GameOver_Victory();
	}
	else if (Crash == GL_TRUE)
	{
		GameOver_Crash();
	}
	else
	{
		Background.Render(this->Renderer, this->Projection, this->GAME_UNIT);

		for (auto& var : Soldiers)
		{
			// we don't need to draw things that are outside of the screen
			if (glm::distance(var.GetCenter(), Player.GetCenter()) < 12)
				var.Render(this->Renderer, this->Projection, this->GAME_UNIT);
		}
		for (auto& var : Supplies)
		{
			if (glm::distance(var.GetCenter(), Player.GetCenter()) < 12)
				var.Render(this->Renderer, this->Projection, this->GAME_UNIT);
		}

		ItalyFlag.Render(this->Renderer, this->Projection, this->GAME_UNIT);
		OttomanFlag.Render(this->Renderer, this->Projection, this->GAME_UNIT);

		Player.Render(this->Renderer, this->Projection, this->GAME_UNIT);

		Grenade.Render(this->Renderer, this->Projection, this->GAME_UNIT);
		InciGrenade.Render(this->Renderer, this->Projection, this->GAME_UNIT);

		// render text
		std::string scoreText = "Score: " + std::to_string(this->Score);
		std::string speedText = "Speed: " + std::to_string((GLint)(Player.XSpeed * 100.0f / 15.0f)) + "km/h";
		std::string ammoText = "Grenades (Regular (Space) / Incendiary (Enter)): " + std::to_string(this->Player.RegularAmmo) + "/" + std::to_string(this->Player.InciAmmo);

		std::string finalText = scoreText + " | " + speedText + " | " + ammoText;

		GLint currentScreenPos = Player.GetCenter().x * GAME_UNIT - this->Width / 2.0f;
		TextRenderer.RenderText(finalText, currentScreenPos, 510, Projection, glm::vec3(0.0f, 0.0f, 0.0f), Alignment::NORMAL);
	}

	// call base class method (at the end. Otherwise it will be invisible. Order matters.)
	GameBase::Render();
}

// ------------------------------------------------------------------------- Helper Functions -------------------------------------------------------------------------

void Game1::RestartGame()
{
	// remove all sounds
	this->MusicPlayer.StopAllSounds();

	// clear enemies
	Soldiers[0].Delete();
	Supplies[0].Delete();

	Soldiers.clear();
	Supplies.clear();

	ItalyFlag.KIA = GL_FALSE;
	OttomanFlag.Delete();

	GenerateEnemies();

	this->Score = 0;
	SoldiersKilled = 0;
	SuppliesDestroyed = 0;
	GameUnitsScouted = 0;

	Player.Reset();

	Paused = GL_TRUE;
	Win = GL_FALSE;
	Crash = GL_FALSE;
}

#include <time.h>

void Game1::GenerateEnemies()
{
	// set the number of enemies
	GLuint soldiers = 40;
	GLuint supplies = 100;

	// seed the random number generator 
	time_t timev;
	time(&timev);
	srand(timev);

	Texture2D soldierTexture = Texture2D::loadTextureFromFile("Data/Textures/Game1/Soldier.png", GL_TRUE);

	for (GLint i = soldiers - 1; i >= 0; i--)
	{
		// get random number for x coordinates in ranges from 150 to 298 (width of complete game)
		GLfloat rx = 150.0f + (GLfloat)(rand()) / ((GLfloat)(RAND_MAX / (this->WidthUnits - 152.0f)));
		// get random number for y coordinates in ranges from 0.1 to 2.0
		GLfloat ry = 0.1f + (GLfloat)(rand()) / ((GLfloat)(RAND_MAX / (1.5f - 0.1f)));

		// create object
		Game1EnemyObject soldier;
		// initialize
		soldier.Init(soldierTexture, glm::vec2(rx, ry), glm::vec2(1.0f, 2.0f));
		// determine, if he looks right or left
		soldier.FlipTexture = rand() & 1 ? GL_TRUE : GL_FALSE;
		// set the point value
		soldier.Points = 10;
		// add to list
		Soldiers.push_back(soldier);
	}

	Texture2D supplyTexture = Texture2D::loadTextureFromFile("Data/Textures/Game1/Crate.png", GL_TRUE);

	for (GLint i = supplies - 1; i >= 0; i--)
	{
		GLfloat rx = 150.0f + (GLfloat)(rand()) / ((GLfloat)(RAND_MAX / (this->WidthUnits - 152.0f)));
		GLfloat ry = 0.1f + (GLfloat)(rand()) / ((GLfloat)(RAND_MAX / (2.2f - 0.1f)));

		Game1EnemyObject crate;
		crate.Init(supplyTexture, glm::vec2(rx, ry), glm::vec2(1.0f, 1.0f));
		crate.Points = 5;
		Supplies.push_back(crate);
	}

	// flag
	GLfloat rx = 150.0f + (GLfloat)(rand()) / ((GLfloat)(RAND_MAX / (this->WidthUnits - 152.0f)));
	OttomanFlag.Init("Data/Textures/Game1/OttomanFlag.png", glm::vec2(rx, 1.0f), glm::vec2(1.0f, 3.0f));
	OttomanFlag.Points = 50;
}


void Game1::DoRegularGrenadePhysics(GLfloat dt)
{
	// update grenade position and do collisions
	if (Grenade.Position.y < 0.0f)			// if grenade is outside of the screen, do nothing
		return;
	else if (Grenade.Position.y > 1.0f)		// if grenade is on screen, move it until it hits the ground
	{
		Grenade.Position.y -= Grenade.YSpeed * dt;
		Grenade.Position.x += Grenade.XSpeed * dt;
	}
	else {									// if it hit the ground, add the time to the explosion time, to control the animation size
		Grenade.TimeSinceExplosion += dt;
		// play sound, if isn't playing currently
		if (this->SoundPlayingEffect == GL_FALSE) {
			this->MusicPlayer.PlaySound("Data/Audio/Game1/bomb1.ogg");
			this->SoundPlayingEffect = GL_TRUE;
		}
		// if reached max explosion time, move out of screen
		if (Grenade.TimeSinceExplosion >= 0.15f) {
			// do collisions, remove hit objects
			DoRegularGrenadeCollisions();

			Grenade.Position.y = -1.0f;
			Grenade.TimeSinceExplosion = 0.0f;

			// reset sound
			this->SoundPlayingEffect = GL_FALSE;
		}
	}
}

void Game1::DoRegularGrenadeCollisions()
{
	glm::vec2 explosionCenter = Grenade.GetCenter();
	GLfloat radius = this->Grenade.explosion_radius_;

	// check if player caught in explosion
	if (glm::distance(explosionCenter, Player.GetCenter()) < radius)
	{
		this->MusicPlayer.StopSound("Data/Audio/Game1/engine.ogg");
		this->MusicPlayer.PlaySound("Data/Audio/Game1/fail.ogg", false);
		this->Crash = GL_TRUE;
	}

	// soldier collision
	for (auto& var : Soldiers)
	{
		if (var.KIA == GL_FALSE && glm::distance(explosionCenter, var.GetCenter()) < radius) {
			Score += var.Points;
			this->SoldiersKilled += 1;
			var.KIA = GL_TRUE;
		}
	}

	// supply collision
	for (auto& var : Supplies)
	{
		if (var.KIA == GL_FALSE && glm::distance(explosionCenter, var.GetCenter()) < radius) {
			Score += var.Points;
			this->SuppliesDestroyed += 1;
			var.KIA = GL_TRUE;
		}
	}

	// flag collisions
	if (ItalyFlag.KIA == GL_FALSE && glm::distance(explosionCenter, ItalyFlag.GetCenter()) < radius) {
		Score += ItalyFlag.Points;
		ItalyFlag.KIA = GL_TRUE;
	}

	// ottoman flag is solid, can only be burned down :P

	//if (OttomanFlag.KIA == GL_FALSE && glm::distance(explosionCenter, OttomanFlag.GetCenter()) < radius) {
	//	Score += OttomanFlag.Points;
	//	OttomanFlag.KIA = GL_TRUE;
	//}
}

void Game1::DoIncendiaryGrenadePhysics(GLfloat dt)
{
	// update grenade position and do collisions
	if (InciGrenade.Position.y < 0.0f)			// if grenade is outside of the screen, do nothing
		return;
	else if (InciGrenade.Position.y > 1.0f)		// if grenade is on screen, move it until it hits the ground
	{
		InciGrenade.Position.y -= InciGrenade.YSpeed * dt;
		InciGrenade.Position.x += InciGrenade.XSpeed * dt;
	}
	else {	// if it hit the ground, add the times to control the animation size
		if (InciGrenade.TimeSinceFireStarted == 0.0f) {	// if fire not stardet, explode the grenade
			InciGrenade.TimeSinceExplosion += dt;

			// play sound, if isn't playing currently
			if (this->SoundPlayingEffect == GL_FALSE) {
				this->MusicPlayer.PlaySound("Data/Audio/Game1/bomb2.ogg");
				this->SoundPlayingEffect = GL_TRUE;
			}

			// if reached max explosion time, let it burn
			if (InciGrenade.TimeSinceExplosion >= InciGrenade.MaxExplosionTime) {
				InciGrenade.TimeSinceFireStarted += dt;
			}
		}
		else //	if fire started, let it burn for a while
		{
			InciGrenade.TimeSinceFireStarted += dt;
			if (InciGrenade.TimeSinceFireStarted >= InciGrenade.MaxBurnTime) { 			// if reached max burning time, delete burned objects

				DoIncendiaryGrenadeCollisions();

				// and reset grenade values
				InciGrenade.Position.y = -1.0f;
				InciGrenade.TimeSinceExplosion = 0.0f;
				InciGrenade.TimeSinceFireStarted = 0.0f;

				this->SoundPlayingEffect = GL_FALSE;
			}
		}
	}
}

void Game1::DoIncendiaryGrenadeCollisions()
{
	glm::vec2 explosionCenter = InciGrenade.GetCenter();
	GLfloat radius = this->InciGrenade.fire_xsize_ / 2.0f;

	//// check if player caught in explosion
	//if (glm::distance(explosionCenter, Player.GetCenter()) < radius)
	//	Player.Color.a = 0.4f;

	// soldier collision
	for (auto& var : Soldiers)
	{
		if (var.KIA == GL_FALSE && glm::distance(explosionCenter, var.GetCenter()) < radius) {
			Score += var.Points;
			this->SoldiersKilled += 1;
			var.KIA = GL_TRUE;
		}
	}

	// supply collision
	for (auto& var : Supplies)
	{
		if (var.KIA == GL_FALSE && glm::distance(explosionCenter, var.GetCenter()) < radius) {
			Score += var.Points;
			this->SuppliesDestroyed += 1;
			var.KIA = GL_TRUE;
		}
	}

	// flag collisions
	if (ItalyFlag.KIA == GL_FALSE && glm::distance(explosionCenter, ItalyFlag.GetCenter()) < radius) {
		Score += ItalyFlag.Points;
		ItalyFlag.KIA = GL_TRUE;
	}
	if (OttomanFlag.KIA == GL_FALSE && glm::distance(explosionCenter, OttomanFlag.GetCenter()) < radius) {
		Score += OttomanFlag.Points;
		OttomanFlag.KIA = GL_TRUE;
	}
}

void Game1::GameStart()
{
	this->Projection = glm::ortho(0.0f, (GLfloat)this->Width, 0.0f, (GLfloat)this->Height, -1.0f, 1.0f);

	glm::vec3 grey(0.5f, 0.5f, 0.5f);
	glm::vec3 greywhite(0.75f, 0.75f, 0.75f);
	glm::vec3 white(1.0f, 1.0f, 1.0f);
	glm::vec3 title(0.6f, 0.4f, 0.4f);
	TextRenderer.RenderText("Libya, 1st November 1911, Italo-Turkish War", 480, 390, Projection, title, Alignment::CENTER);

	TextRenderer.RenderText("You are:", 225, 330, Projection, grey, Alignment::NORMAL);
	TextRenderer.RenderText("2nd Lieutenant Gavotti Giulio.", 700, 330, Projection, greywhite, Alignment::BACKWARDS);
	TextRenderer.RenderText("Your mission:", 225, 300, Projection, grey, Alignment::NORMAL);
	TextRenderer.RenderText("Carry out airplane reconnaissance.", 700, 300, Projection, greywhite, Alignment::BACKWARDS);
	TextRenderer.RenderText("Your plane:", 225, 270, Projection, grey, Alignment::NORMAL);
	TextRenderer.RenderText("Etrich Taube Monoplane.", 700, 270, Projection, greywhite, Alignment::BACKWARDS);

	TextRenderer.RenderText("(press WASD to start your mission)", 480, 210, Projection, white, Alignment::CENTER);

	TextRenderer.RenderText("Controls:", 100, 130, Projection, white, Alignment::NORMAL);
	TextRenderer.RenderText("WASD to move plane.", 100, 100, Projection, grey, Alignment::NORMAL);
	TextRenderer.RenderText("Space to drop grenade.", 100, 70, Projection, grey, Alignment::NORMAL);
	TextRenderer.RenderText("Enter to drop incendiary grenade.", 100, 40, Projection, grey, Alignment::NORMAL);

}

void Game1::GameOver_Victory()
{
	this->Projection = glm::ortho(0.0f, (GLfloat)this->Width, 0.0f, (GLfloat)this->Height, -1.0f, 1.0f);
	TextRenderer.RenderText("YOU WON!", 480, 330, Projection, glm::vec3(1.0f, 0.5f, 0.5f), Alignment::CENTER);
	TextRenderer.RenderText("Your score: " + std::to_string(this->Score), 480, 300, Projection, glm::vec3(0.5f, 1.0f, 0.5f), Alignment::CENTER);
	GLuint percentageScouted = (GLuint)((this->GameUnitsScouted / this->WidthUnits) * 100.0f);
	TextRenderer.RenderText("You completed your mission, recon, in " + std::to_string(percentageScouted) + "%", 480, 270, Projection, glm::vec3(1.0f, 1.0f, 1.0f), Alignment::CENTER);
	TextRenderer.RenderText("You killed " + std::to_string(this->SoldiersKilled) + " enemies and destroyed " + std::to_string(this->SuppliesDestroyed) + " enemy supplies.", 480, 240, Projection, glm::vec3(1.0f, 1.0f, 1.0f), Alignment::CENTER);
	TextRenderer.RenderText("(Press F5 to restart or ESC to quit)", 480, 30, Projection, glm::vec3(0.5f, 0.5f, 0.5f), Alignment::CENTER);
}

void Game1::GameOver_Crash()
{
	this->Projection = glm::ortho(0.0f, (GLfloat)this->Width, 0.0f, (GLfloat)this->Height, -1.0f, 1.0f);
	std::string text;
	TextRenderer.RenderText("You crashed in enemy territory. You were captured.", 480, 300, Projection, glm::vec3(0.6f, 0.5f, 0.4f), Alignment::CENTER);
	TextRenderer.RenderText("Enemies did terrible things to you. You are dead.", 480, 270, Projection, glm::vec3(0.4f, 0.5f, 0.6f), Alignment::CENTER);
	TextRenderer.RenderText("(Press F5 to restart or ESC to quit)", 480, 30, Projection, glm::vec3(0.5f, 0.5f, 0.5f), Alignment::CENTER);
}
