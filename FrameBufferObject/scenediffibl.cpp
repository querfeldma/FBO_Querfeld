#include "scenediffibl.h"

#include <iostream>
#include <glm/gtc/matrix_transform.hpp>
#include "stb/stb_image.h"

SceneDiffIbl::SceneDiffIbl()
    : tPrev(0.0f), camAngle(glm::half_pi<float>()), rotSpeed(0.5f),
    milkyway("../media/texture/equirectangular/milkyway/Milkyway_small.hdr"),
    road("../media/texture/equirectangular/road/Ridgecrest_Road_Ref.hdr"),
    walk_of_fame("../media/texture/equirectangular/walk_of_fame/Mans_Outside_2k.hdr"),
    fbo(512, 512),
    hdrDiffuseProg(new GLSLProgram()),
    hdrEquirectangularToCubeMapProg(new GLSLProgram())
{
    uiData.cow_position = glm::vec3(0.0f, 0.0f, 0.0f);
    uiData.cow_angle = glm::vec3(0.0f, 180.0f, 0.0f);
    uiData.cow_auto_rotate = false;
    uiData.camera_position = glm::vec3(0.0f, 0.0f, 4.0f);
    uiData.camera_auto_rotate = false;
    uiData.use_gamma = true;
    uiData.gamma = 2.2f;
    uiData.use_schlick = false;
    uiData.use_schlick_roughness = false;
    uiData.roughness = 0.1f;
    uiData.show_env_map = false;
    uiData.show_irr_map = false;
}

SceneDiffIbl::~SceneDiffIbl() 
{
    delete hdrEquirectangularToCubeMapProg;
    delete hdrDiffuseProg;
    delete cube;
	delete cow;
}

void SceneDiffIbl::initScene()
{
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LEQUAL);

	compileAndLinkShader();

	cow = new Entity("../media/spot/spot_triangulated.obj", "../media/spot/spot_texture.png", uiData.cow_position, uiData.cow_angle);
	model = glm::mat4(1.0f);
    cube = new Cube(1.0f, glm::vec3(), glm::vec3());    
    
    textureHandle = walk_of_fame.getHandle();

    //Render to Cubemaps
    glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
    glm::mat4 captureViews[] =
    {
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
       glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
    };

    //Render to Environment Map (mode == ENV)
    renderToMap(ENV, textureHandle, fbo.getEnvCubemap(), hdrEquirectangularToCubeMapProg, captureProjection, captureViews);

    //Render to Irradiance Map (mode == IRR)
    renderToMap(IRR, fbo.getEnvCubemap(), fbo.getIrradianceMap(), hdrDiffuseProg, captureProjection, captureViews);
}

void SceneDiffIbl::render()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	model = glm::mat4(1.0f);

	prog.use();
	prog.setUniform("CamPos", uiData.camera_position);
	prog.setUniform("Material.Color", glm::vec3(0.4f, 0.4f, 0.4f));
    prog.setUniform("gamma", uiData.gamma);
    prog.setUniform("use_gamma", uiData.use_gamma);
    prog.setUniform("use_schlick", uiData.use_schlick);
    prog.setUniform("use_schlick_r", uiData.use_schlick_roughness);
    prog.setUniform("roughness", uiData.roughness);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, fbo.getIrradianceMap());
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, cow->getTexture()->getHandle());

    model = getModelMatrix(cow);
	setMatrices(prog);
	cow->render();
    prog.stop();

    if (uiData.show_env_map || uiData.show_irr_map) 
    {
        displayCubeProg.use();
        glActiveTexture(GL_TEXTURE0);

        if (uiData.show_env_map) 
        {
            glBindTexture(GL_TEXTURE_CUBE_MAP, fbo.getEnvCubemap());
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(1.0f, 0.0f, 0.0f));
            displayCubeProg.setUniform("projection", projection);
            displayCubeProg.setUniform("view", view * model);
            cube->render();
        }

        if (uiData.show_irr_map) 
        {
            glBindTexture(GL_TEXTURE_CUBE_MAP, fbo.getIrradianceMap());
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(-1.0f, 0.0f, 0.0f));
            displayCubeProg.setUniform("projection", projection);
            displayCubeProg.setUniform("view", view * model);
            cube->render();
        }
       
        displayCubeProg.stop();
    }



    hdrSkyboxProg.use();
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, fbo.getEnvCubemap());
    //glBindTexture(GL_TEXTURE_CUBE_MAP, irradianceMap);
    hdrSkyboxProg.setUniform("projection", projection);
    hdrSkyboxProg.setUniform("view", view);
    cube->render();
    hdrSkyboxProg.stop();


    renderGUI();
}

