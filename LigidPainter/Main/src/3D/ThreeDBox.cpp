/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

Box.hpp : Is used to render a single 2D square.
          Mostly used by GUI elements.
*/

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include <string>
#include <iostream>
#include <vector>

#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"
#include "SettingsSystem/Settings.hpp"
#include "ShaderSystem/Shader.hpp"
    
ThreeDBox::ThreeDBox(){}


static void subdivideFace(Vertex vert_TL, Vertex vert_TR, Vertex vert_BL, Vertex vert_BR, std::vector<Vertex>& vertices, std::vector<unsigned int>& indices){
    
    Vertex TL_TR = Vertex((vert_TL.Position + vert_TR.Position) / 2.f, (vert_TL.TexCoords + vert_TR.TexCoords) / 2.f, vert_TL.Normal, vert_TL.Tangent, vert_TL.Bitangent);
    Vertex BL_BR = Vertex((vert_BL.Position + vert_BR.Position) / 2.f, (vert_BL.TexCoords + vert_BR.TexCoords) / 2.f, vert_TL.Normal, vert_TL.Tangent, vert_TL.Bitangent);
    Vertex TL_BL = Vertex((vert_TL.Position + vert_BL.Position) / 2.f, (vert_TL.TexCoords + vert_BL.TexCoords) / 2.f, vert_TL.Normal, vert_TL.Tangent, vert_TL.Bitangent);
    Vertex TR_BR = Vertex((vert_TR.Position + vert_BR.Position) / 2.f, (vert_TR.TexCoords + vert_BR.TexCoords) / 2.f, vert_TL.Normal, vert_TL.Tangent, vert_TL.Bitangent);
    Vertex TL_BR = Vertex((vert_TL.Position + vert_BR.Position) / 2.f, (vert_TL.TexCoords + vert_BR.TexCoords) / 2.f, vert_TL.Normal, vert_TL.Tangent, vert_TL.Bitangent);

    vertices.clear();

    vertices.push_back(vert_TL); // Top left 0
    vertices.push_back(vert_TR); // Top right 1
    vertices.push_back(vert_BL); // Bottom left 2
    vertices.push_back(vert_BR); // Bottom right 3
    
    vertices.push_back(TL_TR); // Top center 4
    vertices.push_back(BL_BR); // Bottom center 5
    vertices.push_back(TL_BL); // Left center 6
    vertices.push_back(TR_BR); // Right center 7
    vertices.push_back(TL_BR); // Center 8

    indices = {
                    // Top left face
                    0, 4, 6,
                    4, 6, 8,

                    // Top right face
                    1, 4, 7,
                    4, 7, 8,

                    // Bottom left face
                    2, 5, 6,
                    5, 6, 8,
                    
                    // Bottom right face
                    3, 5, 7,
                    5, 7, 8
                };
                
} 

glm::vec3 calculateNormal(const glm::vec3& v0, const glm::vec3& v1, const glm::vec3& v2, const glm::vec3& v3) {
    // Calculate two vectors on the surface
    glm::vec3 vector1 = v1 - v0;
    glm::vec3 vector2 = v2 - v0;

    // Cross product to get the normal vector
    glm::vec3 normal = glm::cross(vector1, vector2);

    glm::vec3 center = (v0 + v1 + v2 + v3) / 4.f;

    // Normalize the normal vector
    normal = glm::normalize(normal);

    return normal;
}


void subdivideMesh(glm::vec3 pos_topLeft, glm::vec3 pos_topRight, glm::vec3 pos_bottomLeft, glm::vec3 pos_bottomRight, std::vector<Vertex>& meshData, std::vector<unsigned int>& meshIndices, glm::vec3 normal, int subdivisions) {
    
    Vertex topLeftVert = Vertex(pos_topLeft, glm::vec2(0.f, 1.f), normal, glm::vec3(0.f), glm::vec3(0.f));
    Vertex topRightVert = Vertex(pos_topRight, glm::vec2(1.f, 1.f), normal, glm::vec3(0.f), glm::vec3(0.f));
    Vertex bottomLeftVert = Vertex(pos_bottomLeft, glm::vec2(0.f, 0.f), normal, glm::vec3(0.f), glm::vec3(0.f));
    Vertex bottomRightVert = Vertex(pos_bottomRight, glm::vec2(1.f, 0.f), normal, glm::vec3(0.f), glm::vec3(0.f));

    meshData.clear();

    meshData.push_back(topLeftVert);
    meshData.push_back(topRightVert);
    meshData.push_back(bottomLeftVert);
    meshData.push_back(bottomRightVert);

    meshIndices =   {
                        0,1,2,

                        1,2,3
                    };
    
    for (size_t i = 0; i < subdivisions; i++)
    {
        std::vector<Vertex> subdVertices;
        std::vector<unsigned int> subIndices;

        for (size_t i = 0; i < meshIndices.size(); i+=6)
        {
            unsigned int indice1 = meshIndices[i];
            unsigned int indice2 = meshIndices[i + 1];
            unsigned int indice3 = meshIndices[i + 2];
            unsigned int indice4 = meshIndices[i + 5];

            Vertex topLeftVert = meshData[indice1];
            Vertex topRightVert = meshData[indice2];
            Vertex bottomLeftVert = meshData[indice3];
            Vertex bottomRightVert = meshData[indice4];

            std::vector<Vertex> verts;
            std::vector<unsigned int> indics;

            subdivideFace(topLeftVert, topRightVert, bottomLeftVert, bottomRightVert, verts, indics);

            for (size_t i = 0; i < indics.size(); i++)
            {
                subIndices.push_back(indics[i] + subdVertices.size());
            }
            
            for (size_t i = 0; i < verts.size(); i++)
            {
                subdVertices.push_back(verts[i]);
            }            
        }

        meshData = subdVertices;
        meshIndices = subIndices;
    }
}

