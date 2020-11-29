#include "texture.h"
// CS. Added.
#define STB_IMAGE_IMPLEMENTATION
#include "stb/stb_image.h"
#include "glutils.h"
#include <iostream>

Texture::Texture(const std::string& fName, int type, bool flip) 
{
    switch (type) 
    {
    case Tex2D:
        textureHandle = load2DTex(fName, flip);
        break;
    case TexCube: 
        stbi_set_flip_vertically_on_load(flip);
        textureHandle = loadCubeMap(fName);
        break;
    case TexCubeHDR:
        stbi_set_flip_vertically_on_load(flip);
        textureHandle = loadHdrCubeMap(fName); 
        break;
    default: 
        textureHandle = load2DTex(fName, flip);
        break;
    }
}

Texture::~Texture()
{
    glDeleteTextures(1, &textureHandle);
}



unsigned char* Texture::loadPixels(const std::string& fName, int& width, int& height, bool flip) {
    int bytesPerPix;
    stbi_set_flip_vertically_on_load(flip);
    unsigned char* data = stbi_load(fName.c_str(), &width, &height, &bytesPerPix, 4);
    return data;
}

GLuint Texture::load2DTex(const std::string& fName, bool flip) 
{
    int width, height;
    unsigned char* data = loadPixels(fName, width, height, flip);
    GLuint texID = 0;
    if (data != nullptr) {
        glGenTextures(1, &texID);
        glBindTexture(GL_TEXTURE_2D, texID);
        glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, width, height);
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, width, height, GL_RGBA, GL_UNSIGNED_BYTE, data);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

        stbi_image_free(data);
    }

    return texID;
}

GLuint Texture::loadCubeMap(const std::string& baseName, const std::string& extension) {
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

    const char* suffixes[] = { "posx", "negx", "posy", "negy", "posz", "negz" };
    GLint w, h;

    // Load the first one to get width/height
    std::string texName = baseName + "_" + suffixes[0] + extension;
    GLubyte* data = loadPixels(texName, w, h, false);

    // Allocate immutable storage for the whole cube map texture
    glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGBA8, w, h);
    glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
    stbi_image_free(data);

    // Load the other 5 cube-map faces
    for (int i = 1; i < 6; i++) {
        std::string texName = baseName + "_" + suffixes[i] + extension;
        data = loadPixels(texName, w, h, false);
        glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, w, h, GL_RGBA, GL_UNSIGNED_BYTE, data);
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texID;
}

GLuint Texture::loadHdrCubeMap(const std::string& baseName) {
    GLuint texID;
    glGenTextures(1, &texID);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texID);

    const char* suffixes[] = { "posx", "negx", "posy", "negy", "posz", "negz" };
    GLint w, h;

    // Load the first one to get width/height
    std::string texName = baseName + "_" + suffixes[0] + ".hdr";
    float* data = stbi_loadf(texName.c_str(), &w, &h, NULL, 3);

    // Allocate immutable storage for the whole cube map texture
    glTexStorage2D(GL_TEXTURE_CUBE_MAP, 1, GL_RGB32F, w, h);
    glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, 0, 0, w, h, GL_RGB, GL_FLOAT, data);
    stbi_image_free(data);

    // Load the other 5 cube-map faces
    for (int i = 1; i < 6; i++) {
        std::string texName = baseName + "_" + suffixes[i] + ".hdr";
        data = stbi_loadf(texName.c_str(), &w, &h, NULL, 3);
        glTexSubImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, 0, 0, w, h, GL_RGB, GL_FLOAT, data);
        stbi_image_free(data);
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return texID;
}


