#pragma once
#include "Entity.h"
//
//enum aitype { potato, walker, jumper };
//enum aistate { idle, move, attack };

class Enemy : public Entity {
public:
	bool active = true;
	bool attacked = false;
	glm::vec3 position = glm::vec3(0);

	Enemy();
	Enemy(glm::vec3 position);

	void Update(float deltaTime);
	bool collide(Entity& other);

	void ai(Entity* player);
	aitype type = aitype{ potato };
	aistate status = aistate{ idle };
};