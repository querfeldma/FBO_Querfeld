#pragma once
#include "overlay.h"
#include <vector>
#include <glslprogram.h>
#include <iostream>
#include <glm\ext\matrix_transform.hpp>
#include "fbo.h"

class PostProcessingRenderer
{
public:
	Overlay screen_quad;
	Fbo*	fbo;
	GLuint	screenTexture;

	PostProcessingRenderer(GLuint textureHandle, int width, int height);

	~PostProcessingRenderer();

	void render(bool negative, bool color, bool gauss);

	GLuint applyEffect(GLSLProgram* shader, GLuint texture);

private:
	GLuint vaoID;
	GLuint vboID;
	GLSLProgram* screenShader;
	GLSLProgram* negativeShader;
	GLSLProgram* colorSwizzleShader;
	GLSLProgram* gaussShader;


	void compileAndLinkShader(GLSLProgram* shader, const char* vertPath, const char* fragPath);
	void loadToVAO(std::vector<GLfloat> points, int dimensions);
	glm::mat4 createTransformationMatrix(glm::vec2 position, glm::vec2 scale);
};
