#include "overlayrenderer.h"

OverlayRenderer::OverlayRenderer() : shader(GLSLProgram())
{
	std::vector<GLfloat> points = { -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f };

	loadToVAO(points, 2);
	compileAndLinkShader();
}

OverlayRenderer::~OverlayRenderer()
{
	glDeleteVertexArrays(1, &vaoID);
	glDeleteBuffers(1, &vboID);
}

void OverlayRenderer::render(std::vector<Overlay*>* overlays)
{
	shader.use();
	glBindVertexArray(vaoID);
	glEnableVertexAttribArray(0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	//render
	for (int i = 0; i < overlays->size(); i++)
	{
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, overlays->at(i)->getHandle());
		glm::mat4 matrix = createTransformationMatrix(overlays->at(i)->getPosition(), overlays->at(i)->getScale());
		shader.setUniform("transformationMatrix", matrix);
		glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
	}

	//unbind
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
	shader.stop();
}

void OverlayRenderer::render(Overlay* overlay) 
{
	shader.use();

	shader.setUniform("overlayTexture", 0);

	glBindVertexArray(vaoID);
	glEnableVertexAttribArray(0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	//render
	glActiveTexture(GL_TEXTURE0); 
	glBindTexture(GL_TEXTURE_2D, overlay->getHandle());
	glm::mat4 matrix = createTransformationMatrix(overlay->getPosition(), overlay->getScale());
	shader.setUniform("transformationMatrix", matrix);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);

	//unbind
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
	shader.stop();
}

glm::mat4 OverlayRenderer::createTransformationMatrix(glm::vec2 position, glm::vec2 scale)
{
	glm::mat4 matrix = glm::mat4(1.0f); //set identity
	matrix = glm::translate(matrix, glm::vec3(position.x, position.y, 1.0f));
	matrix = glm::scale(matrix, glm::vec3(scale.x, scale.y, 1.0f));
	return matrix;
}

void OverlayRenderer::loadToVAO(std::vector<GLfloat> points, int dimensions)
{
	//create vao
	glGenVertexArrays(1, &vaoID);
	glBindVertexArray(vaoID);

	//store data in attributelist
	glGenBuffers(1, &vboID);
	glBindBuffer(GL_ARRAY_BUFFER, vboID);
	//FloatBuffer buffer = storeDataInFloatBuffer(data);
	glBufferData(GL_ARRAY_BUFFER, points.size() * sizeof(GLfloat), points.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0,
		dimensions, 					//vertex-lenght x,y,z
		GL_FLOAT,
		false,				//is_normalized
		0,					//stride
		0);					//offset

	//unbind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void OverlayRenderer::compileAndLinkShader()
{
	try {
		shader.compileShader("shader/basic.vert.glsl");
		shader.compileShader("shader/basic.frag.glsl");
		shader.link();
	}
	catch (GLSLProgramException& e) {
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}