void SceneDiffIbl::renderToMap(int mode, GLuint source, GLuint destination, GLSLProgram* shader, glm::mat4 captureProjection, glm::mat4 captureViews[])
{
    shader->use();
    shader->setUniform("projection", captureProjection);

    fbo.bind(mode, source);

    for (unsigned int i = 0; i < 6; ++i)
    {
        shader->setUniform("view", captureViews[i]);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
            GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, destination, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        cube->render();
    }

    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

glm::mat4 SceneDiffIbl::getModelMatrix(Entity* entity)
{
    model = glm::mat4(1.0f);
    model = glm::translate(model, entity->getPosition());
    model = glm::rotate(model, entity->getRotation().x, glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, entity->getRotation().y, glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, entity->getRotation().z, glm::vec3(0.0f, 0.0f, 1.0f));
    return model;
}

void SceneDiffIbl::renderGUI()
{
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplGlfw_NewFrame();

    ImGui::NewFrame();
    {
        ImGui::Begin("Scene");


        if (ImGui::Checkbox("Use Schlick", &uiData.use_schlick))
        {

        }

        if (ImGui::Checkbox("Use Schlick-Roughness", &uiData.use_schlick_roughness))
        {

        }

        if (ImGui::SliderFloat("Roughness", &uiData.roughness, 0.1f, 1.0f))
        {

        }

        if (ImGui::Checkbox("Use Gamma", &uiData.use_gamma) )
        {

        }

        if (ImGui::SliderFloat("Gamma", &uiData.gamma, 1.0f, 3.0f)) 
        {

        }

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

        if (ImGui::Checkbox("Display Env. Map", &uiData.show_env_map))
        {
        }


        if (ImGui::Checkbox("Display Irradiance Map", &uiData.show_irr_map)) 
        {
        }

        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void SceneDiffIbl::update(float t)
{
	float deltaT = t - tPrev;
	if (tPrev == 0.0f) deltaT = 0.0f;
	tPrev = t;
	if (uiData.camera_auto_rotate) {
		camAngle = glm::mod(camAngle + deltaT * rotSpeed, glm::two_pi<float>());
		uiData.camera_position.x = glm::cos(camAngle) * 4.0f;
		uiData.camera_position.y = 0.0f;
		uiData.camera_position.z = glm::sin(camAngle) * 4.0f;
	}

    view = glm::lookAt(
        uiData.camera_position,
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    if (uiData.cow_auto_rotate)
    {
        uiData.cow_angle.y += glm::degrees(rotSpeed * deltaT);
        if (uiData.cow_angle.y > 360.0f)
            uiData.cow_angle.y -= 360.0f;
    }



    cow->setPosition(uiData.cow_position);
    cow->setRotation(glm::radians(uiData.cow_angle));
}


void SceneDiffIbl::resize(int w, int h)
{
    glViewport(0, 0, w, h);
    width = w;
    height = h;
    projection = glm::perspective(glm::radians(50.0f), (float)w / h, 0.3f, 100.0f);
}


void SceneDiffIbl::setMatrices(GLSLProgram &p)
{
	glm::mat4 mv = view * model;
	p.setUniform("ModelMatrix", model);
	p.setUniform("ModelViewMatrix", mv);
	p.setUniform("NormalMatrix", glm::mat3(mv));
	p.setUniform("MVP", projection * mv);
}

void SceneDiffIbl::compileAndLinkShader()
{
	try {
		prog.compileShader("shader/diffuseibl.vert.glsl");
		prog.compileShader("shader/diffuseibl.frag.glsl");
		prog.link();

        hdrEquirectangularToCubeMapProg->compileShader("shader/hdr.vert.glsl");
        hdrEquirectangularToCubeMapProg->compileShader("shader/hdr.frag.glsl");
        hdrEquirectangularToCubeMapProg->link();

        displayCubeProg.compileShader("shader/displaycube.vert.glsl");
        displayCubeProg.compileShader("shader/displaycube.frag.glsl");
        displayCubeProg.link();

        hdrSkyboxProg.compileShader("shader/hdr.skybox.vert.glsl");
        hdrSkyboxProg.compileShader("shader/hdr.skybox.frag.glsl");
        hdrSkyboxProg.link();


        hdrDiffuseProg->compileShader("shader/hdr.diffuse.vert.glsl");
        hdrDiffuseProg->compileShader("shader/hdr.diffuse.frag.glsl");
        hdrDiffuseProg->link();
	}
	catch (GLSLProgramException & e) {
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}

