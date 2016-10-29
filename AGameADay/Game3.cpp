#include "Game3.h"

#include <glm/gtc/matrix_transform.hpp>
#include <time.h>
#include <iostream>

// Stone ---------------------------------------------------------------------------------------

const GLint StoneTypes[8][4][2][4] =
{
	{// Type = 0 means empty field.

	},
	{	// Type = 1 (I)
		{	// Rotation = 0
			{ -1,0,1,2 },	// x offset
			{ 0,0,0,0 }		// y offset
		},
		{	// Rotation = 1
			{ 0,0,0,0 },
			{ -1,0,1,2 }
		},
		{	// Rotation = 2
			{ 1,0,-1,-2 },
			{ 0,0,0,0 }
		},
		{	// Rotation = 3
			{ 0,0,0,0 },
			{ 1,0,-1,-2 }
		}
	},
	{	// Type = 2 (T)
		{
			{ -1,0,1,0 },
			{ 0,0,0,1 }
		},
		{
			{ 0,1,0,0 },
			{ 0,0,1,-1 }
		},
		{
			{ 0,1,-1,0 },
			{ 0,0,0,-1 }
		},
		{
			{ 0,0,0,-1 },
			{ 0,1,-1,0 }
		}
	},
	{	// Type = 3 (L)
		{
			{ 0,-1,1,1 },
			{ 0,0,0,1 }
		},
		{
			{ 0,0,0,1 },
			{ 0,1,-1,-1 }
		},
		{
			{ 0,1,-1,-1 },
			{ 0,0,0,-1 }
		},
		{
			{ 0,0,0,-1 },
			{ 0,1,-1,1 }
		}
	},
	{	// Type = 4 (J)
		{
			{ 0,1,-1,-1 },
			{ 0,0,0,1 }
		},
		{
			{ 0,0,0,1 },
			{ 0,1,-1,1 }
		},
		{
			{ 0,-1,1,1 },
			{ 0,0,0,-1 }
		},
		{
			{ 0,0,0,-1 },
			{ 0,1,-1,-1 }
		},
	},
	{	// Type = 5 (S)
		{
			{ 0,-1,0,1 },
			{ 0,0,1,1 }
		},
		{
			{ 0,0,1,1 },
			{ 0,1,0,-1 }
		},
		{
			{ 0,-1,0,1 },
			{ 0,0,1,1 }
		},
		{
			{ 0,0,1,1 },
			{ 0,1,0,-1 }
		}
	},
	{	// Type = 6 (Z)
		{
			{ 0,1,0,-1 },
			{ 0,0,1,1 }
		},
		{
			{ 0,0,1,1 },
			{ 0,-1,0,1 }
		},
		{
			{ 0,1,0,-1 },
			{ 0,0,1,1 }
		},
		{
			{ 0,0,1,1 },
			{ 0,-1,0,1 }
		}
	},
	{	// Type = 7 (O)
		{
			{ 0,1,0,1 },
			{ 0,0,1,1 }
		},
		{
			{ 0,1,0,1 },
			{ 0,0,1,1 }
		},
		{
			{ 0,1,0,1 },
			{ 0,0,1,1 }
		},
		{
			{ 0,1,0,1 },
			{ 0,0,1,1 }
		}
	},
};

void Game3Stone::Init(Texture2D & texture, SoundEngine& musicPlayer)
{
	this->MusicPlayer = &musicPlayer;

	this->Texture = texture;
	this->Size = glm::vec2(1.0f, 1.0f);

	this->Rotation = 0;
	this->FlipTexture = GL_FALSE;
	this->Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);

	next_ = (GLbyte)(rand() % 7 + 1);

	initialized_ = GL_TRUE;
}

