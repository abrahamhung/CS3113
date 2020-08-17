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


class Printer {
public:
	glm::vec3 position = glm::vec3(0);		//position
	GLuint textureID;		//teexture
	glm::mat4 modelMatrix;	//projection matrix, updated by position vector

	int cols = 16;
	int rows = 16;

	Printer();

	void print(ShaderProgram* program, std::string text, float posx, float posy);
	void printBound(ShaderProgram* program, std::string text, float aposx, float aposy, float bposx);
private:
	void DrawSpriteFromTextureAtlas(ShaderProgram* program, GLuint textureID, int index);
};
