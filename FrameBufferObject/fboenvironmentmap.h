#pragma once

#include "glad/glad.h"

enum { ENV = 0, IRR = 1};

class FboEnvironmentMap 
{
private:
    GLuint fboHandle;
    GLuint depthBuffer;
    GLuint envCubemap;
    GLuint irrCubemap;
    int width, height;

    void initEnvMap(GLuint* mapHandle, int width, int height);
    void bindEnvCubeMap(GLuint textureHandle);
    void bindIrradianceMap(GLuint textureHandle);


public:
    FboEnvironmentMap(int width, int height);

    ~FboEnvironmentMap();

    void bind(int map, GLuint textureHandle);

    GLuint getFBOHandle() { return fboHandle; };

    GLuint getDepthBuffer() { return depthBuffer; };

    GLuint getEnvCubemap() { return envCubemap; };

    GLuint getIrradianceMap() { return irrCubemap; };
};
