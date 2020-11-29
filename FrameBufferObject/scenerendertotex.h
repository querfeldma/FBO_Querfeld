#ifndef SCENERENDERTOTEX_H
#define SCENERENDERTOTEX_H

#include "scene.h"
#include "glslprogram.h"
#include "cube.h"
#include "objmesh.h"

#include <glm/glm.hpp>
#include "fbo.h"
#include "entity.h"

#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>
#include "overlayrenderer.h"
#include "postprocessingrenderer.h"

struct uiData
{
    bool cow_auto_rotate;
    glm::vec3 cow_angle;
    glm::vec3 cow_position;
    bool cube_auto_rotate;
    glm::vec3 cube_angle;
    glm::vec3 cube_position;
    glm::vec3 camera_position;
    bool camera_auto_rotate;
    bool show_overlay;
    bool apply_negative;
    bool apply_colorswizzle;
    bool apply_gaussianblur;
};

class SceneRenderToTex : public Scene
{
private:
    //GUI
    float tPrev, rotSpeed;
    uiData uiData;

    //FBO
    Fbo* fbo; 
    Fbo* overlay_fbo;
    Fbo* post_fbo;

    //Entity
    Cube* cube;
    Entity* cow;

    //Rendering
    Texture* tex;
    GLSLProgram prog;
    Overlay* overlay;
    OverlayRenderer* overlayRenderer;
    PostProcessingRenderer* postProcessingRenderer;


    glm::mat4 getModelMatrix(Cube* cube);
    glm::mat4 getModelMatrix(Entity* entity);
    void setMatrices();
    void compileAndLinkShader();
    void renderGUI();
    void renderToTexture();
    void renderCubeToTexture();
    void renderScene(GLuint textureHandle);

public:
    SceneRenderToTex();
    ~SceneRenderToTex();
    

    void initScene();
    void update( float t );
    void render();
    void resize(int, int);
};

#endif // SCENERENDERTOTEX_H