Framebuffer projectToModelFBO;
Texture projectToModelTxtr;

void ThreeDBox::projectToModel(std::vector<Vertex>& vertices, glm::vec3 center){
    
    if(!vertices.size())
        return;

    glm::vec3 camPos = glm::vec3(center + vertices[0].Normal * 5.f);
    glm::mat4 view = glm::lookAt(camPos, glm::vec3(0.f), glm::vec3(0.f, 1.f, 0.f));

    const unsigned int resolution = 2048;

    if(!projectToModelFBO.ID){
        projectToModelTxtr = Texture(nullptr, resolution, resolution);
        projectToModelFBO = Framebuffer(projectToModelTxtr, GL_TEXTURE_2D, Renderbuffer(GL_DEPTH_COMPONENT16, GL_DEPTH_ATTACHMENT, glm::ivec2(resolution)), "ThreeDBox::projectToModel fbo");;
    }

    projectToModelFBO.bind();

    glClearColor(0,0,0,0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //glViewport(0, 0, getContext()->windowScale.x / (Settings::videoScale()->x / resolution), getContext()->windowScale.y / (Settings::videoScale()->y / resolution));
    glViewport(0, 0, resolution, resolution);

    ShaderSystem::renderModelData().use();
    ShaderSystem::renderModelData().setMat4("view", view);
    ShaderSystem::renderModelData().setMat4("projection", getScene()->projectionMatrix);
    ShaderSystem::renderModelData().setMat4("modelMatrix",getScene()->transformMatrix);
    ShaderSystem::renderModelData().setInt("state", 1);

    getModel()->Draw();
    
    float* pxs = new float[resolution * resolution * 4]; 
    
    glReadPixels(
                    0, 
                    0, 
                    resolution, 
                    resolution,
                    GL_RGBA,
                    GL_FLOAT,
                    pxs
                );

    for (size_t i = 0; i < vertices.size(); i++)
    {
        glm::vec3* pos = &vertices[i].Position;
        glm::vec4 projectedPos = getScene()->projectionMatrix * view * glm::vec4(*pos, 1.f);
        projectedPos /= projectedPos.z;
        projectedPos /= projectedPos.w;

        projectedPos.x = (projectedPos.x + 1.f) / 2.f;
        projectedPos.y = (projectedPos.y + 1.f) / 2.f;
        
        projectedPos.x *= resolution;
        projectedPos.y *= resolution;

        int index = (((int)projectedPos.y) * resolution + (int)projectedPos.x) * 4;

        if(index + 3 < resolution * resolution * 4 && index >= 0){
            if(pxs[index + 3] != 0.f){
                pos->x = pxs[index + 0]; 
                pos->y = pxs[index + 1]; 
                pos->z = pxs[index + 2]; 

                pos->x = pos->x * 2.f - 1.f;
                pos->y = pos->y * 2.f - 1.f;
                pos->z = pos->z * 2.f - 1.f;
            }
        }
    }

    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport();

    delete[] pxs;
}

void ThreeDBox::init(glm::vec3 pos_topLeft, glm::vec3 pos_topRight, glm::vec3 pos_bottomLeft, glm::vec3 pos_bottomRight, glm::vec3 normal){
    
    LigidGL::cleanGLErrors();

    subdivideMesh(pos_topLeft, pos_topRight, pos_bottomLeft, pos_bottomRight, this->boxVertices, this->boxIndices, normal, 4);
    projectToModel(this->boxVertices, (pos_topLeft + pos_topRight + pos_bottomLeft + pos_bottomRight) / 4.f);

    //Generate vertex objects
    glGenVertexArrays(1, &VAO);
    LigidGL::testGLError("ThreeDBox::init : Generating VAO");
    glGenBuffers(1, &VBO);
    LigidGL::testGLError("ThreeDBox::init : Generating VBO");
    glGenBuffers(1, &EBO);
    LigidGL::testGLError("ThreeDBox::init : Generating EBO");

    // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
    glBindVertexArray(VAO);
    LigidGL::testGLError("ThreeDBox::init : Binding VAO");
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    LigidGL::testGLError("ThreeDBox::init : Binding VBO");
    glBufferData(GL_ARRAY_BUFFER, boxVertices.size() * sizeof(Vertex), &boxVertices[0], GL_DYNAMIC_DRAW);
    LigidGL::testGLError("ThreeDBox::init : Allocating memory for the VBO");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    LigidGL::testGLError("ThreeDBox::init : Binding EBO");
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, this->boxIndices.size() * sizeof(unsigned int), &this->boxIndices[0], GL_DYNAMIC_DRAW);
    LigidGL::testGLError("ThreeDBox::init : GL_ELEMENT_ARRAY_BUFFER data");

    // vertex Positions
    glEnableVertexAttribArray(0);	
    LigidGL::testGLError("ThreeDBox::init : glEnableVertexAttribArray(0)");
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    LigidGL::testGLError("ThreeDBox::init : glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);");
    
    // vertex TextureMs coords
    glEnableVertexAttribArray(1);	
    LigidGL::testGLError("ThreeDBox::init : glEnableVertexAttribArray(1)");
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));
    LigidGL::testGLError("ThreeDBox::init : glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3 * sizeof(float)));");
    
    // vertex normals
    glEnableVertexAttribArray(2);	
    LigidGL::testGLError("ThreeDBox::init : glEnableVertexAttribArray(2)");
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5 * sizeof(float)));
    LigidGL::testGLError("ThreeDBox::init : glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5 * sizeof(float)));");
    
    // vertex tangent
    glEnableVertexAttribArray(3);
    LigidGL::testGLError("ThreeDBox::init : glEnableVertexAttribArray(3)");
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(8 * sizeof(float)));
    LigidGL::testGLError("ThreeDBox::init : glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(8 * sizeof(float)));");
    
    // vertex bitangent
    glEnableVertexAttribArray(4);
    LigidGL::testGLError("ThreeDBox::init : glEnableVertexAttribArray(4)");
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(11 * sizeof(float)));
    LigidGL::testGLError("ThreeDBox::init : glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(11 * sizeof(float)));");


    // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0); 
    LigidGL::testGLError("ThreeDBox::init : Binding default VBO");

    // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
    // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
    glBindVertexArray(0);
    LigidGL::testGLError("ThreeDBox::init : Binding default VAO");
}

