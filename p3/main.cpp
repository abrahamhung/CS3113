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

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "lander.h"

struct GameState {
	lander* player;
};

GameState state;
int status = 0;
GLuint endTextureID, loseTextureID, platformTextureID;
glm::mat4 platformMatrix;

SDL_Window* displayWindow;
bool gameIsRunning = true;

ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint LoadTexture(const char* filePath) {
	int w, h, n;
	unsigned char* image = stbi_load(filePath, &w, &h, &n, STBI_rgb_alpha);

	if (image == NULL) {
		std::cout << "Unable to load image. Make sure the path is correct\n";
		assert(false);
	}

	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, w, h, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

	stbi_image_free(image);
	return textureID;
}


void Initialize() {
	SDL_Init(SDL_INIT_VIDEO);
	displayWindow = SDL_CreateWindow("LLm2", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);

#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, 640, 480);

	program.Load("shaders/vertex_textured.glsl", "shaders/fragment_textured.glsl");

	viewMatrix = glm::mat4(1.0f);
	modelMatrix = glm::mat4(1.0f);
	projectionMatrix = glm::ortho(-5.0f, 5.0f, -3.75f, 3.75f, -1.0f, 1.0f);

	program.SetProjectionMatrix(projectionMatrix);
	program.SetViewMatrix(viewMatrix);

	glUseProgram(program.programID);

	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// Initialize Game Objects

	// Initialize Player
	state.player = new lander();
	state.player->position = glm::vec3(0);
	state.player->movement = glm::vec3(0);
	state.player->acceleration = glm::vec3(1.0f, 1.0f, 0.0f);
	state.player->textureID = LoadTexture("lander2t.png");

	state.player->animRight = new int[4]{ 3, 7, 11, 15 };
	state.player->animLeft = new int[4]{ 1, 5, 9, 13 };
	state.player->animUp = new int[4]{ 2, 6, 10, 14 };
	state.player->animDown = new int[4]{ 0, 4, 8, 12 };

	state.player->animIndices = state.player->animRight;
	state.player->animFrames = 4;
	state.player->animIndex = 0;
	state.player->animTime = 0;
	state.player->animCols = 4;
	state.player->animRows = 4;

	//load ids because i still haven't mapped text
	endTextureID = LoadTexture("ending.png");
	loseTextureID = LoadTexture("lose.png");

	//place a platform object
	platformMatrix = glm::mat4(1.0f);
	platformMatrix = glm::translate(platformMatrix, glm::vec3(4.0f, -4.0f, 0.0f));
	platformTextureID = LoadTexture("platform.png");
}

void ProcessInput() {

	state.player->movement = glm::vec3(0);

	SDL_Event event;
	while (SDL_PollEvent(&event)) {
		switch (event.type) {
		case SDL_QUIT:
		case SDL_WINDOWEVENT_CLOSE:
			gameIsRunning = false;
			break;

		case SDL_KEYDOWN:
			switch (event.key.keysym.sym) {
			case SDLK_LEFT:
				// Move the player left
				break;

			case SDLK_RIGHT:
				// Move the player right
				break;

			case SDLK_SPACE:
				// Some sort of action
				break;
			}
			break; // SDL_KEYDOWN
		}
	}

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	state.player->animIndices = state.player->animDown;

	if (keys[SDL_SCANCODE_LEFT]) {
		state.player->movement.x = -1.0f;
		state.player->animIndices = state.player->animLeft;
	}
	else if (keys[SDL_SCANCODE_RIGHT]) {
		state.player->movement.x = 1.0f;
		state.player->animIndices = state.player->animRight;
	}
	if (keys[SDL_SCANCODE_UP]) {
		state.player->movement.y = 1.0f;
		state.player->animIndices = state.player->animUp;
	}


	if (glm::length(state.player->movement) > 1.0f) {
		state.player->movement = glm::normalize(state.player->movement);
	}

}

float lastTicks = 0.0f;

void Update() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;
	float deltaTime = ticks - lastTicks;
	lastTicks = ticks;


	if (status == 0) {
		state.player->Update(deltaTime);


		//check for ground contact
		if (state.player->position.y < -2.9) {
			//check for platform position
			//tbi variable platform position
			if (3.5 < state.player->position.x && state.player->position.x < 4.5) {
				status = 1;
			}
			else {
				status = 2;
			}
		}

		//check for out of bounds
		if (-5 > state.player->position.x || state.player->position.x > 5 || state.player->position.y > 5) {
			status = 2;
		}
	}
}


void Render() {
	glClear(GL_COLOR_BUFFER_BIT);


	if (status == 0) {
		state.player->Render(&program);


		//tbh i should really make a platform class to place a plaform and perform detection
		float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
		float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };
		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);
		program.SetModelMatrix(platformMatrix);
		glBindTexture(GL_TEXTURE_2D, platformTextureID);
		glDrawArrays(GL_TRIANGLES, 0, 6);
		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);
	}

	if (status == 1) {	//win
		glClear(GL_COLOR_BUFFER_BIT);

		float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
		float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		program.SetModelMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 1.0f)));
		glBindTexture(GL_TEXTURE_2D, endTextureID);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);
	}

	if (status == 2) {	//lose
		glClear(GL_COLOR_BUFFER_BIT);

		float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
		float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

		glVertexAttribPointer(program.positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program.positionAttribute);
		glVertexAttribPointer(program.texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program.texCoordAttribute);

		program.SetModelMatrix(glm::scale(glm::mat4(1.0f), glm::vec3(3.0f, 3.0f, 1.0f)));
		glBindTexture(GL_TEXTURE_2D, loseTextureID);
		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program.positionAttribute);
		glDisableVertexAttribArray(program.texCoordAttribute);
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
	}

	Shutdown();
	return 0;
}