//
// Created by Weevil on 2024/3/24.
//

#include "image.hpp"
#include <glad/glad.h>
#include <stb_image_write.h>
#include <stb_image.h>
#include <iostream>

unsigned int nell::loadImage(char const* path) {
    unsigned int textureID;
    glGenTextures(1, &textureID);

    stbi_set_flip_vertically_on_load(true);

    int width, height, nrComponents;
    unsigned char* data = stbi_load(path, &width, &height, &nrComponents, 0);
    if (data) {
        GLenum format;
        GLenum iformat;
        if (nrComponents == 1) {
            format = GL_RED;
            iformat = GL_RED;
        }
        else if (nrComponents == 3) {
            format = GL_RGB;
            iformat = GL_RGB;
        }
        else {
            format = GL_RGBA;
            iformat = GL_RGBA;
        }

        glBindTexture(GL_TEXTURE_2D, textureID);
        glTexImage2D(GL_TEXTURE_2D, 0, iformat, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        stbi_image_free(data);
    }
    else {
        std::cout << "[Nell][Debug]Main::loadImage: Unable to laod "<< path << std::endl;
        stbi_image_free(data);
    }

    return textureID;
}