void Game3Stone::Render(SpriteRenderer & renderer, glm::mat4 & projection, GLfloat gameUnit, glm::vec2 levelPosition)
{
	if (!initialized_)
	{
		std::cout << "ERROR::Game3Stone: Game3Stone not initialized!" << std::endl;
		return;
	}
	GLint x1 = Position.x + StoneTypes[this->Type][this->Rotation][0][0],
		x2 = Position.x + StoneTypes[this->Type][this->Rotation][0][1],
		x3 = Position.x + StoneTypes[this->Type][this->Rotation][0][2],
		x4 = Position.x + StoneTypes[this->Type][this->Rotation][0][3];
	GLint y1 = Position.y + StoneTypes[this->Type][this->Rotation][1][0],
		y2 = Position.y + StoneTypes[this->Type][this->Rotation][1][1],
		y3 = Position.y + StoneTypes[this->Type][this->Rotation][1][2],
		y4 = Position.y + StoneTypes[this->Type][this->Rotation][1][3];
	renderer.Draw(this->Texture, projection, (levelPosition + glm::vec2(x1, y1)) * gameUnit, this->Size * gameUnit, 0.0f, GL_FALSE, this->Color);
	renderer.Draw(this->Texture, projection, (levelPosition + glm::vec2(x2, y2)) * gameUnit, this->Size * gameUnit, 0.0f, GL_FALSE, this->Color);
	renderer.Draw(this->Texture, projection, (levelPosition + glm::vec2(x3, y3)) * gameUnit, this->Size * gameUnit, 0.0f, GL_FALSE, this->Color);
	renderer.Draw(this->Texture, projection, (levelPosition + glm::vec2(x4, y4)) * gameUnit, this->Size * gameUnit, 0.0f, GL_FALSE, this->Color);
}

void Game3Stone::RenderNext(SpriteRenderer & renderer, glm::mat4 & projection, GLfloat gameUnit, glm::vec2 position)
{
	if (!initialized_)
	{
		std::cout << "ERROR::Game3Stone: Game3Stone not initialized!" << std::endl;
		return;
	}
	GLint x1 = StoneTypes[this->next_][0][0][0],
		x2 = StoneTypes[this->next_][0][0][1],
		x3 = StoneTypes[this->next_][0][0][2],
		x4 = StoneTypes[this->next_][0][0][3];
	GLint y1 = StoneTypes[this->next_][0][1][0],
		y2 = StoneTypes[this->next_][0][1][1],
		y3 = StoneTypes[this->next_][0][1][2],
		y4 = StoneTypes[this->next_][0][1][3];
	renderer.Draw(this->Texture, projection, (position + glm::vec2(x1, y1)) * gameUnit, this->Size * gameUnit, 0.0f, GL_FALSE, GetTypeColor(next_));
	renderer.Draw(this->Texture, projection, (position + glm::vec2(x2, y2)) * gameUnit, this->Size * gameUnit, 0.0f, GL_FALSE, GetTypeColor(next_));
	renderer.Draw(this->Texture, projection, (position + glm::vec2(x3, y3)) * gameUnit, this->Size * gameUnit, 0.0f, GL_FALSE, GetTypeColor(next_));
	renderer.Draw(this->Texture, projection, (position + glm::vec2(x4, y4)) * gameUnit, this->Size * gameUnit, 0.0f, GL_FALSE, GetTypeColor(next_));
}

void Game3Stone::Spawn()
{
	this->Type = next_;
	next_ = (GLbyte)(rand() % 7 + 1);

	this->Position = glm::ivec2(5, 25);
	this->Rotation = 0;
	
	this->Color = GetTypeColor(this->Type);
}

glm::vec4& Game3Stone::GetTypeColor(GLbyte type)
{
	switch (type)
	{
	case 1:	// I
		return glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);	// red
	case 2:	// T
		return glm::vec4(0.0f, 1.0f, 1.0f, 1.0f);	// cyan
	case 3:	// L
		return glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);	// blue
	case 4:	// J
		return glm::vec4(1.0f, 1.0f, 0.50f, 1.0f);	// yellow
	case 5:	// S
		return glm::vec4(1.0f, 0.0f, 1.0f, 1.0f);	// purple
	case 6:	// Z
		return glm::vec4(0.0f, 1.0f, 0.0f, 1.0f);	// green
	case 7:	// O
		return glm::vec4(1.0f, 0.5f, 0.0f, 1.0f);	// orange
	default:
		std::cout << "ERROR::Game3Stone: Invalid type!";
		return glm::vec4(1);
	}
}

void Game3Stone::MoveLeft(Game3LevelContainer & level)
{
	Position.x -= 1;
	if (Collision(level))
		Position.x += 1;
}

void Game3Stone::MoveRight(Game3LevelContainer & level)
{
	Position.x += 1;
	if (Collision(level))
		Position.x -= 1;
}

