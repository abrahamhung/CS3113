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

#pragma once

enum aitype { potato, walker, jumper };
enum aistate {idle, move, attack };

class Entity {
public:
	bool active = true;
	bool attacked = false;

	glm::vec3 position = glm::vec3(0);		//position
	glm::vec3 velocity = glm::vec3(0);		//change in position
	glm::vec3 acceleration = glm::vec3(0);	//change in velocity
	glm::vec3 movement = glm::vec3(0);		//movement vectors
	
	int height = 1;		//height and width default set to 1
	int width = 1;

	GLuint textureID = NULL;		//teexture

	glm::mat4 modelMatrix;	//projection matrix, updated by position vector

	//textures


	int* animIndices = NULL;
	int animFrames = 0;
	int animIndex = 0;
	float animTime = 0;
	int animCols = 0;
	int animRows = 0;

	Entity();
	Entity(glm::vec3 coords);

	void Update(float deltaTime);
	void Render(ShaderProgram* program);
	void DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index);
	bool collide(Entity& other);

	void ai(Entity* player);
	aitype type = aitype{ potato };
	aistate status = aistate{ idle };
};