#include "fbo.h"
#include <iostream>

Fbo::Fbo(int width, int height, int attachment) 
: width(width), height(height)
{
    std::cout << "FBO-CTOR" << std::endl;

    //generate and bind fbo
    glGenFramebuffers(1, &fboHandle);
    bind();

    //generate texture
    glGenTextures(1, &textureHandle);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureHandle);

    
    if (attachment == COLOR) //use texture for color-attachment
    {
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //link texture to color-attachment in currently bound fbo 
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureHandle, 0);

        //generate depthbuffer as rbo
        glGenRenderbuffers(1, &depthBuffer);
        glBindRenderbuffer(GL_RENDERBUFFER, depthBuffer);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, width, height);

        //link currently bound rbo to depth-attachment in currently bound fbo
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, depthBuffer);
    }
    else if (attachment == DEPTH) //use texture for depth-attachment
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, width, height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        //link texture to depth-attachment in currently bound fbo
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, textureHandle, 0);
    }
        


    GLenum result = glCheckFramebufferStatus(GL_FRAMEBUFFER);
    if (result == GL_FRAMEBUFFER_COMPLETE) {
        std::cout << "Framebuffer is complete" << std::endl;
    }
    else {
        std::cout << "Framebuffer error: " << result << std::endl;
    }

    //unbind the framebuffer, and revert to default-framebuffer/screen
    unbind();
}

void Fbo::bind() 
{
    glViewport(0, 0, width, height);
    glBindFramebuffer(GL_FRAMEBUFFER, fboHandle);
}

void Fbo::unbind() 
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0); 
}


Fbo::~Fbo() 
{
    std::cout << "FBO-DTOR" << std::endl;
    glDeleteTextures(1, &textureHandle);
    glDeleteFramebuffers(1, &fboHandle);
    
    if (depthBuffer)
        glDeleteRenderbuffers(1, &depthBuffer);
}