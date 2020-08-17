#include "Printer.h"

Printer::Printer()
{
	position = glm::vec3(0);
	modelMatrix = glm::mat4(1.0f);
}

void Printer::print(ShaderProgram* program, std::string text, float posx, float posy)
{
	position = glm::vec3(posx, posy, 0);

	char character;
	for (int i = 0; i < text.length(); i++) {
		character = text[i];
		
		if (position.x > 4.5) {
			position.x = posx;
			position.y -= .75;
		}
		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, position);
		DrawSpriteFromTextureAtlas(program, textureID, character);
		position.x += .5f;
	}
}

void Printer::printBound(ShaderProgram* program, std::string text, float aposx, float aposy, float bposx) {
	position = glm::vec3(aposx, aposy, 0);
	float space = (bposx - aposx) / (text.length()-1);
	char character;
	for (int i = 0; i < text.length(); i++) {
		character = text[i];

		//if (position.x > bposx) {
		//	position.x = aposx;
		//	position.y -= .75;
		//}
		modelMatrix = glm::mat4(1);
		modelMatrix = glm::translate(modelMatrix, position);
		DrawSpriteFromTextureAtlas(program, textureID, character);
		position.x += space;
	}
}


void Printer::DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index)
{
	program->SetModelMatrix(modelMatrix);
	float u = (float)(index % cols) / (float)cols;
	float v = (float)(index / cols) / (float)rows;

	float width = 1.0f / (float)cols;
	float height = 1.0f / (float)rows;

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
