#include"Entity.h"

#pragma once

class Player : public Entity {
public:
	bool active = true;
	glm::vec3 position = glm::vec3(0);		//position
	glm::vec3 velocity = glm::vec3(0);		//change in position
	glm::vec3 acceleration = glm::vec3(0);	//change in velocity
	glm::vec3 movement = glm::vec3(0);		//player input

	glm::vec3 maxVel = glm::vec3(2, 2, 2);

	int type = 0;
	int height = 1;
	int width = 1;

	bool tcoll = false;
	bool bcoll = false;
	bool lcoll = false;
	bool rcoll = false;

	Entity* terrain = NULL;
	int terraincount = 0;
	Entity* enemies = NULL;
	int enemycount = 0;

	GLuint textureID;		//teexture
	glm::mat4 modelMatrix;	//projection matrix, updated by position vector anyway

	//animations
	int* animRight = NULL;
	int* animLeft = NULL;
	int* animUp = NULL;
	int* animDown = NULL;

	int* animIndices = NULL;
	int animFrames = 0;
	int animIndex = 0;
	float animTime = 0;
	int animCols = 0;
	int animRows = 0;

	Player();

	void Update(float deltaTime);
	void Render(ShaderProgram* program);
	void DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index);
	void collide(Entity* other, int number);
	bool collidecheck(Entity* other);
};