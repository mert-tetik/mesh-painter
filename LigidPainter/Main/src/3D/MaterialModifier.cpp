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
#include <iostream>
#include <vector>

#include "GUI/Elements/Elements.hpp"
#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"

//0 = albedo
//1 = roughness
//2 = metallic 
//3 = normal map
//4 = height map
//5 = ambient Occlusion

MaterialModifier::MaterialModifier(){}

void textureModifierUpdateMat(Material &material, int textureResolution, int curModI);
void dustModifierUpdateMat(Material &material, int textureResolution, int curModI);

MaterialModifier::MaterialModifier(ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures,int modifierIndex){
    
    if(modifierIndex == TEXTURE_MATERIAL_MODIFIER){
        this->sections = createTextureModifier(colorPalette,buttonShader,appTextures);
        this->title = "Texture Modifier";    
        shader = Shader("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert","LigidPainter/Resources/Shaders/MaterialModifiers/TextureModifier.frag",nullptr,nullptr,nullptr);
        this->updateMaterialChannels = textureModifierUpdateMat;
    }
    else if(modifierIndex == DUST_MATERIAL_MODIFIER){
        this->sections = createDustModifier(colorPalette,buttonShader,appTextures);
        this->title = "Dust Modifier";    
        shader = Shader("LigidPainter/Resources/Shaders/aVert/2D_uniforms.vert","LigidPainter/Resources/Shaders/MaterialModifiers/DustModifier.frag",nullptr,nullptr,nullptr);
        this->updateMaterialChannels = dustModifierUpdateMat;
    }

    this->modifierIndex = modifierIndex;
}


std::vector<Section> MaterialModifier::createTextureModifier(ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures){
        
    return 
    {
        Section(
            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette,buttonShader,"Channels",Texture(),0.f,true)),
            {
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Albedo",              appTextures.greetingDialogImage,0.f,false)),
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Roughness",           appTextures.greetingDialogImage,0.f,false)),
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Metallic",            appTextures.greetingDialogImage,0.f,false)),
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Normal map",          appTextures.greetingDialogImage,0.f,false)),
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Height map",          appTextures.greetingDialogImage,0.f,false)),
                Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Ambient Occlusion",   appTextures.greetingDialogImage,0.f,false))
            }
        )
    };
}

std::vector<Section> MaterialModifier::createDustModifier(ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures){

    return 
    {
        Section(
            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette,buttonShader,"Noise",Texture(),0.f,true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Size",appTextures.greetingDialogImage,0.f,0.f,100.f,20.f), // /10
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Offset Intensity",appTextures.greetingDialogImage,0.f,0.f,500.f,500.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Rotation",appTextures.greetingDialogImage,0.f,0.f,360.f,360.f), // /1
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Brightness",appTextures.greetingDialogImage,0.f,0.f,100.f,100.f), // /100
            }
        ),
        
        Section(
            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette,buttonShader,"Blur",Texture(),0.f,true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Intensity",appTextures.greetingDialogImage,0.f,0.f,100.f,0.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Direction",appTextures.greetingDialogImage,0.f,0.f,360.f,0.f), // /1
            }
        ),
        
        Section(
            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette,buttonShader,"Scratches",Texture(),0.f,true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Wavyness",appTextures.greetingDialogImage,0.f,0.f,100.f,10.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Scale",appTextures.greetingDialogImage,0.f,0.f,100.f,30.f), // /10
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Base Frequency",appTextures.greetingDialogImage,0.f,0.f,100.f,50.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"FrequencyStep",appTextures.greetingDialogImage,0.f,0.f,100.f,25.f), // /100
            }
        ),

        Section(
            Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(1,2.f),colorPalette,buttonShader,"Droplets",Texture(),0.f,true)),
            {
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Count",appTextures.greetingDialogImage,0.f,0.f,100.f,10.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"OpacityJitter",appTextures.greetingDialogImage,0.f,0.f,100.f,100.f), // /100
                RangeBar(ELEMENT_STYLE_SOLID,glm::vec2(1,1.5f),colorPalette,buttonShader,"Size",appTextures.greetingDialogImage,0.f,0.f,100.f,50.f), // /10
            }
        ),
    };
}

