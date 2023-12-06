/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <iostream>
#include <vector>

#include "UTIL/Util.hpp"

Texture::Texture(){

}

Texture::Texture(unsigned int ID){
    this->ID = ID;
}

Texture::Texture(char* pixels, int w, int h){
    glActiveTexture(GL_TEXTURE0);
    
    glGenTextures(1,&ID);

    glBindTexture(GL_TEXTURE_2D, ID);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_BYTE, pixels);
	
    glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::Texture(char* pixels, int w, int h, unsigned int filterParam){
    glActiveTexture(GL_TEXTURE0);
    
    glGenTextures(1,&ID);

    glBindTexture(GL_TEXTURE_2D, ID);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterParam);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterParam);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_BYTE, pixels);
	
    glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::Texture(char* pixels, int w, int h, unsigned int filterParam, int proceduralID){

    this->proceduralProps.proceduralID = proceduralID;
    
    glActiveTexture(GL_TEXTURE0);
    
    glGenTextures(1,&ID);

    glBindTexture(GL_TEXTURE_2D, ID);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterParam);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterParam);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, w, h, 0, GL_RGBA, GL_BYTE, pixels);
	
    glGenerateMipmap(GL_TEXTURE_2D);
}

Texture::Texture(char* pixels, int w, int h, unsigned int filterParam, unsigned int format, unsigned int internalFormat){
    glActiveTexture(GL_TEXTURE0);
    
    glGenTextures(1,&ID);

    glBindTexture(GL_TEXTURE_2D, ID);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterParam);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterParam);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_MIRRORED_REPEAT);
    
    glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_BYTE, pixels);
	
    glGenerateMipmap(GL_TEXTURE_2D);
}


void Texture::update(char* pixels, int w, int h){
    this->update(pixels, w, h, GL_LINEAR, GL_RGBA, GL_RGBA8);
}

void Texture::update(char* pixels, int w, int h, unsigned int filterParam){
    this->update(pixels, w, h, filterParam, GL_RGBA, GL_RGBA8);
}

void Texture::update(char* pixels, int w, int h, unsigned int filterParam, unsigned int format){
    this->update(pixels, w, h, filterParam, format, format);
}

void Texture::update(char* pixels, int w, int h, unsigned int filterParam, unsigned int format, unsigned internalFormat){
    this->update(pixels, w, h, filterParam, format, internalFormat, GL_MIRRORED_REPEAT);
}

void Texture::update(char* pixels, int w, int h, unsigned int filterParam, unsigned int format, unsigned internalFormat, unsigned int wrap){
    glm::ivec2 res = this->getResolution();
    
    glActiveTexture(GL_TEXTURE0);
    
    glBindTexture(GL_TEXTURE_2D, ID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, filterParam);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, filterParam);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, wrap);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, wrap);

    if(res.x == w && res.y == h) //!LEAK check the internal format & format
        glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, w, h, format, GL_BYTE, pixels);
    else{
        glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, format, GL_BYTE, pixels);
    }
    
    glGenerateMipmap(GL_TEXTURE_2D);
}