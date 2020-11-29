#pragma once

#include <glm/glm.hpp>
#include "drawable.h"
#include "trianglemesh.h"

class Cube : public TriangleMesh
{
private:
    glm::vec3 position;
    glm::vec3 rotation;

public:
    Cube(GLfloat size = 1.0f);

	Cube(GLfloat size, glm::vec3 position, glm::vec3 rotation);

	glm::vec3 getPosition() { return position; };
	glm::vec3 getRotation() { return rotation; };

	void setPosition(glm::vec3 position)
	{
		this->position = position;
	}

	void setRotation(glm::vec3 rotation)
	{
		this->rotation = rotation;
	}
};
