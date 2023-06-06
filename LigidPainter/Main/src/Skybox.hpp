/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef LGDSKYBOX_HPP
#define LGDSKYBOX_HPP


#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Shader.hpp"
#include "Texture.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>


using namespace std;

class Skybox{
public:
    unsigned int ID;
    unsigned int IDPrefiltered = 0;

    unsigned int VBO;
    unsigned int VAO;

	unsigned int displayingTexture = 0;

	glm::mat4 transformMatrix = glm::mat4(1);//Used to rotate the skybox
	float lod = 0.; //Used to blur the skybox
	glm::vec3 bgColor = glm::vec3(0.1);
	float opacity = 0.f;

    Skybox(){}
    
    void init(){
        //Couldn't use the constructor since the glad is not initialized before definition
        
        std::vector<float> skyboxVertices = { //Array size is 108
	        -1.0f,  1.0f, -1.0f,
	        -1.0f, -1.0f, -1.0f,
	         1.0f, -1.0f, -1.0f,
	         1.0f, -1.0f, -1.0f,
	         1.0f,  1.0f, -1.0f,
	        -1.0f,  1.0f, -1.0f,
	        -1.0f, -1.0f,  1.0f,
	        -1.0f, -1.0f, -1.0f,
	        -1.0f,  1.0f, -1.0f,
	        -1.0f,  1.0f, -1.0f,
	        -1.0f,  1.0f,  1.0f,
	        -1.0f, -1.0f,  1.0f,
	         1.0f, -1.0f, -1.0f,
	         1.0f, -1.0f,  1.0f,
	         1.0f,  1.0f,  1.0f,
	         1.0f,  1.0f,  1.0f,
	         1.0f,  1.0f, -1.0f,
	         1.0f, -1.0f, -1.0f,
	        -1.0f, -1.0f,  1.0f,
	        -1.0f,  1.0f,  1.0f,
	         1.0f,  1.0f,  1.0f,
	         1.0f,  1.0f,  1.0f,
	         1.0f, -1.0f,  1.0f,
	        -1.0f, -1.0f,  1.0f,
	        -1.0f,  1.0f, -1.0f,
	         1.0f,  1.0f, -1.0f,
	         1.0f,  1.0f,  1.0f,
	         1.0f,  1.0f,  1.0f,
	        -1.0f,  1.0f,  1.0f,
	        -1.0f,  1.0f, -1.0f,
	        -1.0f, -1.0f, -1.0f,
	        -1.0f, -1.0f,  1.0f,
	         1.0f, -1.0f, -1.0f,
	         1.0f, -1.0f, -1.0f,
	        -1.0f, -1.0f,  1.0f,
	         1.0f, -1.0f,  1.0f
	    };

        //Generate the cubemap texture
	    glGenTextures(1,&ID);

        //Generate vertex objects
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        
        // bind the Vertex Array Object first, then bind and set vertex buffer(s), and then configure vertex attributes(s).
        glBindVertexArray(VAO);
        
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, 108 * sizeof(float), &skyboxVertices[0], GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);

        // note that this is allowed, the call to glVertexAttribPointer registered VBO as the vertex attribute's bound vertex buffer object so afterwards we can safely unbind
        glBindBuffer(GL_ARRAY_BUFFER, 0); 