EmplaceEvent Game3Stone::MoveDown(Game3LevelContainer & level)
{
	Position.y -= 1;
	if (Collision(level))
	{
		this->MusicPlayer->PlaySound("Data/Audio/Game3/touchdown.ogg");

		Position.y += 1;

		GLint x1 = Position.x + StoneTypes[this->Type][this->Rotation][0][0],
			x2 = Position.x + StoneTypes[this->Type][this->Rotation][0][1],
			x3 = Position.x + StoneTypes[this->Type][this->Rotation][0][2],
			x4 = Position.x + StoneTypes[this->Type][this->Rotation][0][3];
		GLint y1 = Position.y + StoneTypes[this->Type][this->Rotation][1][0],
			y2 = Position.y + StoneTypes[this->Type][this->Rotation][1][1],
			y3 = Position.y + StoneTypes[this->Type][this->Rotation][1][2],
			y4 = Position.y + StoneTypes[this->Type][this->Rotation][1][3];

		EmplaceEvent result = level.Emplace(x1, x2, x3, x4, y1, y2, y3, y4, this->Type);
		if (result != EmplaceEvent::GAME)
			Spawn();
		return result;
	}
	return EmplaceEvent::NONE;
}

void Game3Stone::RotateRight(Game3LevelContainer & level)
{
	Rotation += 1;
	if (Rotation > 3)
		Rotation = 0;
	if (Collision(level))
	{
		if (Rotation == 0)
			Rotation = 3;
		else
			Rotation -= 1;
	}
}

GLboolean Game3Stone::Collision(Game3LevelContainer & level)
{
	GLint x1 = Position.x + StoneTypes[this->Type][this->Rotation][0][0],
		x2 = Position.x + StoneTypes[this->Type][this->Rotation][0][1],
		x3 = Position.x + StoneTypes[this->Type][this->Rotation][0][2],
		x4 = Position.x + StoneTypes[this->Type][this->Rotation][0][3];
	GLint y1 = Position.y + StoneTypes[this->Type][this->Rotation][1][0],
		y2 = Position.y + StoneTypes[this->Type][this->Rotation][1][1],
		y3 = Position.y + StoneTypes[this->Type][this->Rotation][1][2],
		y4 = Position.y + StoneTypes[this->Type][this->Rotation][1][3];
	if (level.Get(y1, x1) || level.Get(y2, x2) || level.Get(y3, x3) || level.Get(y4, x4)) // if any of them are != 0
		return GL_TRUE;
	return GL_FALSE;
}

// Level ---------------------------------------------------------------------------------------

// Row ------

Game3LevelRow::Game3LevelRow() : data_(10, 0)
{

}

GLboolean Game3LevelRow::Full()
{
	for (auto& x : data_)
	{
		if (x == 0)
			return GL_FALSE;
	}
	return GL_TRUE;
}

GLbyte & Game3LevelRow::operator[](GLuint idx)
{
	return data_[idx];
}

// Container ------


Game3LevelContainer::Game3LevelContainer()
{
	data_.resize(size_, Game3LevelRow());
}

GLbyte Game3LevelContainer::Get(GLint row, GLint column)
{
	if (column < 0 || column >= 10)	// limit left & right movement
		return 7;
	if (row < 0)	// if hit the ground, it's colliding
		return 7;
	if ((GLuint)row >= size_)	// if high above, it's not colliding
		return 0;
	return data_[row][column];	// if inside of level, check if collides with blocks
}

EmplaceEvent Game3LevelContainer::Emplace(GLuint x1, GLuint x2, GLuint x3, GLuint x4, GLuint y1, GLuint y2, GLuint y3, GLuint y4, GLbyte type)
{
	if (y1 >= size_ || y2 >= size_ || y3 >= size_ || y4 >= size_)
		return EmplaceEvent::GAME;

	data_[y1][x1] = type;
	data_[y2][x2] = type;
	data_[y3][x3] = type;
	data_[y4][x4] = type;

	return ClearFullRows();
}

