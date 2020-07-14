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

class Entity {
public:
	bool active;

	glm::vec3 position = glm::vec3(0);		//position
	glm::vec3 velocity = glm::vec3(0);		//change in position
	glm::vec3 acceleration = glm::vec3(0);	//change in velocity
	glm::vec3 movement = glm::vec3(0);		//player input
	
	int type = 0;
	int height = 1;
	int width = 1;

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

	Entity();

	void Update(float deltaTime);
	void Render(ShaderProgram* program);
	void DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index);
	bool collide(Entity& other);
};