#pragma once
#include <glad\glad.h>

class Fbo {
private:
    GLuint fboHandle;
    GLuint textureHandle;
    GLuint depthBuffer;
    GLuint width, height;

public:
    enum { COLOR = 0, DEPTH = 1 };

    Fbo(int width, int height, int attachment = COLOR);

    ~Fbo();

    void bind();

    void unbind(); 
   
    GLuint getFBOHandle() { return fboHandle; };

    GLuint getTextureHandle() { return textureHandle; };

    GLuint getWidth() { return width; };

    GLuint getHeight() { return height; };
};