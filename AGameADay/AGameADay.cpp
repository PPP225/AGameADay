/**
 *	Application entry point and windowing
 */

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>

#include <iostream>

#include "GameBase.h"
#include "Game0.h"
#include "Game1.h"
#include "Game2.h"
#include "Game3.h"
#include "Game4.h"
#include "Game5.h"

 // --------------------------------------------------------- Definitions ---------------------------------------------------------

// variables
GLboolean window_hidden_ = GL_TRUE;
GLuint WINDOW_POSITION_X = 100;
GLuint WINDOW_POSITION_Y = 100;
const GLuint WINDOW_WIDTH = 960;
const GLuint WINDOW_HEIGHT = 540;

GLboolean full_screen_ = GL_FALSE;
GLboolean F11_processed = GL_FALSE;

// Helper function declarations
GLuint InitWindow();
void Run();
void HideWindow(GLboolean hide);

// GLFW callback function declerations
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
void window_size_callback(GLFWwindow* window, int width, int height);
void window_pos_callback(GLFWwindow* window, int xpos, int ypos);

// objects
GLFWwindow* window_;
GameBase* TheGame;

// --------------------------------------------------------- MAIN ---------------------------------------------------------

#include <windows.h>

#include <thread>
#include <atomic>

int main() {
	// set console window position
	// this, and a line in HideWindow(), are the only windows specific things
	HWND consoleWindow = GetConsoleWindow();
	SetWindowPos(consoleWindow, 0, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOZORDER);

	// init window
	if (InitWindow() != 0)
		return 1;

	HideWindow(GL_TRUE);

	while (true)
	{
		std::string input;

		// get input
		std::cout << std::endl;
		std::cout << "Type in the game number. (h for game list, q to quit)" << std::endl;
		std::cin >> input;

		// check input
		if (input == "quit" || input == "exit" || input == "q")
			return 0;
		else if (input == "help" || input == "h" || input == "ls" || input == "dir" || input == "games" || input == "list" || input == "gamelist" || input == "g" || input == "gl")
			std::cout << "Gamelist:" << std::endl <<
			"001: Italo-Turkish Bomber Simulator 1911" << std::endl <<
			"002: Pong" << std::endl <<
			"003: Basic Tetris" << std::endl <<
			"004: Politics 20016" << std::endl <<
			"005: Snake Classic" << std::endl <<
			" ";
		else
		{
			// check game number
			GLint gameNumber;
			try {
				gameNumber = std::stoi(input);
			}
			catch (std::invalid_argument&) {
				std::cout << "No such command." << std::endl;
				continue;
			}
			catch (std::out_of_range&) {
				std::cout << "No such game." << std::endl;
				continue;
			}
			catch (...) {
				std::cout << "Unknown error." << std::endl;
				continue;
			}

			// create game object
			switch (gameNumber)
			{
			case 0:
				TheGame = new Game0();
				break;
			case 1:
				TheGame = new Game1();
				break;
			case 2:
				TheGame = new Game2();
				break;
			case 3:
				TheGame = new Game3();
				break;
			case 4:
				TheGame = new Game4();
				break;
			case 5:
				TheGame = new Game5();
				break;
			default:
				std::cout << "No such game number." << std::endl;
				continue;
			}

			// run the game and show window
			HideWindow(GL_FALSE);
			Run();
		}
	}

	return 0;
}

// --------------------------------------------------------- Helper Functions ---------------------------------------------------------

GLuint InitWindow()
{
	// GLFW
	if (!glfwInit())
		return 1;

	// window creation
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

	window_ = glfwCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, "AGameADay Project", nullptr, nullptr);
	glfwSetWindowPos(window_, WINDOW_POSITION_X, WINDOW_POSITION_Y);
	glfwMakeContextCurrent(window_);

	// glew
	glewExperimental = GL_TRUE;
	glewInit();
	glGetError(); // Call it once to catch glewInit() bug, all other errors are now from our application.

	// callbacks
	glfwSetKeyCallback(window_, key_callback);
	glfwSetCursorPosCallback(window_, mouse_pos_callback);
	glfwSetMouseButtonCallback(window_, mouse_button_callback);
	//glfwSetScrollCallback(window, scroll_callback);
	glfwSetWindowPosCallback(window_, window_pos_callback);
	glfwSetWindowSizeCallback(window_, window_size_callback);

	glfwSwapInterval(0);	// glfw swapinterval off

	// OpenGL configuration
	glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);
	//glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return 0;
}

