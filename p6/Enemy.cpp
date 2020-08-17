#include "Enemy.h"
#include<Windows.h>

//bool active = true;
//bool attacked = false;

Enemy::Enemy() {
}
Enemy::Enemy(glm::vec3 coords) :position(coords){}

void Enemy::Update(float deltaTime) {

}
bool Enemy::collide(Entity& other) {
	return false;
}

void Enemy::ai(Entity* player) {
		OutputDebugString("ai running");
		if (jumper) {
			OutputDebugString("jumpting");
			if (fabs(position.x - player->position.x) < .5) {
				velocity.y = 3;
			}
			else {
				velocity.y = 0;
			}
		}
}



//aitype type = aitype{ potato };
//aistate status = aistate{ idle };
//bool jumper