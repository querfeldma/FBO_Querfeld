#pragma once
#include "glutils.h"
#include <glm/glm.hpp>
#include <iostream>

class Overlay
{
public:
	Overlay(GLuint texture_handle, glm::vec2 position, glm::vec2 scale) : texture_handle(texture_handle), position(position), scale(scale)
	{
		width = scale.x;
		height = scale.y;
	}

	glm::vec2 getPosition() { return position; };

	glm::vec2 getScale() { return scale; };

	GLuint getHandle() { return texture_handle; };

private:
	GLuint texture_handle;
	glm::vec2 position;
	glm::vec2 scale;
	float width;
	float height;
};