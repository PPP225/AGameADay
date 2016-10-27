# AGameADay - Simple games, each made in one day.

-------------------------------------------------------------------------------

This is a Visual Studio 2015 project. It should complile everywhere
(with the removal of 3 lines of *windows code* in `AGameADay.cpp`), as long as 
dependencies (below) are available on the target platform.

-------------------------------------------------------------------------------

## Tools used

* **C++** 11+ - for everything
* **OpenGL** 3.3 - for graphics
* **GLFW** + GLEW - for windowing
* **SOIL2** - for image loading from disk and creating OpenGL textures
* **glm** - for math things
* **irrklang** - for sound
* **freetype** - for text rendering

-------------------------------------------------------------------------------

## Code description

###### Everything is made as simple as it can get, with as few abstractions as possible.

### Entry point

* `AGameADay.cpp`
	* Application entry point
	* All window related things
	* Game loop execution

### Game related classes

* `GameBase.h/.cpp`
	* Abstract base class.
	* Contains all needed elements for a game to work.
		* `Init()` - initializes all needed things.
		* `ProcessKeys(dt)` - handles key input.
		* `Update(dt)` - updates the game state.
		* `Render()` - renders things on screen.

* `Gamebase.h`/`GameObjectBase` class
	* Abstract GameObject base class.
	* Makes creating objects easier, as it contains important methods.
		* `Init()` - allows to initialize object with a Texture and basic data.
		* `Delete()` - which deletes the texture from VRAM.
		* `Render()` - which renders the texture on screen.

* `Game0.h/.cpp`
	* Template class for a game.
	* To create a new game, best to use a copy this class, and make sure to make the game selectable by user in AGameADay.cpp.

### Sound

* `SoundEngine.h/.cpp`
	* Uses irrklang library for sound playback.
	* Contains most important functions.

### OpenGL related classes

* `SpriteRenderer.h/.cpp`
	* Basic renderer. `Draw(...)` method takes all parameters needed to render objects on screen.
	* Needs to be initialized only once!
	
* `Shader.h/.cpp` - holds a shader
* `Texture2D.h/.cpp` - holds a texture
* `TextEngine.h/.cpp` - allows to render text

-------------------------------------------------------------------------------

## Game List

Number | Game | Lines of code | Comments
----|---------|------
000 | Template | 150 | Base new game files on this.
001 | Italo-Turkish Bomber Simulator 1911 | 790 | Brute forced this as fast as possible. The code is a mess.
002 | Pong | 460 | This code is quite organised and readable. 
003 | - | - | -
