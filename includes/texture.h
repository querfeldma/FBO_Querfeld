#pragma once

#include "cookbookogl.h"
#include <string>

enum {Tex2D = 0, TexCube = 1, TexCubeHDR = 2};

class Texture {
private:
    GLuint textureHandle;

public:

    Texture(const std::string& fName, int type, bool flip = true);

    ~Texture();

    GLuint loadCubeMap(const std::string& baseName, const std::string& extention = ".png");
    GLuint loadHdrCubeMap(const std::string& baseName);
    GLuint load2DTex(const std::string& fName, bool flip);
    unsigned char* loadPixels(const std::string& fName, int& w, int& h, bool flip = true);

    GLuint getHandle()
    {
        return textureHandle;
    }
};