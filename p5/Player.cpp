#include "Player.h"

#include<windows.h>
Player::Player()
{
	position = glm::vec3(0);
	velocity = glm::vec3(0);
	acceleration = glm::vec3(0);
	movement = glm::vec3(0);

	modelMatrix = glm::mat4(1.0f);
}

void Player::Update(float deltaTime)
{
	if (!active) { return; }

	if (animIndices != NULL) {
		if (glm::length(movement) != 0) {
			animTime += deltaTime;
			if (animTime >= 0.25f)
			{
				animTime = 0.0f;
				animIndex++;
				if (animIndex >= animFrames)
				{
					animIndex = 0;
				}
			}
		}
		else {
			animIndex = 0;
		}
	}


	velocity += movement * acceleration * deltaTime;
	position += velocity * deltaTime;

	//terrain collision
	tcoll = bcoll = rcoll = lcoll = false;
	collide(terrain, terraincount);



	//check jump input 
	if (movement.y > 0) {
		if (bcoll) {	//check terrain contact
			velocity.y = 3;
		}
	}

	//cenemy collision
	tcoll = bcoll = lcoll = rcoll = false;
	collide(enemies, enemycount);

	if (tcoll || lcoll || rcoll) {
		active = false;
	}
	else if (bcoll) {
		velocity.y = 1;
	}


	//restrict max velocity
	if (velocity.x > maxVel.x) { velocity.x = maxVel.x; }
	else if (velocity.x < -maxVel.x) { velocity.x = -maxVel.x; }
	if (velocity.y > maxVel.y) { velocity.y = maxVel.y;	}
	else if (velocity.y < -maxVel.y) { velocity.y = -maxVel.y; }

	position += velocity * deltaTime;	//update position for velocity
		
	if (movement.x == 0) {				//decay velocity
		velocity.x = velocity.x / 2;
	}

	velocity.y -= 5 * deltaTime;		//"gravity"

	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, position);

	if (position.y < -4) {
		active = false;
	}	
}

void Player::collide(Entity* entitylist, int number) {
	for (int i = 0; i < number; i++) {
		Entity* other = &entitylist[i];
		if (collidecheck(other)) {	//check that there is a collision
			if (fabs(position.y - other->position.y) < ((height + other->height) / 2.0f)) {	//collision in y direction
				float space = ((height + other->height) / 2.0f);
				float distance = (fabs(position.y - other->position.y));
				float penetration = space - distance;
				if (position.y > other->position.y) {	//higher than
					bcoll = true;
					position.y += penetration;
					velocity.y = 0;
				}
				else {
					tcoll = true;
					position.y -= penetration;
					velocity.y = 0;
				}

			}
			else if (fabs(position.x - other->position.x) < (width + other->width) / 2) {	//collision in x direction
				float xdist = fabs(position.x - other->position.x);
				float penetration = fabs(xdist - ((height / 2) - (other->height / 2)));
				if (position.x > other->position.x) {	//to right of
					lcoll = true;
					position.x += penetration;
					velocity.x = 0;
				}
				else {
					rcoll = true;
					position.x -= penetration;
					velocity.x = 0;
				}
			}
		}
	}
}

bool Player::collidecheck(Entity* other) {
	if (active && other->active) {	//check that both objects are active
		if (fabs(position.y - other->position.y) < (height + other->height) / 2) {
			if (fabs(position.x - other->position.x) < (width + other->width) / 2) {
				//has collided
				return true;
			}
		}
	}
	return false;
}

void Player::DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index)
{
	float u = (float)(index % animCols) / (float)animCols;
	float v = (float)(index / animCols) / (float)animRows;

	float width = 1.0f / (float)animCols;
	float height = 1.0f / (float)animRows;

	float texCoords[] = { u, v + height, u + width, v + height, u + width, v,
		u, v + height, u + width, v, u, v };

	float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };

	glBindTexture(GL_TEXTURE_2D, textureID);

	glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
	glEnableVertexAttribArray(program->positionAttribute);

	glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
	glEnableVertexAttribArray(program->texCoordAttribute);

	glDrawArrays(GL_TRIANGLES, 0, 6);

	glDisableVertexAttribArray(program->positionAttribute);
	glDisableVertexAttribArray(program->texCoordAttribute);
}

void Player::Render(ShaderProgram* program) {
	if (active) {
		program->SetModelMatrix(modelMatrix);

		if (animIndices != NULL) {
			DrawSpriteFromTextureAtlas(program, textureID, animIndices[animIndex]);
			return;
		}

		float vertices[] = { -0.5, -0.5, 0.5, -0.5, 0.5, 0.5, -0.5, -0.5, 0.5, 0.5, -0.5, 0.5 };
		float texCoords[] = { 0.0, 1.0, 1.0, 1.0, 1.0, 0.0, 0.0, 1.0, 1.0, 0.0, 0.0, 0.0 };

		glBindTexture(GL_TEXTURE_2D, textureID);

		glVertexAttribPointer(program->positionAttribute, 2, GL_FLOAT, false, 0, vertices);
		glEnableVertexAttribArray(program->positionAttribute);

		glVertexAttribPointer(program->texCoordAttribute, 2, GL_FLOAT, false, 0, texCoords);
		glEnableVertexAttribArray(program->texCoordAttribute);

		glDrawArrays(GL_TRIANGLES, 0, 6);

		glDisableVertexAttribArray(program->positionAttribute);
		glDisableVertexAttribArray(program->texCoordAttribute);
	}
}