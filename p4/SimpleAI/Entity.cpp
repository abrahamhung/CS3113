#include "Entity.h"

Entity::Entity()
{
	position = glm::vec3(0);		
	velocity = glm::vec3(0);		
	acceleration = glm::vec3(0);	
	movement = glm::vec3(0);		

	modelMatrix = glm::mat4(1.0f);
}

void Entity::Update(float deltaTime)
{
	if (active) {

		if (type == 0) {	//reserved for player
			if (4 < position.y || position.y < -4) {
				active = false;
			}
		}
		else if (type == 1) {
			animIndices = animRight;
			velocity = glm::vec3(-2, 0, 0);
			acceleration = glm::vec3(0);
		}
		else if (type == 2) {
			animIndices = animLeft;
			acceleration = glm::vec3(-3, 0, 0);
			movement = glm::vec3(1, 1, 1);
		}
		else if (type == 3) {
			animIndices = animUp;
			movement = glm::vec3(1, 1, 1);
			acceleration.x = -.5;
			if (position.y > 0) {
				acceleration.y = -5;
			}
			else if (position.y < 0) {
				acceleration.y = 5;
			}
		}

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

		modelMatrix = glm::mat4(1.0f);
		modelMatrix = glm::translate(modelMatrix, position);

		if (position.x < -4) {
			active = false;
		}
	}
}

bool Entity::collide(Entity& other) {
	if (active && other.active) {	//check that both objects are active
		if (abs(position.y - other.position.y) < (height + other.height) / 2) {
			if (abs(position.x - other.position.x) < (width + other.width) / 2) {
				//has collided
				return true;
			}
		}
	}
	return false;
}

void Entity::DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index)
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

void Entity::Render(ShaderProgram* program) {
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