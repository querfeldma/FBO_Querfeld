#include "fboenvironmentmap.h"

FboEnvironmentMap::FboEnvironmentMap(int width, int height)
: width(width), height(height)
{
    glGenFramebuffers(1, &fboHandle);
    glGenRenderbuffers(1, &depthBuffer);

    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);

    initEnvMap(&envCubemap, width, height);
    initEnvMap(&irrCubemap, 32, 32);
}

FboEnvironmentMap::~FboEnvironmentMap()
{
    glDeleteFramebuffers(1, &fboHandle);
    glDeleteRenderbuffers(1, &depthBuffer);
    glDeleteTextures(1, &envCubemap);
    glDeleteTextures(1, &irrCubemap);
}

void FboEnvironmentMap::bind(int map, GLuint textureHandle)
{
    if (map == ENV)
    {
        bindEnvCubeMap(textureHandle);
    }
    else if (map == IRR)
    {
        bindIrradianceMap(textureHandle);
    }
}

void FboEnvironmentMap::initEnvMap(GLuint* mapHandle, int width, int height)
{
    glGenTextures(1, mapHandle);
    glBindTexture(GL_TEXTURE_CUBE_MAP, *mapHandle);
    for (unsigned int i = 0; i < 6; ++i)
    {
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB16F, width, height, 0,
            GL_RGB, GL_FLOAT, nullptr);
    }
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}

void FboEnvironmentMap::bindEnvCubeMap(GLuint textureHandle)
{
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textureHandle);
    glViewport(0, 0, width, height); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
};

void FboEnvironmentMap::bindIrradianceMap(GLuint textureHandle)
{
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_CUBE_MAP, textureHandle);
    glViewport(0, 0, 32, 32); // don't forget to configure the viewport to the capture dimensions.
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
    glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, 32, 32);
};