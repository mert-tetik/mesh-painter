/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

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
#include <filesystem>

#include "UTIL/Util.hpp"
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "MouseSystem/Mouse.hpp"

// Defined in Painter/faceSelection.cpp
void updatePrimitivesArrayTexture(Texture& primitivesArrayTexture, std::vector<byte> primitivesArray, std::vector<byte>& prevPrimArray, Mesh& selectedMesh, std::vector<int>& changedIndices, bool updateAll);



void Model::updateObjectIDsTexture(){
    glDepthFunc(GL_LESS);
    
    glm::ivec2 resolution = glm::ivec2(getContext()->windowScale);

    // Create the texture if it wasn't created
    if(!this->objectIDs.ID){
        glGenTextures(1, &this->objectIDs.ID);
    }

    // Remove if slow
    this->objectIDs.update(nullptr, resolution.x, resolution.y, GL_LINEAR, GL_RG, GL_RG32F);

    Framebuffer captureFBO = Framebuffer(this->objectIDs, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, glm::ivec2(resolution)), "updateObjectIDsTexture");
    captureFBO.bind();

    {
        //Clear the object ids texture
        glViewport(0, 0, resolution.x, resolution.y);
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        //Use the depth 3D shader
        ShaderSystem::modelObjectID().use();
        ShaderSystem::modelObjectID().setMat4("view", getScene()->viewMatrix);
        ShaderSystem::modelObjectID().setMat4("projection", getScene()->projectionMatrix);
        ShaderSystem::modelObjectID().setMat4("modelMatrix",getScene()->transformMatrix);

        for (size_t i = 0; i < this->meshes.size(); i++){
            
            ShaderSystem::modelObjectID().setInt("objIndicesArraySize", this->meshes[i].objects.size());
            ShaderSystem::modelObjectID().setInt("meshI", i + 1);
            
            for (size_t objI = 0; objI < this->meshes[i].objects.size(); objI++)
            {
                ShaderSystem::modelObjectID().setVec2("objIndices[" + std::to_string(objI) + "]", this->meshes[i].objects[objI].vertIndices / glm::ivec2(3));
            }
            
            this->meshes[i].Draw(false);
        }
    }
    
    //!Finished

    //Set back to default shader
    ShaderSystem::buttonShader().use();

    //Delete the capture framebuffer
    captureFBO.deleteBuffers(false, true);

    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();

    glDepthFunc(GL_LEQUAL);
}

static int originalOBJI = 0;

