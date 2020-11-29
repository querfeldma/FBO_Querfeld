#pragma once

#include "cookbookogl.h"
#include <string>
#include <iostream>
#include "stb/stb_image.h"

class EquirectangularHDR 
{
private:
    GLuint textureHandle;

public:
	EquirectangularHDR(std::string fName) 
	{
        stbi_set_flip_vertically_on_load(true);
        int width, height, nrComponents;
        float* data = stbi_loadf(fName.c_str(), &width, &height, &nrComponents, 0);
        if (data)
        {            
            glGenTextures(1, &textureHandle);
            glBindTexture(GL_TEXTURE_2D, textureHandle);
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB16F, width, height, 0, GL_RGB, GL_FLOAT, data);

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            stbi_image_free(data);
            std::cout << "Successfully loaded HDR image." << std::endl;
        }
        else
        {
            std::cout << "Failed to load HDR image." << std::endl;
        }

	}

    ~EquirectangularHDR() 
    {
        glDeleteTextures(1, &textureHandle);
    }

    GLuint getHandle()
    {
        return textureHandle;
    }
};

