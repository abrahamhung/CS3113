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

#include "Entity.h"


#define MAX_ENEMIES 5

struct GameState {
	Entity* player;
	Entity* enemies;
	Entity* endingText;
	Entity* winningText;
};
int points = 0;

GameState state;
int active_enemies;

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
	displayWindow = SDL_CreateWindow("Ex Terminate", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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

	glClearColor(0.2f, 0.5f, .8f, 1.0f);
	glEnable(GL_BLEND);

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);


	// Initialize Game Objects
	GLuint64 enemyTextureID = LoadTexture("enemies.png");
	state.enemies = new Entity[MAX_ENEMIES];
	active_enemies = 0;
	for (int i = 0; i < MAX_ENEMIES; i++) {
		state.enemies[i].textureID = enemyTextureID;

		//animation but it has only one frame
		state.enemies[i].animFrames = 1;
		state.enemies[i].animIndex = 0;
		state.enemies[i].animTime = 0;
		state.enemies[i].animCols = 3;
		state.enemies[i].animRows = 1;
		state.enemies[i].animRight = new int[1]{ 0 };
		state.enemies[i].animLeft = new int[1]{ 1 };
		state.enemies[i].animUp = new int[1]{ 2 };
		state.enemies[i].animIndices = state.enemies[i].animRight;
		state.enemies[i].position = glm::vec3(4, i-2, 0);
		state.enemies[i].velocity = glm::vec3(0, 0, 0);
		state.enemies[i].active = false;
	}



	// Initialize Player
	state.player = new Entity();
	state.player->position = glm::vec3(-4,0,0);
	state.player->velocity = glm::vec3(0);
	state.player->acceleration = glm::vec3(0, 1, 0);
	state.player->movement = glm::vec3(0);
	state.player->textureID = LoadTexture("player.png");

	state.player->animRight = new int[1]{ 0 };
	state.player->animLeft = new int[1]{ 1 };

	state.player->animIndices = state.player->animRight;
	state.player->animFrames = 1;
	state.player->animIndex = 0;
	state.player->animTime = 0;
	state.player->animCols = 2;
	state.player->animRows = 1;

	state.player->active = true;


	state.endingText = new Entity();
	state.endingText->position = glm::vec3(0);
	state.endingText->textureID = LoadTexture("texLose.png");
	state.endingText->animCols = 1;
	state.endingText->animRows = 1;
	state.endingText->active = false;

	state.winningText = new Entity();
	state.winningText->position = glm::vec3(0);
	state.winningText->textureID = LoadTexture("texWin.png");
	state.winningText->animCols = 1;
	state.winningText->animRows = 1;
	state.winningText->active = false;

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

		}
	}

	const Uint8* keys = SDL_GetKeyboardState(NULL);

	if (keys[SDL_SCANCODE_UP]) {
		state.player->movement.y = 1.0f;
		state.player->animIndices = state.player->animRight;
	}
	else if (keys[SDL_SCANCODE_DOWN]) {
		state.player->movement.y = -1.0f;
		state.player->animIndices = state.player->animRight;
	}
	if (keys[SDL_SCANCODE_RIGHT]) {
		//fire a projectile
		state.player->animIndices = state.player->animLeft;	//actually its a sword
	}

	if (glm::length(state.player->movement) > 1.0f) {
		state.player->movement = glm::normalize(state.player->movement);
	}

}

float lastTicks = 0.0f;
float TIMESTEP = .0166666f;
float spawnTimer = 0.0f;

void Update() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;	//find current time
	float deltaTime = ticks - lastTicks;			//find change in time
	lastTicks = ticks;								//update game time
	spawnTimer += deltaTime;

	
	if (points >= 5) {
		//win
		state.winningText->active = true;
		for (int i = 0; i < MAX_ENEMIES; i++) {
			state.enemies[i].active = false;
		}
	}
	else if (state.player->active == false) {
		//player has lost. do something.
		state.endingText->active = true;	//display ending text.
		for (int i = 0; i < MAX_ENEMIES; i++) {
			state.enemies[i].active = false;
		}
	}
	else {
		if (spawnTimer >= 50) {
			for (int i = 0; i < MAX_ENEMIES; i++) {
				if (state.enemies[i].active == false) {
					state.enemies[i].active = true;
					state.enemies[i].position = glm::vec3(4, (rand() % 6 - 3), 0);
					state.enemies[i].type = rand() % 3 + 1;
					state.enemies[i].velocity = glm::vec3(0);
					spawnTimer -= 100;
					break;
				}
			}
		}

		while (deltaTime >= TIMESTEP) {
			deltaTime -= TIMESTEP;
			state.player->Update(TIMESTEP);
			for (int i = 0; i < MAX_ENEMIES; i++) {
				state.enemies[i].Update(TIMESTEP);

				//check for collision after updating the position of the enemy
				if (state.player->collide(state.enemies[i])) {
					if (state.player->animIndices == state.player->animRight) {
					state.player->active = false;
					}
					else {
						state.enemies[i].active = false;
						points++;
					}
				}
			}


		}
	}

	lastTicks -= deltaTime;		//push extra time to lastTicks to be counted in next cycle
}


void Render() {
	glClear(GL_COLOR_BUFFER_BIT);


	state.player->Render(&program);
	for (int i = 0; i < MAX_ENEMIES; i++) {
		state.enemies[i].Render(&program);
	}

	state.endingText->Render(&program);
	state.winningText->Render(&program);
	//man i really need to get aroudn to writing a class to print text


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