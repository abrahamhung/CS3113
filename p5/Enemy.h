#pragma once
#include "Entity.h"
//
//enum aitype { potato, walker, jumper };
//enum aistate { idle, move, attack };

class Enemy : public Entity {
public:
	bool active = true;
	bool attacked = false;

	Enemy();

	void Update(float deltaTime);
	bool collide(Entity& other);

	void ai(Entity* player);
	aitype type = aitype{ potato };
	aistate status = aistate{ idle };
	bool jumper;
};