void Model::selectObject(Panel& objectsPanel){
    
    int objI = -1;
    int meshI = -1;

    if(shortcuts_CTRL_A()){
        for (size_t meshI = 0; meshI < this->meshes.size(); meshI++)
        {
            for (size_t i = 0; i < this->meshes[meshI].objects.size(); i++)
            {
                this->meshes[meshI].selectedObjectIndices.push_back(i);
            }
        }
    }
    else if(this->objectIDs.ID){

        Framebuffer FBO = Framebuffer(this->objectIDs, GL_TEXTURE_2D, "Model::selectObject");
        FBO.bind();

        glm::vec2 pos = *Mouse::cursorPos();
        pos.y = getContext()->windowScale.y - pos.y;

        float* pxs = new float[2]; 
        
        glReadPixels(
                        pos.x, 
                        pos.y, 
                        1, 
                        1,
                        GL_RG,
                        GL_FLOAT,
                        pxs
                    );

        //Set back to default shader
        ShaderSystem::buttonShader().use();

        //Delete the capture framebuffer
        FBO.deleteBuffers(false, true);

        Settings::defaultFramebuffer()->FBO.bind();
        Settings::defaultFramebuffer()->setViewport();

        objI = pxs[0];
        meshI = pxs[1] - 1;

        for (size_t secI = 0; secI < objectsPanel.sections.size(); secI++)
        {
            for (size_t elI = 0; elI < objectsPanel.sections[secI].elements.size(); elI++){
                if(objectsPanel.sections[secI].elements[elI].button.hover && !objectsPanel.sliderButton.hover){
                    meshI = secI;
                    objI = elI;
                    objectsPanel.sections[secI].elements[elI].button.hover = false;
                }
            }
        }
        
        delete[] pxs;



        if(meshI < this->meshes.size() && meshI >= 0 && *Mouse::LClick()){
            registerObjectSelectionAction("Object selected", meshI, this->meshes[meshI].selectedObjectIndices);
        }



        for (size_t i = 0; i < this->meshes.size(); i++){
            if(!this->meshes[i].selectedObjectPrimitivesTxtr.ID){
                glGenTextures(1, &this->meshes[i].selectedObjectPrimitivesTxtr.ID);
            }
            
            if(!getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) && !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL)){
                if(*Mouse::LClick()){
                    this->meshes[i].selectedObjectIndices.clear();
                    originalOBJI = objI;
                }
                this->meshes[i].selectedObjectPrimitivesTxtr.update(nullptr, 1, 1);
            }        
        }

        if(meshI < this->meshes.size() && meshI >= 0){
            int match = -1;
            for (size_t i = 0; i < this->meshes[meshI].selectedObjectIndices.size(); i++)
            {
                if(this->meshes[meshI].selectedObjectIndices[i] == objI)
                    match = i; 
            }
            
            if(*Mouse::LClick()){
                if(!objectsPanel.hover){
                    if(!getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL)){
                        if(match == -1)
                            this->meshes[meshI].selectedObjectIndices.push_back(objI);
                    }
                    else{
                        if(match != -1)
                            this->meshes[meshI].selectedObjectIndices.erase(this->meshes[meshI].selectedObjectIndices.begin() + match);
                    }
                }
                else{
                    if(!getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) && !getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT)){
                        if(match == -1)
                            this->meshes[meshI].selectedObjectIndices.push_back(objI);
                    }
                    else if(getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL)){
                        if(match != -1)
                            this->meshes[meshI].selectedObjectIndices.erase(this->meshes[meshI].selectedObjectIndices.begin() + match);
                        else
                            this->meshes[meshI].selectedObjectIndices.push_back(objI);
                    }
                    else if(getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT)){
                        for (size_t si = std::min(originalOBJI, objI); si < std::max(originalOBJI, objI) + 1; si++)
                        {
                            match = -1;
                            for (size_t i = 0; i < this->meshes[meshI].selectedObjectIndices.size(); i++)
                            {
                                if(this->meshes[meshI].selectedObjectIndices[i] == si)
                                    match = i; 
                            }
                            
                            if(match == -1 && si < this->meshes[meshI].objects.size())
                                this->meshes[meshI].selectedObjectIndices.push_back(si);
                        }
                    }
                }
            }

            if(!getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL))
                Mouse::setCursor(*Mouse::pointerCursor());
            else
                Mouse::setCursor(*Mouse::pointerXCursor());
        }
    }
    
    for (size_t mshI = 0; mshI < this->meshes.size(); mshI++)
    {
        std::vector<int> changedIndices;
        std::vector<byte> primitivesArray;
        primitivesArray.resize(this->meshes[mshI].indices.size() / 3);
        for (size_t oI = 0; oI < this->meshes[mshI].objects.size(); oI++)
        {
            bool contains = false;
                for (size_t i = 0; i < this->meshes[mshI].selectedObjectIndices.size(); i++)
                {
                    if(this->meshes[mshI].selectedObjectIndices[i] == oI)
                        contains = true;
                }
            
            if(contains || (oI == objI && mshI == meshI)){
                for (size_t i = this->meshes[mshI].objects[oI].vertIndices.x / 3; i < this->meshes[mshI].objects[oI].vertIndices.y / 3; i++)
                {
                    if(i < primitivesArray.size()){
                        changedIndices.push_back(i);
                        if(contains)
                            primitivesArray[i] = 2;
                        else
                            primitivesArray[i] = 1;
                    }
                }
            }
        }

        updatePrimitivesArrayTexture(this->meshes[mshI].selectedObjectPrimitivesTxtr, primitivesArray, primitivesArray, this->meshes[mshI], changedIndices, true);
    }
}