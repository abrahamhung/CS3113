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
#include "Player.h"
#include "Printer.h"

#include<stdio.h>
#include<iostream>
#include<Windows.h>

Printer* printr;


struct level;
struct GameState {
	Player* player;
	Entity* enemies;
	int numEnemies;
	Entity* tiles;
	Entity* finishline;
	level* levels;
	int currentLevel;
	int numLevels;
	bool levelOver;
};
GameState state;
struct level {
	glm::vec3 playerposition;
	int tilecount;
	glm::vec3* tilexy;
	GLuint tileTextures;
	int enemycount;
	glm::vec3* enemyxy;
	GLuint enemyTextures;
	aitype* enemies;
	glm::vec3 finishposition;
};
void loadLevel(level map);

int lives = 3;

SDL_Window* displayWindow;
bool gameIsRunning = true;
ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint LoadTexture(const char* filePath);


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

	state.currentLevel = 0;
	state.levelOver = true;

	// Initialize Game Objects
	state.finishline = new Entity();
	state.finishline->textureID = LoadTexture("finish.png");

	// Initialize Player
	
		state.player = new Player();
		state.player->position = glm::vec3(-4, 0, 0);
		state.player->acceleration = glm::vec3(4, 0, 0);
		state.player->maxVel = glm::vec3(2, 5, 0);
		state.player->textureID = LoadTexture("triangleAtlas.png");

		state.player->animRight = new int[4]{ 0, 4, 8, 12 };
		state.player->animLeft = new int[4]{ 1, 5, 9, 13 };
		state.player->animDown = new int[4]{ 2, 6, 10, 14 };

		state.player->animIndices = state.player->animDown;
		state.player->animFrames = 4;
		state.player->animTime = .05f;
		state.player->animCols = 4;
		state.player->animRows = 4;

		state.player->active = true;
	


	GLuint grassTexture = LoadTexture("grass.png");
	GLuint spikeTex = LoadTexture("spike.png");

	glm::vec3* tiles;
	glm::vec3* enemies;

	tiles = new glm::vec3[10];
	for (int i = 0; i < 10; i++) {
		tiles[i] = glm::vec3(i - 4.5, -3, 0);
	}
	enemies = new glm::vec3[1];
	for (int i = 0; i < 1; i++) {
		enemies[i] = glm::vec3(0, -2, 0);
	}
	aitype* enemytypes = new aitype[1];
	enemytypes[0] = potato;
	level* lvOne = new level{
		glm::vec3(-4,0,0),	//pos player
		10,					//num tiles
		tiles,				//pos tiles
		grassTexture,		//tex tiles
		1,					//num enemies
		enemies,			//pos enemy
		spikeTex,			//tex enemy
		enemytypes,			//typ enemy
		glm::vec3(4,-2,0),	//pos finish
	};


	tiles = new glm::vec3[6];
	tiles[0] = glm::vec3(-4.5, -3, 0);
	tiles[1] = glm::vec3(-3.5, -3, 0);
	tiles[2] = glm::vec3(-2.5, -3, 0);
	tiles[3] = glm::vec3(2.5, -3, 0);
	tiles[4] = glm::vec3(3.5, -3, 0);
	tiles[5] = glm::vec3(4.5, -3, 0);

	level* lvTwo = new level{
		glm::vec3(-4,0,0),	//pos player
		6,					//num tiles
		tiles,				//pos tiles
		grassTexture,		//tex tiles
		0,					//num enemies
		NULL,				//pos enemy
		spikeTex,			//tex enemy
		NULL,				//typ enemy
		glm::vec3(4,-2,0),	//pos finish
	};

	tiles = new glm::vec3[10];
	for (int i = 0; i < 10; i++) {
		tiles[i] = glm::vec3(i - 4.5, -3, 0);
	}
	enemies = new glm::vec3[1];
	for (int i = 0; i < 1; i++) {
		enemies[i] = glm::vec3(0, -2, 0);
	}
	enemytypes = new aitype[1];
	enemytypes[0] = jumper;
	level* lvThr = new level{
		glm::vec3(-4,0,0),	//pos player
		10,					//num tiles
		tiles,				//pos tiles
		grassTexture,		//tex tiles
		1,					//num enemies
		enemies,			//pos enemy
		spikeTex,			//tex enemy
		enemytypes,			//typ enemy
		glm::vec3(4,-2,0),	//pos finish
	};

	state.numLevels = 3;
	state.levels = new level[4];
	state.levels[1] = *lvOne;
	state.levels[2] = *lvTwo;
	state.levels[3] = *lvThr;
	loadLevel(state.levels[1]);
	//create printer object to write text
	printr = new Printer();
	printr->textureID = LoadTexture("font1.png");
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

	if (keys[SDL_SCANCODE_RETURN]) {
		if (state.levelOver) {
			state.currentLevel += 1;
			state.levelOver = false;
			loadLevel(state.levels[state.currentLevel]);
		}
		else {
			if (!state.player->active) {
				lives -= 1;
			}
			loadLevel(state.levels[state.currentLevel]);
		}
	}

	if (keys[SDL_SCANCODE_UP]) {
		state.player->movement.y = 1.0f;
		state.player->animIndices = state.player->animDown;
	}
	else if (keys[SDL_SCANCODE_DOWN]) {
		state.player->movement.y = -1.0f;
		state.player->animIndices = state.player->animDown;
	}
	if (keys[SDL_SCANCODE_RIGHT]) {
		state.player->movement.x = 1.0f;
		state.player->animIndices = state.player->animRight;	//actually its a sword
	}
	else if (keys[SDL_SCANCODE_LEFT]) {
		state.player->movement.x = -1.0f;
		state.player->animIndices = state.player->animLeft;
	}

	if (glm::length(state.player->movement) > 1.0f) {
		state.player->movement = glm::normalize(state.player->movement);
	}

}

