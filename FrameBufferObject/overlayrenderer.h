#pragma once
#include "overlay.h"
#include <vector>
#include <glslprogram.h>
#include <iostream>
#include <glm\ext\matrix_transform.hpp>

class OverlayRenderer 
{
public:
	OverlayRenderer();

	~OverlayRenderer();

	void render(std::vector<Overlay*>* overlays);

	void render(Overlay* overlay);

private:
	GLuint vaoID;
	GLuint vboID;
	GLSLProgram shader;

	void compileAndLinkShader();
	void loadToVAO(std::vector<GLfloat> points, int dimensions);
	glm::mat4 createTransformationMatrix(glm::vec2 position, glm::vec2 scale);
};