        // You can unbind the VAO afterwards so other VAO calls won't accidentally modify this VAO, but this rarely happens. Modifying other
        // VAOs requires a call to glBindVertexArray anyways so we generally don't unbind VAOs (nor VBOs) when it's not directly necessary.
        glBindVertexArray(0);

    }

    void load(std::string path) //Path to the folder
	{
		std::vector<std::string> faces //Each face of the skybox 
		{
		    path + "/px.png", 
		    path + "/nx.png",
		    path + "/py.png",
		    path + "/ny.png",
		    path + "/pz.png",
		    path + "/nz.png"
		};

	    int width, height, nrChannels;//Of the face of the skybox
	    for (unsigned int i = 0; i < faces.size(); i++)
	    {

            //Cube's faces are determined that way cause macOS having trouble indexing cubemap's faces.
            //That doesn't solve the problem tho
		    unsigned int cubePos;
			if(i == 0)
				cubePos = GL_TEXTURE_CUBE_MAP_POSITIVE_X;
			if(i == 1)
				cubePos = GL_TEXTURE_CUBE_MAP_NEGATIVE_X;
			if(i == 2)
				cubePos = GL_TEXTURE_CUBE_MAP_POSITIVE_Y;
			if(i == 3)
				cubePos = GL_TEXTURE_CUBE_MAP_NEGATIVE_Y;
			if(i == 4)
				cubePos = GL_TEXTURE_CUBE_MAP_POSITIVE_Z;
			if(i == 5)
				cubePos = GL_TEXTURE_CUBE_MAP_NEGATIVE_Z;

            Texture txtr;
            unsigned char *data = txtr.getTextureDataViaPath(faces[i].c_str(),width,height,nrChannels,3,true);

	        if (data) //Loaded
	        {
                glActiveTexture(GL_TEXTURE0);
	            glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

                glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	            glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
	            
                glTexImage2D(cubePos, 
	                         0, 
                             GL_RGB8, 
                             width, 
                             height,
                             0, 
                             GL_RGB, 
                             GL_UNSIGNED_BYTE, 
                             data
	            );
	            
                std::cout << "Loaded " << faces[i] << std::endl;
                stbi_image_free(data);
	        }
	        else //An error occured while loading
	        {
	            std::cout << "Cubemap texture failed to load at path: " << faces[i] << std::endl;
	            stbi_image_free(data);
	        }
	    }
	}

    void draw(bool bindTxtr){ //Draw the skybox
        //Bind the skybox texture to the texture slot 0
        if(bindTxtr){
			glActiveTexture(GL_TEXTURE0);
        	glBindTexture(GL_TEXTURE_CUBE_MAP,IDPrefiltered);
		}

        //Disable the depth testing
        glDisable(GL_DEPTH_TEST);
	    glDepthMask(GL_FALSE);
	    glDepthFunc(GL_LEQUAL);

        //Bind vertex objects
	    glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);

        //Draw the skybox
        glDrawArrays(GL_TRIANGLES, 0, 108);

        //Bind default buffers
        glBindBuffer(GL_ARRAY_BUFFER, 0); 
        glBindVertexArray(0);
	
        //Enable the depth testing
        glEnable(GL_DEPTH_TEST);
        glDepthFunc(GL_LESS);
	    glDepthMask(GL_TRUE);
    }
	unsigned int createPrefilterMap(Shader prefilteringShader,glm::vec2 videoScale){
		//Creates the prefildered cubemap for the skybox
		//Which is used for PBR

		//Is required for rendering the skybox
		glDepthMask(GL_FALSE);
		glDepthFunc(GL_LEQUAL);

		//Skybox will be rendered to that FBO
		unsigned int captureFBO, captureRBO;
		glGenFramebuffers(1, &captureFBO);
		glGenRenderbuffers(1, &captureRBO);
		glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

		
		glm::mat4 captureProjection = glm::perspective(glm::radians(90.0f), 1.0f, 0.1f, 10.0f);
		//For rendering every side of the skybox
		glm::mat4 captureViews[] = 
		{
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(-1.0f,  0.0f,  0.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  1.0f,  0.0f), glm::vec3(0.0f,  0.0f,  1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f, -1.0f,  0.0f), glm::vec3(0.0f,  0.0f, -1.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f,  1.0f), glm::vec3(0.0f, -1.0f,  0.0f)),
		   glm::lookAt(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3( 0.0f,  0.0f, -1.0f), glm::vec3(0.0f, -1.0f,  0.0f))
		};	


			//Create the cube map texture
			glActiveTexture(GL_TEXTURE28);
			if(!IDPrefiltered)
				glGenTextures(1, &IDPrefiltered);
			glBindTexture(GL_TEXTURE_CUBE_MAP, IDPrefiltered);
			for (unsigned int txtrI = 0; txtrI < 6; ++txtrI)
			{
				//Every side
    			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + txtrI, 0, GL_RGB16F, 128, 128, 0, GL_RGB, GL_FLOAT, nullptr);
			}

			//Texture Parameters
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR); 
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			//Generate mipmap after creating the txtr
			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);


			//Use the related program for rendering
			prefilteringShader.use();
			
			prefilteringShader.setInt("environmentMap",0);
			prefilteringShader.setMat4("projection",captureProjection);
			prefilteringShader.setMat4("transformMatrix",glm::mat4(1));

			//Bind the skybox to the Slot 0
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_CUBE_MAP, ID);

			//Bind the framebuffer
			glBindFramebuffer(GL_FRAMEBUFFER, captureFBO);

			//5 levels will be created
			unsigned int maxMipLevels = 5;
			for (unsigned int mip = 0; mip < maxMipLevels; ++mip)
			{
				//Every level

			    //resize framebuffer according to mip-level size.
			    unsigned int mipWidth  = 128 * std::pow(0.5, mip);
			    unsigned int mipHeight = 128 * std::pow(0.5, mip);
			    glViewport(0, 0, mipWidth, mipHeight);

				//Adjust the roughness value
			    float roughness = (float)mip / (float)(maxMipLevels - 1);
				prefilteringShader.setFloat("roughness",roughness);


				//Create the texture
			    for (unsigned int txtrI = 0; txtrI < 6; ++txtrI)
			    {
					//Every side
					prefilteringShader.setMat4("view",captureViews[txtrI]);
			        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, 
			                               GL_TEXTURE_CUBE_MAP_POSITIVE_X + txtrI, IDPrefiltered, mip);

			        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
					draw(false); //Draw the skybox
			    }
			}
		
		//Set everything to default
		glBindFramebuffer(GL_FRAMEBUFFER, 0); 
		
		//!Set viewport the default was there

		glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);  

		//Bind the result cube map to 16th slote
		glActiveTexture(GL_TEXTURE16);
		glBindTexture(GL_TEXTURE_CUBE_MAP, IDPrefiltered);

		//Delete the framebuffer
		glDeleteFramebuffers(1,&captureFBO);
		glDeleteRenderbuffers(1,&captureRBO);

		return IDPrefiltered;
	}

	void createDisplayingTxtr(Shader skyboxBall,Model &sphereModel,glm::vec2 windowScale){
		 //Update the material texture

		glActiveTexture(GL_TEXTURE0);
		if(displayingTexture == 0)
		glGenTextures(1,&displayingTexture);
        glBindTexture(GL_TEXTURE_2D,displayingTexture);
        
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER);
        
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 100, 100, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        glGenerateMipmap(GL_TEXTURE_2D);

        unsigned int FBO; //That framebuffer will be used to get the results of the shader (modifier)
        glGenFramebuffers(1,&FBO);
        glBindFramebuffer(GL_FRAMEBUFFER,FBO);
        
        glViewport(0,0,100,100);

        unsigned int RBO;
		glGenRenderbuffers(1,&RBO);
		glBindRenderbuffer(GL_RENDERBUFFER,RBO);
		glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT16, 100, 100);
		glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, RBO);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, displayingTexture, 0);

        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        skyboxBall.use();
        glm::mat4 view = glm::lookAt(glm::vec3(3.f,0,0), 
                                     glm::vec3(0), 
                                     glm::vec3(0.0, 1.0, 0.0));
        
        glm::vec3 viewPos = glm::vec3(3.f,0,0);

        glm::mat4 projectionMatrix = glm::perspective(glm::radians(90.f), 
                                         1.f, 
                                         100.f, 
                                         0.1f);


        skyboxBall.setVec3("viewPos",viewPos);
        skyboxBall.setMat4("view",view);
        skyboxBall.setMat4("projection",projectionMatrix);

		skyboxBall.setInt("useTransformUniforms",0);

        glm::mat4 modelMatrix = glm::mat4(1);
        skyboxBall.setMat4("modelMatrix",modelMatrix);
        
		skyboxBall.setInt("skybox",0);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_CUBE_MAP,ID);
        
        sphereModel.Draw();
        
        glGenerateMipmap(GL_TEXTURE_2D);
        

        //Finish
        //buttonShader.use();
        glBindFramebuffer(GL_FRAMEBUFFER,0);
        glDeleteFramebuffers(1,&FBO);

        glViewport(0,0,windowScale.x,windowScale.y);
	}
};

#endif