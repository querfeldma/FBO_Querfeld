#include "postprocessingrenderer.h"

PostProcessingRenderer::PostProcessingRenderer(GLuint textureHandle, int width, int height)
	: screen_quad(textureHandle, glm::vec2(0.0f, 0.0f), glm::vec2(1.0f, 1.0f)),
	screenTexture(screen_quad.getHandle()),
	screenShader(new GLSLProgram()),
	negativeShader(new GLSLProgram()),
	colorSwizzleShader(new GLSLProgram()),
	gaussShader(new GLSLProgram())
{
	std::vector<GLfloat> points = { -1.0f, 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f };
	/*std::vector<GLfloat> points = { -1.0f, 1.0f, 
									-1.0f, -1.0f,
									 1.0f, -1.0f,
									-1.0f, 1.0f,
									 1.0f, -1.0f,
									1.0f, 1.0f};*/
	//float quadVertices[] = { // vertex attributes for a quad that fills the entire screen in Normalized Device Coordinates.
	//   // positions   // texCoords
	//   -1.0f,  1.0f,  0.0f, 1.0f,
	//   -1.0f, -1.0f,  0.0f, 0.0f,
	//	1.0f, -1.0f,  1.0f, 0.0f,

	//   -1.0f,  1.0f,  0.0f, 1.0f,
	//	1.0f, -1.0f,  1.0f, 0.0f,
	//	1.0f,  1.0f,  1.0f, 1.0f
	//};
	fbo = new Fbo(width, height);
	loadToVAO(points, 2);
	compileAndLinkShader(screenShader, "shader/basic.vert.glsl", "shader/basic.frag.glsl");
	compileAndLinkShader(negativeShader, "shader/post.negative.vert.glsl", "shader/post.negative.frag.glsl");
	compileAndLinkShader(colorSwizzleShader, "shader/post.color.vert.glsl", "shader/post.color.frag.glsl");
	compileAndLinkShader(gaussShader, "shader/post.gauss.vert.glsl", "shader/post.gauss.frag.glsl");
}

void PostProcessingRenderer::render(bool negative, bool color, bool gauss) 
{
	glBindVertexArray(vaoID);
	glEnableVertexAttribArray(0);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glDisable(GL_DEPTH_TEST);

	applyEffect(screenShader, screenTexture);

	//apply post processing effects
	if(negative)
		applyEffect(negativeShader, fbo->getTextureHandle());
	if(color)
		applyEffect(colorSwizzleShader, fbo->getTextureHandle());
	if(gauss)
		applyEffect(gaussShader, fbo->getTextureHandle());

	//render
	screenShader->use();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, fbo->getTextureHandle());
	glm::mat4 matrix = createTransformationMatrix(screen_quad.getPosition(), screen_quad.getScale());
	screenShader->setUniform("transformationMatrix", matrix);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	screenShader->stop();

	//unbind
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);
	glDisableVertexAttribArray(0);
	glBindVertexArray(0);
}

GLuint PostProcessingRenderer::applyEffect(GLSLProgram* shader, GLuint texture) 
{
	shader->use();
	fbo->bind();
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glm::mat4 matrix = createTransformationMatrix(screen_quad.getPosition(), screen_quad.getScale());
	shader->setUniform("transformationMatrix", matrix);
	glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);
	fbo->unbind();
	shader->stop();
	return fbo->getTextureHandle();
}

PostProcessingRenderer::~PostProcessingRenderer() 
{
	delete fbo;
	delete screenShader;
	delete negativeShader;
	delete colorSwizzleShader;
	delete gaussShader;
	glDeleteVertexArrays(1, &vaoID);
	glDeleteBuffers(1, &vboID);
}
void PostProcessingRenderer::compileAndLinkShader(GLSLProgram* shader, const char* vertPath, const char* fragPath)
{
	try {
		shader->compileShader(vertPath);
		shader->compileShader(fragPath);
		shader->link();
	}
	catch (GLSLProgramException& e) {
		std::cerr << e.what() << std::endl;
		exit(EXIT_FAILURE);
	}
}

glm::mat4 PostProcessingRenderer::createTransformationMatrix(glm::vec2 position, glm::vec2 scale)
{
	glm::mat4 matrix = glm::mat4(1.0f); //set identity
	matrix = glm::translate(matrix, glm::vec3(position.x, position.y, 1.0f));
	matrix = glm::scale(matrix, glm::vec3(scale.x, scale.y, 1.0f));
	return matrix;
}


void PostProcessingRenderer::loadToVAO(std::vector<GLfloat> points, int dimensions)
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