void Run()
{
	// delta time variables
	GLfloat deltaTime = 0.0f;
	GLfloat lastFrame = 0.0f;

	// FPS diagnostics
	GLfloat time = 0;
	GLuint fps = 0;

	// init game
	TheGame->Init();

	while (!window_hidden_) {
		// check if user clicked the X button
		if (glfwWindowShouldClose(window_))
		{
			glfwSetWindowShouldClose(window_, GLFW_FALSE);
			HideWindow(GL_TRUE);
		}
		// delta time
		GLfloat currentFrame = (GLfloat)(glfwGetTime());
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;
		if (deltaTime > 0.25)
			continue;

		time += deltaTime;
		++fps;

		// pull input events from the window
		glfwPollEvents();

		// uptade game state
		TheGame->ProcessKeys(deltaTime);
		TheGame->Update(deltaTime);
		TheGame->Render();

		// swap buffers
		glfwSwapBuffers(window_);

		// fps counter
		if (time >= 1.0f)
		{
			// print fps data to console
			std::cout << "\r" << "FPS: " << fps << "    ";

			fps = 0;
			time = 0.0f;
		}
	}

	delete TheGame;
}

void HideWindow(GLboolean hide)
{
	// clear console
	system("cls");

	if (full_screen_)
	{
		full_screen_ = !full_screen_;

		// windowing
		glfwSetWindowMonitor(window_, NULL, WINDOW_POSITION_X, WINDOW_POSITION_Y, WINDOW_WIDTH, WINDOW_HEIGHT, -1);

		// update viewport
		glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

		// restoring callbacks
		glfwSetWindowSizeCallback(window_, window_size_callback);
		glfwSetWindowPosCallback(window_, window_pos_callback);
	}

	window_hidden_ = hide;
	if (hide)
		glfwHideWindow(window_);
	else
		glfwShowWindow(window_);
}

// --------------------------------------------------------- Callbacks ---------------------------------------------------------


void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods)
{
	// ESCAPE KEY closes APP
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		HideWindow(GL_TRUE);
		//glfwSetWindowShouldClose(window, GL_TRUE);

	// Fullscreen
	if (key == GLFW_KEY_F11 && action == GLFW_RELEASE) {
		F11_processed = GL_FALSE;
	}
	if (key == GLFW_KEY_F11 && action == GLFW_PRESS && !F11_processed) {
		full_screen_ = !full_screen_;
		F11_processed = GL_TRUE;
		if (full_screen_)
		{
			// disabling callbacks
			glfwSetWindowSizeCallback(window, NULL);	//crashes on fullscreen
			glfwSetWindowPosCallback(window, NULL);	// malfunctions

			// fullscreening
			GLFWmonitor* monitor = glfwGetPrimaryMonitor();
			const GLFWvidmode* mode = glfwGetVideoMode(monitor);
			glfwSetWindowMonitor(window, monitor, -1, -1, mode->width, mode->height, mode->refreshRate);

			// update viewport
			glViewport(0, 0, mode->width, mode->height);
		}
		else
		{
			// windowing
			glfwSetWindowMonitor(window, NULL, WINDOW_POSITION_X, WINDOW_POSITION_Y, WINDOW_WIDTH, WINDOW_HEIGHT, -1);

			// update viewport
			glViewport(0, 0, WINDOW_WIDTH, WINDOW_HEIGHT);

			// restoring callbacks
			glfwSetWindowSizeCallback(window, window_size_callback);
			glfwSetWindowPosCallback(window, window_pos_callback);
		}
	}

	if (key >= 0 && key < 1024)
	{
		if (action == GLFW_PRESS)
			TheGame->Keys[key] = GL_TRUE;
		else if (action == GLFW_RELEASE)
		{
			TheGame->Keys[key] = GL_FALSE;
			TheGame->KeysProcessed[key] = GL_FALSE;
		}
	}
}

void mouse_pos_callback(GLFWwindow* window, double xpos, double ypos)
{
	ypos = WINDOW_HEIGHT - ypos - 1.0;
}

void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
{
	//if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS)
	//{
	//}
}

void window_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

void window_pos_callback(GLFWwindow* window, int xpos, int ypos)
{
	if (ypos <= 0)
		return;
	WINDOW_POSITION_X = xpos;
	WINDOW_POSITION_Y = ypos;
}