EmplaceEvent Game3LevelContainer::ClearFullRows()
{
	GLuint cleared = 0;
	for (GLuint row = 0; row < size_; row++)
	{
		//for (GLuint col = 0; col < 10; col++)
		if (data_[row].Full())
		{
			data_.erase(data_.begin() + row);
			data_.push_back(Game3LevelRow());
			--row;
			++cleared;
		}
	}
	switch (cleared)
	{
	case 0:
		return EmplaceEvent::NONE;
	case 1:
		return EmplaceEvent::ROW1;
	case 2:
		return EmplaceEvent::ROW2;
	case 3:
		return EmplaceEvent::ROW3;
	case 4:
		return EmplaceEvent::ROW4;
	default:
		std::cout << "ERROR::Game3LevelContainer::ClearFullRows(): Invalid switch!";
		return EmplaceEvent::GAME;
	}
}

void Game3LevelContainer::ClearAll()
{
	data_.clear();
	data_.resize(size_, Game3LevelRow());
}

// Level ------

void Game3Level::Init(Texture2D & blockTexture, Texture2D & backgroundTexture, glm::vec2 levelTexturePosition, glm::vec2 levelTextureSize)
{
	// level image data
	this->BackgroundTexture = backgroundTexture;
	this->LevelTexPosition = levelTexturePosition;
	this->LevelTexSize = levelTextureSize;

	// level block image data
	this->Texture = blockTexture;
	this->Size = glm::vec2(1.0f, 1.0f);

	initialized_ = GL_TRUE;
}

void Game3Level::Render(SpriteRenderer & renderer, glm::mat4 & projection, GLfloat gameUnit, glm::vec2 levelPosition)
{
	if (!initialized_)
	{
		std::cout << "ERROR::Game3Level: Game3Level not initialized!" << std::endl;
	}

	renderer.Draw(this->BackgroundTexture, projection, this->LevelTexPosition * gameUnit, this->LevelTexSize * gameUnit);

	for (GLuint row = 0; row < data_.size_; row++)
	{
		for (GLuint col = 0; col < 10; col++)
		{
			GLbyte t = data_.Get(row, col);
			if (t != 0)
			{
				renderer.Draw(this->Texture, projection, (levelPosition + glm::vec2(col, row)) * gameUnit, this->Size * gameUnit, 0.0f, GL_TRUE, Game3Stone::GetTypeColor(t));
			}
		}
	}
}

Game3Level::Game3Level() : data_()
{

}

Game3LevelContainer & Game3Level::GetData()
{
	return data_;
}

void Game3Level::Clear()
{
	data_.ClearAll();
}

// Game ---------------------------------------------------------------------------------------

void Game3::Init()
{
	// call base class method
	GameBase::Init();

	// set projection matrix to game coordinates
	this->Projection = glm::ortho(0.0f, 960.0f, 0.0f, 540.0f, -1.0f, 1.0f);

	// seed the random number generator 
	time_t timev;
	time(&timev);
	srand((unsigned int)timev);

	// create game objects
	Texture2D blockTexture = Texture2D::loadTextureFromFile("Data/Textures/Game3/Block.png", GL_TRUE);
	Texture2D levelTexture = Texture2D::loadTextureFromFile("Data/Textures/Game3/Level.png", GL_TRUE);
	Texture2D backgroundTexture = Texture2D::loadTextureFromFile("Data/Textures/Game3/Background.png", GL_TRUE);

	blockTexture.Filter_Max = GL_NEAREST;
	blockTexture.Filter_Min = GL_NEAREST;
	levelTexture.Filter_Max = GL_NEAREST;
	levelTexture.Filter_Min = GL_NEAREST;
	backgroundTexture.Filter_Max = GL_NEAREST;
	backgroundTexture.Filter_Min = GL_NEAREST;

	blockTexture.Regen();
	levelTexture.Regen();
	backgroundTexture.Regen();

	Stone.Init(blockTexture, this->MusicPlayer);
	Stone.Spawn();

	glm::vec2 lvlPos = LEVEL_POSITION;
	lvlPos.x -= 2;
	lvlPos.y -= 2;
	glm::vec2 lvlSize(14,27);

	Level.Init(blockTexture, levelTexture, lvlPos, lvlSize);

	Background.Init(backgroundTexture, glm::vec2(0), glm::vec2(this->Width, this->Height));

	// Init some other data
	RestartGame();
}

GLfloat holdingS = 0.0f;
GLfloat holdingA = 0.0f;
GLfloat holdingD = 0.0f;
const GLfloat hold_time_treshold_ = 0.2f;

