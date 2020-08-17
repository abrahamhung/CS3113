#define GL_SILENCE_DEPRECATION

#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#define GL_GLEXT_PROTOTYPES 1
#include <SDL.h>
#include <SDL_mixer.h>
#include <SDL_opengl.h>
#include "glm/mat4x4.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "ShaderProgram.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define RESOURCE_FILES
#include "Entity.h"
#include "Player.h"
#include "Enemy.h"
#include "Printer.h"
#include<Windows.h>
#include<vector>
#include<string>

Mix_Music* music;
Mix_Chunk* bounce;
Printer* printr;

struct level;
struct GameState {
	Player* player;		//there is only allowed one player object
	Entity* finishline;	//there is allowed one finish object
	std::vector<level*> levels;
	int currentLevel;
	bool levelOver;
	//these two are vectors so that it's easier to handle loading
	std::vector<Entity*> enemies;
	GLuint enemytexture;
	std::vector<Entity*> tiles;		
	GLuint tiletexture;
};
GameState state;
struct level {
	int width;
	int height;
	int* map;
};
void loadLevel(level map);

int lives = 1;

SDL_Window* displayWindow;
bool gameIsRunning = true;
ShaderProgram program;
glm::mat4 viewMatrix, modelMatrix, projectionMatrix;

GLuint LoadTexture(const char* filePath);


void Initialize() {
	SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO);
	//SDL_Init(SDL_INIT_VIDEO);

	Mix_OpenAudio(44100, MIX_DEFAULT_FORMAT, 2, 4096);
	music = Mix_LoadMUS("VCDOST7P2810.mp3");
	//Mix_VolumeMusic(MIX_MAX_VOLUME / 2);
	Mix_PlayMusic(music, -1);

	bounce = Mix_LoadWAV("146726__leszek-szary__jumping.wav");
	


	displayWindow = SDL_CreateWindow("Floaty Rocks", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 640, 480, SDL_WINDOW_OPENGL);
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
	state.finishline = new Entity();
	state.finishline->textureID = LoadTexture("finish.png");

	// Initialize Player
	
		state.player = new Player();
		state.player->position = glm::vec3(-4, 0, 0);
		state.player->acceleration = glm::vec3(2, .2, 0);
		state.player->direction = 0.0f;
		state.player->maxVel = glm::vec3(5, 5, 0);
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


	


	state.tiletexture = LoadTexture("grass.png");
	state.enemytexture = LoadTexture("spike.png");
	level *newLevel = new level;
	newLevel->width = 30;
	newLevel->height = 30;
	newLevel->map = new int[900]{
		3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,
		3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		3,0,0,1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,	//5
		3,0,0,0,0,0,3,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,	//10
		3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		3,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		3,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,4,0,0,0,0,3,	//15
		3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,2,4,0,0,0,3,
		3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,4,0,0,0,0,0,0,0,0,0,3,	//20
		3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,4,0,0,0,0,0,0,0,0,0,0,3,
		3,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		3,0,0,0,0,0,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,0,0,0,0,3,	//25
		3,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,3,
		3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3,3	//30
	};
	state.levels.push_back(newLevel);

	state.currentLevel = -1;
	state.levelOver = true;

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
			std::string s = "level call " + std::to_string(state.currentLevel);
			OutputDebugString(s.c_str());
			if (state.currentLevel >= state.levels.size()-1) {	//have beaten all level
				//restart the game
				OutputDebugString("restarting");
				state.currentLevel = 0;
				state.levelOver = false;
				loadLevel(*state.levels[state.currentLevel]);
			}
			else if(state.currentLevel < state.levels.size()-1){
				OutputDebugString("loading next");
				state.currentLevel += 1;
				state.levelOver = false;
				loadLevel(*state.levels[state.currentLevel]);
			}
		}
		else {
			if (!state.player->active) {
				state.player->active;
				lives -= 1;
			}
			loadLevel(*state.levels[state.currentLevel]);
		}
		SDL_Delay(200);
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
		for (size_t i = 0; i < state.enemies.size(); i++) {
			state.enemies[i]->ai(state.player);
			state.enemies[i]->Update(TIMESTEP);
		}
	}


	//terrain collision
	state.player->Update(0);
	state.player->tcoll = false; 
	state.player->bcoll = false; 
	state.player->rcoll = false; 
	state.player->lcoll = false;
	state.player->Update(0);
	state.player->collide(state.tiles);

	if (state.player->tcoll || state.player->bcoll || state.player->rcoll || state.player->lcoll) {
		//state.player->active = false;
		Mix_PlayChannel(-1, bounce, 0);
	}


	state.player->Update(0);
	//cenemy collision
	state.player->tcoll = state.player->bcoll = state.player->lcoll = state.player->rcoll = false;
	state.player->collide(state.enemies);

	if (state.player->tcoll || state.player->lcoll || state.player->rcoll || state.player->lcoll) {
		state.player->active = false;
	}


	//contact to finish object
	if (state.player->collidecheck(state.finishline)) {
		state.levelOver = true;
	}


	//invert player position
	glm::vec3 offset = glm::vec3(0);
	offset.x = -state.player->position.x;
	offset.y = -state.player->position.y;

	//offset drawing of all items to keep character centered
	for (size_t i = 0; i < state.enemies.size(); i++) {
		state.enemies[i]->Update(0);	//to generate new modelmatrix, 
		state.enemies[i]->modelMatrix = glm::translate(state.enemies[i]->modelMatrix, offset);
	}
	for (size_t i = 0; i < state.tiles.size(); i++) {
		state.tiles[i]->Update(0);
		state.tiles[i]->modelMatrix = glm::translate(state.tiles[i]->modelMatrix, offset);

	}
	state.finishline->Update(0);
	state.finishline->modelMatrix = glm::translate(state.finishline->modelMatrix, offset);
	state.player->Update(0);
	//state.player->modelMatrix = glm::translate(state.player->modelMatrix, offset);


	lastTicks -= deltaTime;		//push extra time to lastTicks to be counted in next cycle
}


