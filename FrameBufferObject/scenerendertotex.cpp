#include "scenerendertotex.h"

#include <iostream>
using std::cout;
using std::endl;
using std::cerr;

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/constants.hpp>
using glm::vec3;
using glm::mat4;

SceneRenderToTex::SceneRenderToTex() : tPrev(0.0f), rotSpeed(glm::pi<float>()/8.0f)
{
    //Init GUI
    uiData.cow_angle = glm::vec3(0.0f, 140.0f, 0.0f); 
    uiData.cow_position = glm::vec3(); // zeros
    uiData.cow_auto_rotate = true;

    uiData.cube_angle = glm::vec3();
    uiData.cube_position = glm::vec3();
    uiData.cube_auto_rotate = false;

    uiData.camera_position = vec3(0.0f, 1.5f, 2.0f);
    uiData.camera_auto_rotate = false;

    uiData.show_overlay = false;
    uiData.apply_negative = false;
    uiData.apply_colorswizzle = false;
    uiData.apply_gaussianblur = false;
}

SceneRenderToTex::~SceneRenderToTex() 
{
    delete fbo;
    delete cow;
    delete cube;

    delete overlay_fbo;
    delete overlay;

    delete post_fbo;

    delete overlayRenderer;
    delete postProcessingRenderer;
    delete tex;
}

void SceneRenderToTex::initScene()
{
    compileAndLinkShader();

    glEnable(GL_DEPTH_TEST);

    cow = new Entity("../media/spot/spot_triangulated.obj", "../media/spot/spot_texture.png", vec3(0.0f, 0.0f, 0.0f), vec3());
    cube = new Cube(1.0f, glm::vec3(), glm::vec3());

    projection = mat4(1.0f);
	prog.setUniform("Light.L", vec3(1.0f));
	prog.setUniform("Light.La", vec3(0.15f));
    fbo = new Fbo(512, 512);
    overlay_fbo = new Fbo(512, 512);
    post_fbo = new Fbo(width, height);

    //Init Overlay
    tex = new Texture("../media/texture/flower.png", Tex2D);
    overlay = new Overlay(overlay_fbo->getTextureHandle(), glm::vec2(0.75f, 0.75f), glm::vec2(0.25f, 0.25f));

    overlayRenderer = new OverlayRenderer();
    postProcessingRenderer = new PostProcessingRenderer(post_fbo->getTextureHandle(), width, height);
}


void SceneRenderToTex::update( float t )
{
    float deltaT = t - tPrev;
    if (tPrev == 0.0f) deltaT = 0.0f;
    tPrev = t;

    if (uiData.cow_auto_rotate) 
    {
        uiData.cow_angle.y += glm::degrees(rotSpeed * deltaT);
        if (uiData.cow_angle.y > 360.0f) 
            uiData.cow_angle.y -= 360.0f;
    }

    if (uiData.cube_auto_rotate)
    {
        uiData.cube_angle.y += glm::degrees(rotSpeed * deltaT);
        if (uiData.cube_angle.y > 360.0f)
            uiData.cube_angle.y -= 360.0f;
    }

    //Update Entities
    cow->setPosition(uiData.cow_position);
    cow->setRotation(glm::radians(uiData.cow_angle));
    cube->setPosition(uiData.cube_position);
    cube->setRotation(glm::radians(uiData.cube_angle));

    if (uiData.camera_auto_rotate) 
    {
        uiData.camera_position.x = 2.0f * cos(glm::radians(uiData.cow_angle.y));
        uiData.camera_position.z = 2.0f * sin(glm::radians(uiData.cow_angle.y));
    }
}

void SceneRenderToTex::render()
{
    //render cow to texture
    fbo->bind();
    view = glm::lookAt(vec3(0.0f, 0.0f, 2.5f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));    
    renderToTexture(); //scene A

    //render cube to texture from above
    view = glm::lookAt(vec3(0.0f, 3.0f, 0.1f), vec3(0.0f, 0.0f, 0.0f), vec3(0.0, 1.0f, 0.0f));
    overlay_fbo->bind();
    renderScene(fbo->getTextureHandle()); //scene C
    
    //render entire scene to post-processing quad
    post_fbo->bind();
    view = glm::lookAt(uiData.camera_position, vec3(0.0f, 0.0f, 0.0f), vec3(0.0f, 1.0f, 0.0f));
    renderScene(fbo->getTextureHandle()); //scene B


    if(uiData.show_overlay)
        overlayRenderer->render(overlay); //scene C

    //unbind
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    postProcessingRenderer->render(uiData.apply_negative, uiData.apply_colorswizzle, uiData.apply_gaussianblur); //scene D

    renderGUI();
}


void SceneRenderToTex::renderCubeToTexture() 
{
    prog.use();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex->getHandle());


    prog.setUniform("RenderTex", 1); //render cow-texture
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    projection = glm::perspective(glm::radians(50.0f), 1.0f, 0.3f, 100.0f);

    prog.setUniform("Light.Position", glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
    prog.setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
    prog.setUniform("Material.Shininess", 100.0f);

    model = getModelMatrix(cow);
    setMatrices();
    cube->render();
    prog.stop();
}

void SceneRenderToTex::renderToTexture() {
    prog.use();
    //bind texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, cow->getTexture()->getHandle());

    //set uniforms
    prog.setUniform("RenderTex", 1); //render cow-texture
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    projection = glm::perspective(glm::radians(50.0f), 1.0f, 0.3f, 100.0f);

    prog.setUniform("Light.Position", glm::vec4(0.0f,0.0f,0.0f,1.0f) );
    prog.setUniform("Material.Ks", 0.95f, 0.95f, 0.95f);
    prog.setUniform("Material.Shininess", 100.0f);

    model = getModelMatrix(cow);    
    setMatrices();

    //render
    cow->render();

    //bind texture
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, tex->getHandle());

    //set uniforms
    cube->setPosition(glm::vec3(-0.8f, 0.0f, -1.0f));
    model = getModelMatrix(cube);
    setMatrices();

    //render
    cube->render();

    prog.stop();
}


