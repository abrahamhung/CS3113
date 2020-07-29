#include "Entity.h"
#include<Windows.h>

Entity::Entity()
{
	position = glm::vec3(0);		
	velocity = glm::vec3(0);		
	acceleration = glm::vec3(0);	
	movement = glm::vec3(0);		
	modelMatrix = glm::mat4(1.0f);
	active = true;
}

void Entity::Update(float deltaTime)
{
	if (!active) { return; }
	animIndex = 0;
	animIndices = NULL;

	velocity += movement * acceleration * deltaTime;
	position += velocity * deltaTime;

	modelMatrix = glm::mat4(1.0f);
	modelMatrix = glm::translate(modelMatrix, position);
}

bool Entity::collide(Entity& other) {
	if (active && other.active) {	//check that both objects are active
		if (fabs(position.y - other.position.y) < (height + other.height) / 2) {
			if (fabs(position.x - other.position.x) < (width + other.width) / 2) {
				//has collided
				return true;
			}
		}
	}
	return false;
}

void Entity::ai(glm::vec3 playerposition){
		std::string s = std::to_string(playerposition.x);
			
	switch(type){
		case jumper:
			OutputDebugString(s.c_str());
			if (fabs(position.x - playerposition.x) < .5) {
				velocity.y = 3;
			}
			else {
				velocity.y = 0;
			}
	}
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
	if (!active) { return; }

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