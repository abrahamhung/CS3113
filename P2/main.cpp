//program to render triangle in window

#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

SDL_Window* displayWindow;
bool gameIsRunning = true;
int gamestate = 0;

ShaderProgram program;

glm::vec3 lpos, rpos, bpos;
glm::vec3 ldir, rdir, bdir;
glm::mat4 viewMatrix, lmatrix, rmatrix, bmatrix, projectionMatrix;

void Initialize() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("Triangle!", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 640, 480);

	program.Load("shaders/vertex.glsl", "shaders/fragment.glsl");

	viewMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);
	program.SetColor(0.2f, 0.2f, 0.0f, 1.0f);

	glUseProgram(program.programID);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	lpos = glm::vec3(-4.0f, 0.0f, 0.0f);
	rpos = glm::vec3(4.0f, 0.0f, 0.0f);
	bpos = glm::vec3(0.0f, 0.0f, 0.0f);
	ldir = glm::vec3(0.0f, 0.0f, 0.0f);
	rdir = glm::vec3(0.0f, 0.0f, 0.0f);
	bdir = glm::vec3(1.0f, .5f, 0.0f);
	lmatrix = glm::mat4(1.0f);
	rmatrix = glm::mat4(1.0f);
	bmatrix = glm::mat4(1.0f);
}

void ProcessInput() {
	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
			gameIsRunning = false;
		}
	}

	ldir = glm::vec3(0.0f, 0.0f, 0.0f);
	rdir = glm::vec3(0.0f, 0.0f, 0.0f);

	const Uint8* keys = SDL_GetKeyboardState(NULL);
	if (keys[SDL_SCANCODE_DOWN]) {
		rdir.y = -1.0f;
	}
	else if (keys[SDL_SCANCODE_UP]) {
		rdir.y = 1.0f;
	}
	if (keys[SDL_SCANCODE_S]) {
		ldir.y = -1.0f;
	}
	else if (keys[SDL_SCANCODE_W]) {
		ldir.y = 1.0f;
	}
	
}

float lastTicks = 0;
void Update() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float deltaTime = ticks - lastTicks;
	lastTicks = ticks;


	if (gamestate == 0) {
		lmatrix = glm::mat4(1.0f);
		rmatrix = glm::mat4(1.0f);
		bmatrix = glm::mat4(1.0f);

		if (bpos.y < -3) {
			bdir.y = 0 - bdir.y;
		}
		else if (bpos.y > 3) {
			bdir.y = 0 - bdir.y;
		}

		if (bpos.x < -4) {
			if (bpos.y < (lpos.y + .5) && bpos.y >(lpos.y - .5)) {
				bdir.x = 0 - bdir.x;
			}
			else {
				gamestate = 2;
			}
		}
		if (bpos.x > 4) {
			if (bpos.y < (rpos.y + .5) && bpos.y >(rpos.y - .5)) {
				bdir.x = 0 - bdir.x;
			}
			else {
				gamestate = 1;
			}
		}

		bpos += bdir * deltaTime;

		lpos += ldir * deltaTime;
		rpos += rdir * deltaTime;

		lmatrix = glm::translate(lmatrix, lpos);
		rmatrix = glm::translate(rmatrix, rpos);
		bmatrix = glm::translate(bmatrix, bpos);
	}
}

void Render() {
	glClear(GL_COLOR_BUFFER_BIT);

	

	float vertices[] = { 0.5f, -0.5f, 0.0f, 0.5f, -0.5f, -0.5f };
	float vertices2[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program.positionAttribute);

	program.SetModelMatrix(bmatrix);
	glDrawArrays(GL_TRIANGLES, 0, 3);

	glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices2);

	program.SetModelMatrix(lmatrix);
	glDrawArrays(GL_TRIANGLES, 0, 6);

	program.SetModelMatrix(rmatrix);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	glDisableVertexAttribArray(program.positionAttribute);


	if (gamestate == 1) {
		//win left player
	}
	if (gamestate == 2) {
		//win right player
	}


	SDL_GL_SwapWindow(displayWindow);
}

void Shutdown() {
	SDL_Quit();
}

int main(int argc, char* argv[]) {
	Initialize();

	while (gameIsRunning) {
		ProcessInput();
		Update();
		Render();
		SDL_Delay(50);
	}

	Shutdown();
	return 0;
}