void SceneRenderToTex::renderScene(GLuint textureHandle) {
    prog.use();
    prog.setUniform("RenderTex", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureHandle);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    projection = glm::perspective(glm::radians(45.0f), (float)width/height, 0.3f, 100.0f);

    prog.setUniform("Light.Position", glm::vec4(0.0f,0.0f,0.0f,1.0f) );
    prog.setUniform("Material.Ks", 0.0f, 0.0f, 0.0f);
    prog.setUniform("Material.Shininess", 1.0f);

    cube->setPosition(uiData.cube_position);
    model = getModelMatrix(cube);
    setMatrices();
    cube->render();
    prog.stop();
}

glm::mat4 SceneRenderToTex::getModelMatrix(Entity* entity)
{
    model = mat4(1.0f);
    model = glm::translate(model, entity->getPosition());
    model = glm::rotate(model, entity->getRotation().x, vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, entity->getRotation().y, vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, entity->getRotation().z, vec3(0.0f, 0.0f, 1.0f));
    return model;
}


glm::mat4 SceneRenderToTex::getModelMatrix(Cube* cube)
{
    model = mat4(1.0f);
    model = glm::translate(model, cube->getPosition());
    model = glm::rotate(model, cube->getRotation().x, vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, cube->getRotation().y, vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, cube->getRotation().z, vec3(0.0f, 0.0f, 1.0f));
    return model;
}

void SceneRenderToTex::renderGUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();
    {
        ImGui::Begin("Scene");        

        if (ImGui::BeginMenu("Cow"))
        {
            if (ImGui::SliderFloat("RotX", &uiData.cow_angle.x, 0.0f, 360.0f))
            {
            }

            if (ImGui::SliderFloat("RotY", &uiData.cow_angle.y, 0.0f, 360.0f))
            {
            }

            if (ImGui::SliderFloat("RotZ", &uiData.cow_angle.z, 0.0f, 360.0f))
            {
            }

            if (ImGui::SliderFloat("Translate X", &uiData.cow_position.x, -5.0f, 5.0f))
            {
            }

            if (ImGui::SliderFloat("Translate Y", &uiData.cow_position.y, -5.0f, 5.0f))
            {
            }

            if (ImGui::SliderFloat("Translate Z", &uiData.cow_position.z, -5.0f, 5.0f))
            {
            }

            if (ImGui::Checkbox("Auto-Rotate", &uiData.cow_auto_rotate)) 
            {
            }

            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Cube"))
        {
            if (ImGui::SliderFloat("RotX", &uiData.cube_angle.x, 0.0f, 360.0f))
            {
            }

            if (ImGui::SliderFloat("RotY", &uiData.cube_angle.y, 0.0f, 360.0f))
            {
            }

            if (ImGui::SliderFloat("RotZ", &uiData.cube_angle.z, 0.0f, 360.0f))
            {
            }

            if (ImGui::SliderFloat("Translate X", &uiData.cube_position.x, -5.0f, 5.0f))
            {
            }

            if (ImGui::SliderFloat("Translate Y", &uiData.cube_position.y, -5.0f, 5.0f))
            {
            }

            if (ImGui::SliderFloat("Translate Z", &uiData.cube_position.z, -5.0f, 5.0f))
            {
            }

            if (ImGui::Checkbox("Auto-Rotate", &uiData.cube_auto_rotate))
            {
            }


            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Camera"))
        {
            if (ImGui::SliderFloat("Position X", &uiData.camera_position.x, -10.0f, 10.0f))
            {
            }

            if (ImGui::SliderFloat("Position Y", &uiData.camera_position.y, -10.0f, 10.0f))
            {
            }

            if (ImGui::SliderFloat("Position Z", &uiData.camera_position.z, -10.0f, 10.0f))
            {
            }


            if (ImGui::Checkbox("Auto-Rotate", &uiData.camera_auto_rotate))
            {
            }


            ImGui::EndMenu();
        }

        if (ImGui::BeginMenu("Post-Processing"))
        {
            if (ImGui::Checkbox("Apply Negative Filter", &uiData.apply_negative))
            {

            }

            if (ImGui::Checkbox("Apply Color Swizzle", &uiData.apply_colorswizzle))
            {


            }

            if (ImGui::Checkbox("Apply Gaussian Blur", &uiData.apply_gaussianblur))
            {
            }

            ImGui::EndMenu();
        }


        if (ImGui::Checkbox("Show Overlay", &uiData.show_overlay)) 
        {
        }

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void SceneRenderToTex::setMatrices()
{
    mat4 mv = view * model;
    prog.setUniform("ModelViewMatrix", mv);
    prog.setUniform("NormalMatrix",
                    glm::mat3( vec3(mv[0]), vec3(mv[1]), vec3(mv[2]) ));
    prog.setUniform("MVP", projection * mv);
}

void SceneRenderToTex::resize(int w, int h)
{
    glViewport(0,0,w,h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(60.0f), (float)w/h, 0.3f, 100.0f);
}

void SceneRenderToTex::compileAndLinkShader()
{
  try {
    prog.compileShader("shader/rendertotex.vs");
    prog.compileShader("shader/rendertotex.fs");
    prog.link();
    prog.use();
  } catch(GLSLProgramException & e) {
    cerr << e.what() << endl;
    exit( EXIT_FAILURE );
  }
}