void Game3::ProcessKeys(GLfloat dt)
{
	// call base class method
	GameBase::ProcessKeys(dt);

	// handle all game input

	if (this->Keys[GLFW_KEY_F5])
		RestartGame();

	if (game_over_ && this->Keys[GLFW_KEY_SPACE])
		RestartGame();

	EmplaceEvent eevent = EmplaceEvent::NONE;

	// check if player pauses or unpauses game
	if (this->Keys[GLFW_KEY_P] && !this->KeysProcessed[GLFW_KEY_P])
	{
		game_paused_ = !game_paused_;
		this->KeysProcessed[GLFW_KEY_P] = GL_TRUE;
	}
	if (this->Keys[GLFW_KEY_SPACE] && !this->KeysProcessed[GLFW_KEY_SPACE])
	{
		game_paused_ = !game_paused_;
		this->KeysProcessed[GLFW_KEY_SPACE] = GL_TRUE;
	}

	// if game is over or paused, don't handle any input
	if (game_over_ || game_paused_)
		return;

	// handle wsad input
	if (this->Keys[GLFW_KEY_W] && !this->KeysProcessed[GLFW_KEY_W]) {
		Stone.RotateRight(Level.GetData());
		this->KeysProcessed[GLFW_KEY_W] = GL_TRUE;
	}
	if (this->Keys[GLFW_KEY_S])
	{
		if (this->KeysProcessed[GLFW_KEY_S] == GL_FALSE)
		{
			
			eevent = Stone.MoveDown(Level.GetData());
			this->KeysProcessed[GLFW_KEY_S] = GL_TRUE;
			holdingS = 0.0f;
		}
		else // if processed, time how long is it being hold.
		{
			holdingS += dt;

			// if long enough, move more
			if (holdingS > hold_time_treshold_)
			{
				eevent = Stone.MoveDown(Level.GetData());
				holdingS -= 0.1f;
			}
		}
	}
	if (this->Keys[GLFW_KEY_A])
	{
		if (this->KeysProcessed[GLFW_KEY_A] == GL_FALSE)
		{
			Stone.MoveLeft(Level.GetData());
			this->KeysProcessed[GLFW_KEY_A] = GL_TRUE;
			holdingA = 0.0f;
		}
		else
		{
			holdingA += dt;

			if (holdingA > hold_time_treshold_)
			{
				Stone.MoveLeft(Level.GetData());
				holdingA -= 0.1f;
			}
		}
	}
	if (this->Keys[GLFW_KEY_D])
	{
		if (this->KeysProcessed[GLFW_KEY_D] == GL_FALSE)
		{
			Stone.MoveRight(Level.GetData());
			this->KeysProcessed[GLFW_KEY_D] = GL_TRUE;
			holdingD = 0.0f;
		}
		else
		{
			holdingD += dt;

			if (holdingD > hold_time_treshold_)
			{
				Stone.MoveRight(Level.GetData());
				holdingD -= 0.1f;
			}
		}
	}

	AddScore(eevent);
}

void Game3::Update(GLfloat dt)
{
	// call base class method
	GameBase::Update(dt);

	// if game is over or paused, don't update
	if (game_over_ || game_paused_)
		return;

	// update game state, physics, collisions
	// movement correlated with key input is done in `ProcessKeys()`

	this->TimeElapsed += dt;
	// if the time has come to move the stone down
	if (this->TimeElapsed > this->LevelSpeed)
	{
		this->TimeElapsed -= this->LevelSpeed;

		EmplaceEvent eevent = Stone.MoveDown(Level.GetData());
		AddScore(eevent);
	}
	
	// check if player gets to new level and advance him if necessary
	LevelAdvancement();
}

void Game3::Render()
{
	// clear background
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	Background.Render(this->Renderer, this->Projection, 1.0f);

	if (game_over_)
	{
		RenderGameoverBackground();
	}
	else if (game_paused_)
	{
		RenderPauseBackground();
	}
	else
	{
		RenderRegularBackground();
	}

	// render game objects
	Level.Render(this->Renderer, this->Projection, this->GAME_UNIT, LEVEL_POSITION);
	Stone.Render(this->Renderer, this->Projection, this->GAME_UNIT, LEVEL_POSITION);
	Stone.RenderNext(this->Renderer, this->Projection, this->GAME_UNIT, (LEVEL_POSITION + glm::vec2(15, 20)));

	// call base class method (at the end. Otherwise it will be invisible. Order matters.)
	GameBase::Render();
}

