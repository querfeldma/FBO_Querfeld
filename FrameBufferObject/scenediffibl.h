#pragma once

#include "scene.h"
#include "glslprogram.h"
#include "objmesh.h"
#include "skybox.h"

#include <glm/glm.hpp>
#include <texture.h>
#include "entity.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "equirectangularHDR.h"
#include "cube.h"
#include "fboenvironmentmap.h"

struct uiDataIBL
{
	bool cow_auto_rotate;
	glm::vec3 cow_angle;
	glm::vec3 cow_position;
	bool cube_auto_rotate;
	glm::vec3 cube_angle;
	glm::vec3 cube_position;
	glm::vec3 camera_position;
	bool camera_auto_rotate;
	float gamma;
	bool use_gamma;
	bool use_schlick;
	bool use_schlick_roughness;
	float roughness;
	bool show_env_map;
	bool show_irr_map;
};

class SceneDiffIbl : public Scene
{
private:
	uiDataIBL uiData;

	GLuint textureHandle;
	GLSLProgram prog, hdrSkyboxProg, displayCubeProg;
	GLSLProgram* hdrEquirectangularToCubeMapProg;
	GLSLProgram* hdrDiffuseProg;
	EquirectangularHDR milkyway, road, walk_of_fame;
	FboEnvironmentMap fbo;

	Entity* cow;
	Cube* cube;

	float camAngle, tPrev, rotSpeed;

	void setMatrices(GLSLProgram &);
	glm::mat4 getModelMatrix(Entity* entity);
	void compileAndLinkShader();
	void renderGUI();
	void renderToMap(int mode, GLuint source, GLuint destination, GLSLProgram* shader, glm::mat4 captureProjection, glm::mat4 captureViews[]);

public:
	SceneDiffIbl();

	~SceneDiffIbl();

	void initScene();
	void update(float t);
	void render();
	void resize(int, int);

};