void textureModifierUpdateMat(Material &material, int textureResolution, int curModI){
    //Set the orthographic projection to the texture resolution
    glm::mat4 projection = glm::ortho(0.f,(float)textureResolution,(float)textureResolution,0.f);
    
    //Transform values to take the texture in to the middle of the screen and cover it completely
    glm::vec2 fragScale = glm::vec2((float)textureResolution/2.f,(float)textureResolution/2.f);
    glm::vec3 fragPos = glm::vec3((float)textureResolution/2.f,(float)textureResolution/2.f,1.0f);

    //Disable the depth test (just in case)
    for (int channelI = 0; channelI < 6; channelI++){
    
        glDisable(GL_DEPTH_TEST);

        //That framebuffer will be used to get the results of the shader 
        unsigned int FBO; 
        glGenFramebuffers(1,&FBO);
        glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    
        //Get the channel's texture from material
        unsigned int textureBuffer; //Material's texture
        if(channelI == 0){
            textureBuffer = material.albedo.ID;
        }
        if(channelI == 1){
            textureBuffer = material.roughness.ID;
        }
        if(channelI == 2){
            textureBuffer = material.metallic.ID;
        }
        if(channelI == 3){
            textureBuffer = material.normalMap.ID;
        }
        if(channelI == 4){
            textureBuffer = material.heightMap.ID;
        }
        if(channelI == 5){
            textureBuffer = material.ambientOcclusion.ID;
        }
        
        //Bind the channel texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,textureBuffer);
        
        //Params for the channel texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
        
        //Refresh the channel texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureResolution, textureResolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glGenerateMipmap(GL_TEXTURE_2D);

        //Bind the channel texture to the capture framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBuffer, 0);
        
        //Make the background pink (cause why not)
        glClearColor(1,0,1,1);
        glClear(GL_COLOR_BUFFER_BIT);
        
        //Set the uniforms of the modifier's shader
        material.materialModifiers[curModI].shader.use(); //Use the shader of the modifier
        material.materialModifiers[curModI].shader.setMat4("projection",projection); //Set the projection
        material.materialModifiers[curModI].shader.setVec2("scale",fragScale); //Set the scale
        material.materialModifiers[curModI].shader.setVec3("pos",fragPos); //Set the position
        material.materialModifiers[curModI].shader.setInt("state",channelI); //Set the texture slot
        material.materialModifiers[curModI].shader.setInt("theTexture",0); //Set the texture slot


        //Bind the texture (bind the channel textures if rendering a texture modifier & bind the result of the previous modifier)
        glActiveTexture(GL_TEXTURE0);

        //Bind the texture from the materialModifier's button element texture
        glBindTexture(GL_TEXTURE_2D,material.materialModifiers[curModI].sections[0].elements[channelI].button.texture.ID);
        
        //Render the result to the framebuffer
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        //Just in case 🤫😁🤑 
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //Delete the framebuffer after completing the channel
        glDeleteFramebuffers(1,&FBO);
        glEnable(GL_DEPTH_TEST);


    }
}

glm::vec2 getDirectionVector(float rotation) {
  // Convert rotation from degrees to radians
  float radians = glm::radians(rotation);

  // Calculate the x and y components of the direction vector
  float x = glm::cos(radians);
  float y = glm::sin(radians);

  return glm::vec2(x, y);
}