void Render() {
	glClear(GL_COLOR_BUFFER_BIT);
	
	std::string s = "lives: " + std::to_string(lives);
	//failed
	if (lives <= 0) {
		printr->print(&program, "you lose!", -4, 2);
		printr->print(&program, s, -4, 3);
	}
	//out of levels
	else if (state.currentLevel == state.levels.size() && state.levelOver) {
	printr->printBound(&program, "You're Win!", -4, 1, 4);
	printr->print(&program, "Press Enter", -4, 0);
	printr->print(&program, "to play again", -4, -1);
	}
	//level has ended
	else if (state.levelOver) {
		printr->print(&program, "Aesteroieders", -4, 3);
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
		for (size_t i = 0; i < state.tiles.size(); i++) {
			state.tiles[i]->Render(&program);
		}
		//draw enemies
		for (size_t i = 0; i < state.enemies.size(); i++) {
			state.enemies[i]->Render(&program);
		}
		//draw finish line
		state.finishline->Render(&program);
		//draw player
		state.player->Render(&program);
	}

	SDL_GL_SwapWindow(displayWindow);
}


void Shutdown() {
	Mix_FreeMusic(music);
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

	for (int i = 0; i < state.enemies.size(); i++) {
		free(state.enemies[i]);
	}
	state.enemies.clear();
	for (int i = 0; i < state.tiles.size(); i++) {
		free(state.tiles[i]);
	}
	state.tiles.clear();
	std::string s = "loading " + std::to_string(state.currentLevel);
	OutputDebugString(s.c_str());
	//start loading map from (0,0) - this means that everything will be offset
	for (int i = 0; i < map.height; i++) {
		for (int j = 0; j < map.width; j++) {
			int object = map.map[(i * map.width) + j];
			if (object == 0) {
			}
			else if (object == 1) {
				state.player->position = glm::vec3(j, -i, 0);
			}
			else if (object == 2) {
				state.finishline->position = glm::vec3(j, -i, 0);
			}
			else if (object == 3) {
				state.tiles.push_back(new Entity(glm::vec3(j, -i, 0)));
				state.tiles[state.tiles.size() - 1]->textureID = state.tiletexture;
				state.tiles[state.tiles.size() - 1]->Update(0);
			}
			else if (object == 4) {
				state.enemies.push_back(new Entity(glm::vec3(j, -i, 0)));
				state.enemies[state.enemies.size() - 1]->textureID = state.enemytexture;
				state.enemies[state.enemies.size() - 1]->Update(0);
			}
		}
	}
	std::string b = " Positions:" 
	+ std::to_string(state.player->position.x) 
	+ std::to_string(state.player->position.y)
	+ std::to_string(state.finishline->position.x)
	+ std::to_string(state.finishline->position.y)
	+"\n" ;
	OutputDebugString(b.c_str());

	state.player->velocity = glm::vec3(0);
	state.player->Update(0);
	state.player->active;
	state.levelOver = false;
}