void ThreeDBox::update(glm::vec3 pos_topLeft, glm::vec3 pos_topRight, glm::vec3 pos_bottomLeft, glm::vec3 pos_bottomRight, glm::vec3 normal){
    LigidGL::cleanGLErrors();

    subdivideMesh(pos_topLeft, pos_topRight, pos_bottomLeft, pos_bottomRight, this->boxVertices, this->boxIndices, normal, 4);
    projectToModel(this->boxVertices, (pos_topLeft + pos_topRight + pos_bottomLeft + pos_bottomRight) / 4.f);

    glBindVertexArray(VAO);
    LigidGL::testGLError("ThreeDBox::update : Binding VAO");
    
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    LigidGL::testGLError("ThreeDBox::update : Binding VBO");
    glBufferSubData(GL_ARRAY_BUFFER, 0, boxVertices.size() * sizeof(Vertex), &boxVertices[0]);
    LigidGL::testGLError("ThreeDBox::update : Changing data in the allocated memory for the VBO");

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    LigidGL::testGLError("ThreeDBox::update : Binding EBO");

    glBufferSubData(GL_ELEMENT_ARRAY_BUFFER, 0, this->boxIndices.size() * sizeof(unsigned int), &this->boxIndices[0]);
    LigidGL::testGLError("ThreeDBox::update : Changing data in the allocated memory for the EBO");
}


void ThreeDBox::draw(){
    
    LigidGL::cleanGLErrors();

    //Bind vertex objects
    glBindVertexArray(VAO);
    LigidGL::testGLError("ThreeDBox::init : Binding VAO");
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    LigidGL::testGLError("ThreeDBox::init : Binding VBO");

    //Draw the box
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(this->boxIndices.size()), GL_UNSIGNED_INT, 0);
    LigidGL::testGLError("ThreeDBox::draw : Drawing elements");
}