void Game3::RestartGame()
{
	this->MusicPlayer.StopAllSounds();
	this->MusicPlayer.PlaySound("Data/Audio/Game3/bgm1.it", true);

	Level.Clear();
	Stone.Spawn();

	this->Score = 0;
	this->LevelScore = 0;
	this->LevelNum = 1;
	this->LevelSpeed = 0.5f;
	GLfloat TimeElapsed = 0.5f;

	game_paused_ = GL_FALSE;
	game_over_ = GL_FALSE;
}

void Game3::LevelAdvancement()
{
	if (this->LevelScore > POINTS_PER_LEVEL)
	{
		this->MusicPlayer.PlaySound("Data/Audio/Game3/levelup.ogg");
		this->Score += 100;
		this->LevelNum++;
		this->LevelScore = 0;
		this->LevelSpeed -= 0.05f;
		if (this->LevelSpeed < 0.1f)
			this->LevelSpeed = 0.1f;
	}
}

void Game3::AddScore(EmplaceEvent e)
{
	switch (e)
	{
	case EmplaceEvent::NONE:
		break;
	case EmplaceEvent::ROW1:
		this->MusicPlayer.PlaySound("Data/Audio/Game3/pop.ogg");
		Score += 10;
		LevelScore += 15;
		break;
	case EmplaceEvent::ROW2:
		this->MusicPlayer.PlaySound("Data/Audio/Game3/pop.ogg");
		Score += 25;
		LevelScore += 25;
		break;
	case EmplaceEvent::ROW3:
		this->MusicPlayer.PlaySound("Data/Audio/Game3/pop.ogg");
		Score += 40;
		LevelScore += 34;
		break;
	case EmplaceEvent::ROW4:
		this->MusicPlayer.PlaySound("Data/Audio/Game3/pop.ogg");
		Score += 65;
		LevelScore += 40;
		break;
	case EmplaceEvent::GAME:
		this->MusicPlayer.StopSound("Data/Audio/Game3/bgm1.it");
		this->MusicPlayer.PlaySound("Data/Audio/Game3/gameover.ogg");
		game_over_ = GL_TRUE;
		break;
	default:
		break;
	}
}

void Game3::RenderRegularBackground()
{
	glm::vec3 white(1, 1, 1);
	this->TextRenderer.RenderText("Level: " + std::to_string(this->LevelNum), 800, (GLint)this->Height - 30, this->Projection, white, Alignment::NORMAL);
	this->TextRenderer.RenderText("Score: " + std::to_string(this->Score), 800, (GLint)this->Height - 60, this->Projection, white, Alignment::NORMAL);

	this->TextRenderer.RenderText("Next:", 650, (GLint)this->Height - 40, this->Projection, white, Alignment::NORMAL);
}

void Game3::RenderPauseBackground()
{
	glm::vec3 white(1, 1, 1);
	glm::vec3 grey(0.5f, 0.5f, 0.5f);
	this->TextRenderer.RenderText("GAME PAUSED", 170, (GLint)this->Height - 100, this->Projection, white, Alignment::CENTER);

	this->TextRenderer.RenderText("Controls: ", this->Width - 10.0f, 80.0f, this->Projection, white, Alignment::BACKWARDS);
	this->TextRenderer.RenderText("WSAD to move", this->Width - 10.0f, 50.0f, this->Projection, grey, Alignment::BACKWARDS);
	this->TextRenderer.RenderText("Space or P to pause/unpause", this->Width - 10.0f, 20.0f, this->Projection, grey, Alignment::BACKWARDS);
}

void Game3::RenderGameoverBackground()
{
	glm::vec3 white(1, 1, 1);
	this->TextRenderer.RenderText("GAME OVER", 170, (GLint)this->Height - 100, this->Projection, white, Alignment::CENTER);
	this->TextRenderer.RenderText("Yar score: " + std::to_string(this->Score) + "!", 170, (GLint)this->Height - 100 - 24, this->Projection, white, Alignment::CENTER);
	this->TextRenderer.RenderText("(Space or F5 to restart)", 170, (GLint)this->Height - 100 - 48, this->Projection, white, Alignment::CENTER);
}