void dustModifierUpdateMat(Material &material, int textureResolution, int curModI){

    Shader modifierShader = material.materialModifiers[curModI].shader;

    textureResolution = 512;

    //Set the orthographic projection to the texture resolution
    glm::mat4 projection = glm::ortho(0.f,(float)textureResolution,(float)textureResolution,0.f);

    //Transform values to take the texture in to the middle of the screen and cover it completely
    glm::vec2 fragScale = glm::vec2((float)textureResolution/2.f,(float)textureResolution/2.f);
    glm::vec3 fragPos = glm::vec3((float)textureResolution/2.f,(float)textureResolution/2.f,1.0f);

    //Disable the depth test (just in case)
    for (int channelI = 0; channelI < 6; channelI++){
    
        glDisable(GL_DEPTH_TEST);

        //That framebuffer will be used to get the results of the shader 
        unsigned int FBO; 
        glGenFramebuffers(1,&FBO);
        glBindFramebuffer(GL_FRAMEBUFFER,FBO);
    
        //Get the channel's texture from material
        unsigned int textureBuffer; //Material's texture
        if(channelI == 0){
            textureBuffer = material.albedo.ID;
        }
        if(channelI == 1){
            textureBuffer = material.roughness.ID;
        }
        if(channelI == 2){
            textureBuffer = material.metallic.ID;
        }
        if(channelI == 3){
            textureBuffer = material.normalMap.ID;
        }
        if(channelI == 4){
            textureBuffer = material.heightMap.ID;
        }
        if(channelI == 5){
            textureBuffer = material.ambientOcclusion.ID;
        }
        
        //Bind the channel texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,textureBuffer);
        
        //Params for the channel texture
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
        
        //Refresh the channel texture
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, textureResolution, textureResolution, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glGenerateMipmap(GL_TEXTURE_2D);

        //Bind the channel texture to the capture framebuffer
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureBuffer, 0);
        
        //Make the background pink (cause why not)
        glClearColor(1,0,1,1);
        glClear(GL_COLOR_BUFFER_BIT);
        
        //Set the uniforms of the modifier's shader
        modifierShader.use(); //Use the shader of the modifier
        modifierShader.setMat4("projection",projection); //Set the projection
        modifierShader.setVec2("scale",fragScale); //Set the scale
        modifierShader.setVec3("pos",fragPos); //Set the position
        // modifierShader.shader.setInt("theTexture",0); //Set the texture slot
        // modifierShader.shader.setInt("state",channelI); //Set the texture slot

        /* Noise */
        modifierShader.setFloat("size", material.materialModifiers[curModI].sections[0].elements[0].rangeBar.value / 10.f); 
        modifierShader.setFloat("offsetIntensity", material.materialModifiers[curModI].sections[0].elements[1].rangeBar.value / 10.f); 
        modifierShader.setFloat("rotation", material.materialModifiers[curModI].sections[0].elements[2].rangeBar.value); 
        modifierShader.setFloat("brightness", material.materialModifiers[curModI].sections[0].elements[3].rangeBar.value / 100.f); 
        /* Blur */
        modifierShader.setFloat("blurIntensity", material.materialModifiers[curModI].sections[1].elements[0].rangeBar.value / 100.f);
        modifierShader.setVec2("blurDirection", getDirectionVector(material.materialModifiers[curModI].sections[1].elements[1].rangeBar.value));
        /* Scratches */
        modifierShader.setFloat("scratchesWavyness", material.materialModifiers[curModI].sections[2].elements[0].rangeBar.value / 100.f); 
        modifierShader.setVec2("scratchesScale", glm::vec2(material.materialModifiers[curModI].sections[2].elements[1].rangeBar.value / 10.f));
        modifierShader.setVec2("scratchesBaseFrequency", glm::vec2(material.materialModifiers[curModI].sections[2].elements[2].rangeBar.value / 100.f));
        modifierShader.setVec2("scratchesFrequencyStep", glm::vec2(material.materialModifiers[curModI].sections[2].elements[3].rangeBar.value / 100.f));
        /* Droplets */
        modifierShader.setFloat("dropletsCount", material.materialModifiers[curModI].sections[2].elements[0].rangeBar.value / 100.f); 
        modifierShader.setFloat("dropletsOpacityJitter", material.materialModifiers[curModI].sections[2].elements[1].rangeBar.value / 100.f);
        modifierShader.setFloat("dropletsSize", material.materialModifiers[curModI].sections[2].elements[2].rangeBar.value / 10.f);
        

        //Bind the texture (bind the channel textures if rendering a texture modifier & bind the result of the previous modifier)
        glActiveTexture(GL_TEXTURE0);
        
        //Bind the texture from the materialModifier's button element texture
        //glBindTexture(GL_TEXTURE_2D,material.materialModifiers[curModI].sections[0].elements[channelI].button.texture.ID);
        
        //Render the result to the framebuffer
        glDrawArrays(GL_TRIANGLES, 0, 6);
        
        //Just in case 🤫😁🤑 
        glGenerateMipmap(GL_TEXTURE_2D);
        
        //Delete the framebuffer after completing the channel
        glDeleteFramebuffers(1,&FBO);
        glEnable(GL_DEPTH_TEST);
    }
}