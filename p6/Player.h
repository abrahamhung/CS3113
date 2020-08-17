#include"Entity.h"
#include<vector>
#pragma once

class Player : public Entity {
public:
	glm::vec3 maxVel = glm::vec3(2, 2, 2);
	float direction;
	float rotationspeed;
	int type = 0;

	bool tcoll = false;
	bool bcoll = false;
	bool lcoll = false;
	bool rcoll = false;

	Entity* terrain = NULL;
	int terraincount = 0;
	Entity* enemies = NULL;
	int enemycount = 0;

	//specific animindices that are expected from player movement
	int* animRight = NULL;
	int* animLeft = NULL;
	int* animUp = NULL;
	int* animDown = NULL;

	Player();

	void Update(float deltaTime);
	void Render(ShaderProgram* program);
	void DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index);
	void collide(Entity* entitylist, int number);
	void collide(std::vector<Entity*> entitylist);
	bool collidecheck(Entity* other);
};