float lastTicks = 0.0f;
float TIMESTEP = .0166666f;

void Update() {
	float ticks = (float)SDL_GetTicks() / 1000.0f;	//find current time
	float deltaTime = ticks - lastTicks;			//find change in time
	lastTicks = ticks;								//update game time
	
	while (deltaTime >= TIMESTEP) {
		deltaTime -= TIMESTEP;
		state.player->Update(TIMESTEP);
		for (int i = 0; i < state.levels[state.currentLevel].enemycount; i++) {
			state.enemies[i].ai(state.player->position);
			state.enemies[i].Update(TIMESTEP);
		}
	}

	if (state.player->collidecheck(state.finishline)) {
		state.levelOver = true;
		state.player->active = false;
	}


	//invert player position
	glm::vec3 offset = glm::vec3(0);
	offset.x = -state.player->position.x;
	offset.y = -state.player->position.y;

	//offset drawing of all items to keep character centered
	for (int i = 0; i < state.levels[state.currentLevel].enemycount; i++) {
		state.enemies[i].Update(0);
		state.enemies[i].modelMatrix = glm::translate(state.enemies[i].modelMatrix, offset);
	}
	for (int i = 0; i < state.levels[state.currentLevel].tilecount; i++) {
		state.tiles[i].Update(0);
		state.tiles[i].modelMatrix = glm::translate(state.tiles[i].modelMatrix, offset);

	}
	state.finishline->Update(0);
	state.finishline->modelMatrix = glm::translate(state.finishline->modelMatrix, offset);
	state.player->Update(0);
	state.player->modelMatrix = glm::translate(state.player->modelMatrix, offset);


	lastTicks -= deltaTime;		//push extra time to lastTicks to be counted in next cycle
}


void Render() {
	glClear(GL_COLOR_BUFFER_BIT);
	
	std::string s = "lives: " + std::to_string(lives);
	//failed
	if (lives <= 0) {
		printr->print(&program, "you loser!", -4, 2);
		printr->print(&program, s, -4, 3);
	}
	//out of levels
	else if (state.currentLevel == state.numLevels && state.levelOver) {
	printr->printBound(&program, "You're Win!", -4, 1, 4);
	}
	//level has ended
	else if (state.levelOver) {
		printr->print(&program, "Palteformer", -4, 3);
		printr->print(&program, "Press Enter", -4, 2);
		printr->print(&program, "to continue", -4, 1);
								
	}
	//player is dead
	else if (!state.player->active) {
		printr->print(&program, "press enter", -4, 2);
		printr->print(&program, "to restart", -4, 1);
		printr->print(&program, s, -4, 3);
	}
	//draw level
	else {
		printr->print(&program, s, -4, 3);
		//draw tiles
		for (int i = 0; i < state.levels[state.currentLevel].tilecount; i++) {
			state.tiles[i].Render(&program);
		}
		//draw enemies
		for (int i = 0; i < state.levels[state.currentLevel].enemycount; i++) {
			state.enemies[i].Render(&program);
		}
		//draw finish line
		state.finishline->Render(&program);
		//draw player
		state.player->Render(&program);
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
		SDL_Delay(10);
	}

	Shutdown();
	return 0;
}

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

void loadLevel(level map) {
	//update player position
	state.player->position = map.playerposition;
	state.player->velocity = glm::vec3(0);

	//load terrain tiles]
	if (map.tilecount > 0) {
		state.tiles = new Entity[map.tilecount];
		for (int i = 0; i < map.tilecount; i++) {
			state.tiles[i].textureID = map.tileTextures;
			state.tiles[i].position = map.tilexy[i];
			state.tiles[i].Update(0);
			state.tiles[i].active = true;
		}
	}
	//update player tile collision list
	state.player->terrain = state.tiles;
	state.player->terraincount = map.tilecount;

	//load enemy objects 
	if (map.enemycount > 0) {
		state.enemies = new Entity[map.enemycount];
		for (int i = 0; i < map.enemycount; i++) {
			state.enemies[i].textureID = map.enemyTextures;
			state.enemies[i].position = map.enemyxy[i];
			state.enemies[i].type = map.enemies[i];
			state.enemies[i].active = true;
			state.enemies[i].animIndices = NULL;
			state.enemies[i].Update(0);
		}
	}
	state.numEnemies = map.enemycount;
	//update player enemy list
	state.player->enemies = state.enemies;
	state.player->enemycount = map.enemycount;

	//place finish line
	state.finishline->position = map.finishposition;
	state.finishline->Update(0);

	state.player->Update(0);
	//activate player
	state.player->active = true;
}