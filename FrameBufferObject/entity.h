#pragma once
#include "objmesh.h"
#include <texture.h>

class Entity
{
private:
	std::unique_ptr<ObjMesh> model;
	Texture* texture;
	glm::vec3 position;
	glm::vec3 rotation;

public:
	Entity(const char* modelPath, const char* texturePath, glm::vec3 position, glm::vec3 rotation) : position(position), rotation(rotation)
	{
		model = ObjMesh::load(modelPath);	
		texture = new Texture(texturePath, Tex2D);
	}

	~Entity() 
	{
		delete texture;
	}

	void render() 
	{
		model->render();
	}

	Texture* getTexture() { return texture; };

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