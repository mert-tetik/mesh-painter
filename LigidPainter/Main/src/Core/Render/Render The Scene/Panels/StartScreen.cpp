#include<iostream>

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include <map>
#include <filesystem>

#include "Core/LigidPainter.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "Core/Model/model.h"

#include "Core/Render/Render.h"
#include "Core/UI/UserInterface.h"
#include "Core/Utilities.h"
#include "Core/Renderer.hpp"
#include "Core/gl.h"
#include "Core/Texture/Texture.h"
#include "Core/ProjectFile/WRLigidFile.hpp"
#include "Core/ProjectFile/WRLigidMaterialFile.hpp"
#include "Core/ProjectFile/WRLigidFolder.hpp"


#include "stb_image.h"
#include "stb_image_write.h"

#include "LibAL.h"

#include "tinyfiledialogs.h"


std::vector<std::string> tdModelPaths;

float gradPosMixVal = 0.f;

//TODO settings file - 3D Model
//TODO Add default 3D Models and do not copy a folder
//TODO Error : This 3d model is already imported

std::string createProjectErrorMsg = "";
int createProjectErrorMsgCounter = 0;

float startScreenLastMousePosY = 0;

bool startScreenSliderPressed = false;

void Render::startScreenPanel(std::vector<UIElement> &UIElements,Programs renderPrograms,Cubemaps cubemaps,SkyBoxShaderData skyBoxShaderData,
                                        float &createProjectPanelBlurVal,std::string &projectPath,double screenGapX,GLFWwindow* window,Icons icons,double mouseXpos,double mouseYpos,
                                        bool firstClick,bool &displayProjectFolderManager,std::vector<Font> &fonts,ProjectManager &projectManager,std::vector<aTexture> &albedoTextures
                                        ,int txtrRes,std::vector<NodeScene> &materials, std::vector<Node> &appNodes, ContextMenu &addNodeContexMenu, Model &model,bool firstClickR,Renderer &renderer
										,float &scrVal,bool &startScreen, float &startScreenLoadPanelScrollVal,int &selectedSkyBox,NodePanel &nodePanel,bool &starctScCreateProjectMode,bool &starctScLoadProjectMode){
		#if defined(_WIN32) || defined(_WIN64)
		    char folderDistinguisher = '\\';
		#else
			char folderDistinguisher = '/'; 
		#endif

		float yOffset = 0.f;
		yOffset = mouseYpos - startScreenLastMousePosY;

        glDisable(GL_DEPTH_TEST);

        Utilities util;
        UserInterface ui;
        ColorData colorData;
        ColorData2 colorData2;
        GlSet gls;

	    glm::mat4 view = glm::lookAt(glm::vec3(-5,0,0), glm::vec3(0), glm::vec3(0.0, 1.0, 0.0)); 
        skyBoxShaderData.view = view;
		gls.blendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		
        glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f);
		glUseProgram(renderPrograms.iconsProgram);
		gls.uniformMatrix4fv(renderPrograms.iconsProgram, "Projection", projection);
		gls.uniform1i(renderPrograms.iconsProgram, "icon", 6);
		glUseProgram(renderPrograms.uiProgram);
		gls.uniformMatrix4fv(renderPrograms.uiProgram, "TextProjection", projection);
		glUseProgram(renderPrograms.gradient);
		gls.uniformMatrix4fv(renderPrograms.gradient, "TextProjection", projection);
		glUseProgram(renderPrograms.directionalShadow);
		gls.uniformMatrix4fv(renderPrograms.directionalShadow, "TextProjection", projection);
		glUseProgram(renderPrograms.textureDisplayer);
		gls.uniform1i(renderPrograms.textureDisplayer, "currentTexture", 14);
		gls.uniform1i(renderPrograms.textureDisplayer, "roundCor",1);
		gls.uniformMatrix4fv(renderPrograms.textureDisplayer, "TextProjection", projection);
		glUseProgram(renderPrograms.renderTheTextureProgram);
		gls.uniformMatrix4fv(renderPrograms.renderTheTextureProgram, "TextProjection", projection);
		gls.uniform1i(renderPrograms.renderTheTextureProgram, "txtr", 14);
		gls.uniform1i(renderPrograms.renderTheTextureProgram, "isHover", 0);
		gls.uniform1i(renderPrograms.renderTheTextureProgram, "isPressed", 0);
		gls.uniform1i(renderPrograms.renderTheTextureProgram, "subSelected", 0);
		gls.uniform1i(renderPrograms.renderTheTextureProgram, "isMask", 0);
		gls.uniform1i(renderPrograms.renderTheTextureProgram, "maskUseColor", 0);
		gls.uniform1i(renderPrograms.renderTheTextureProgram, "roundCorners", 0);
		gls.uniform1i(renderPrograms.renderTheTextureProgram, "renderMaterials", 0);
		glUseProgram(renderPrograms.renderTheTextureBlur);
		gls.uniformMatrix4fv(renderPrograms.renderTheTextureBlur, "TextProjection", projection);
		glUseProgram(renderPrograms.uiProgram);

        glfwSetWindowAttrib(window,GLFW_DECORATED,GLFW_TRUE);
	    glfwSetWindowSize(window,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);
		glfwSetWindowSizeLimits(window,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);

        glViewport(0,
                   0,
                    glfwGetVideoMode(glfwGetPrimaryMonitor())->height,
                    glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);
        
        glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);

		
        
        //Back color
        ui.box(10,10,0,0,"",glm::vec4(0.98f,0.98f,0.98f,1),0,0,0,0.9f,10000,glm::vec4(1),0.f);
        //Left panel
        ui.box(0.2f,1.f,-1.f + 0.2f ,0,"",glm::vec4(1),0,0,0,0.91f,10000,glm::vec4(1),0.f);

        glDisable(GL_DEPTH_TEST);

		glUseProgram(renderPrograms.gradient);
		gradPosMixVal = util.transitionEffect(starctScCreateProjectMode,gradPosMixVal,0.1f);

		gls.uniform1f(renderPrograms.gradient, "gradPosX", 3.f-gradPosMixVal*3.f);
		ui.renderTheProgram(-1.0 + 0.4f + 0.9f,0.0f,0.9f,1.f);
		
		glUseProgram(renderPrograms.directionalShadow);
		ui.renderTheProgram(-1.0 + 0.4f + 0.9f,0.0f,0.9f,1.f);

		glUseProgram(renderPrograms.renderTheTextureProgram);
		glActiveTexture(GL_TEXTURE14);
		gls.uniform1f(renderPrograms.renderTheTextureProgram,"opacity",0.1f);
		
		if(starctScCreateProjectMode)
			gls.bindTexture(icons.rendered1);
		if(starctScLoadProjectMode)
			gls.bindTexture(icons.rendered2);
		ui.renderTheProgram(0.2f,-0.2f,0.8f / 1.428 /1.5,0.8f);
		gls.uniform1f(renderPrograms.renderTheTextureProgram,"opacity",1.f);
		
        
		glEnable(GL_DEPTH_TEST);
		glDepthMask(GL_TRUE);

		
		
		glUseProgram(renderPrograms.uiProgram);
        renderer.startScreenNewProjectButton.draw(glm::vec3(-1.f + 0.2f,0.7f,0.92f),glm::vec2(mouseXpos,mouseYpos));
		if(renderer.startScreenNewProjectButton.buttonEnter)
				nodePanel.pointerCursor = true;
		if(renderer.startScreenNewProjectButton.buttonEnter && firstClick){
			starctScCreateProjectMode = true;
			starctScLoadProjectMode = false;
		}
        renderer.startScreenLoadProjectButton.draw(glm::vec3(-1.f + 0.2f,0.58f,0.92f),glm::vec2(mouseXpos,mouseYpos));
		if(renderer.startScreenLoadProjectButton.buttonEnter)
				nodePanel.pointerCursor = true;
		if(renderer.startScreenLoadProjectButton.buttonEnter && firstClick){
			starctScLoadProjectMode = true;
			starctScCreateProjectMode = false;
		}
			


		if(starctScCreateProjectMode){

			//Slider
			float sliderHeight = 2.f / (3.5f + std::ceil(tdModelPaths.size()/4.f)*0.31f * 10.f);
			float sliderPos = 1.f-sliderHeight + scrVal;
			if(startScreenSliderPressed)
				scrVal -= yOffset/205.f;
			if(scrVal > 0.f)
				scrVal = 0.f;
			
			bool sliderHover = ui.isMouseOnButton(window,0.01f,sliderHeight, 0.98,sliderPos+0.16f,mouseXpos,mouseYpos,false,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);
			if(sliderPos - sliderHeight < -1.f){
				sliderPos = -1.f + sliderHeight;
				scrVal = +sliderPos - 1.f + sliderHeight;
			}
			
			if(sliderHover && firstClick)
				startScreenSliderPressed = true;
			if(glfwGetMouseButton(window,0) == GLFW_RELEASE)
				startScreenSliderPressed = false;
			

			ui.box(0.01f,sliderHeight, 0.98,sliderPos+0.16f,"",glm::vec4(colorData.LigidPainterThemeColor,0.5),0,false,false,0.98f,10000,glm::vec4(colorData.LigidPainterThemeColor,1.0),sliderHover);

			ui.renderText(renderPrograms.uiProgram,"Create new 3D project",-0.55f,0.8f-scrVal,0.0006f,glm::vec4(0.06,0.12,0.15,1.0),0.91f,false);
			
			
			
			//2 Project Path
			glUseProgram(renderPrograms.iconsProgram);
			ui.iconBox(0.05f/1.5f,0.05f,-0.54f,0.415f-scrVal,0.9f,icons.Circle,0.f,glm::vec4(0.8,0.8,0.8,1.0),glm::vec4(0.06,0.12,0.15,1.0));
			glUseProgram(renderPrograms.uiProgram);
			
			ui.renderText(renderPrograms.uiProgram,"2",-0.55f,0.4f-scrVal,0.0004f,glm::vec4(0.95,0.95,0.95,1.0),0.91f,false);
			ui.renderText(renderPrograms.uiProgram,"Project Path",-0.47f,0.4f-scrVal,0.0004f,glm::vec4(0.06,0.12,0.15,1.0),0.91f,false);

			renderer.startScreenProjectPathTextBox.draw(glm::vec3(-0.47f + renderer.startScreenProjectTitleTextBox.width,0.315f-scrVal,0.9f),glm::vec2(mouseXpos,mouseYpos),firstClick);
			if(renderer.startScreenProjectPathTextBox.buttonEnter)
				nodePanel.pointerCursor = true;
			projectPath = renderer.startScreenProjectPathTextBox.text;
			
			//3 Texture Resolution
			glUseProgram(renderPrograms.iconsProgram);
			ui.iconBox(0.05f/1.5f,0.05f,-0.54f,0.115f-scrVal,0.9f,icons.Circle,0.f,glm::vec4(0.8,0.8,0.8,1.0),glm::vec4(0.06,0.12,0.15,1.0));
			glUseProgram(renderPrograms.uiProgram);
			
			ui.renderText(renderPrograms.uiProgram,"3",-0.55f,0.1f-scrVal,0.0004f,glm::vec4(0.95,0.95,0.95,1.0),0.91f,false);
			ui.renderText(renderPrograms.uiProgram,"Texture Resolution",-0.47f,0.1f-scrVal,0.0004f,glm::vec4(0.06,0.12,0.15,1.0),0.91f,false);

			renderer.startScreenProjectResolutionTextBox.draw(glm::vec3(-0.47f + renderer.startScreenProjectTitleTextBox.width,0.015f-scrVal,0.96f),glm::vec2(mouseXpos,mouseYpos),firstClick,renderPrograms,icons);
			if(renderer.startScreenProjectResolutionTextBox.buttonEnter)
				nodePanel.pointerCursor = true;
			
			//1 Project Title
			glUseProgram(renderPrograms.iconsProgram);
			ui.iconBox(0.05f/1.5f,0.05f,-0.54f,0.715f-scrVal,0.9f,icons.Circle,0.f,glm::vec4(0.8,0.8,0.8,1.0),glm::vec4(0.06,0.12,0.15,1.0));
			glUseProgram(renderPrograms.uiProgram);
			
			ui.renderText(renderPrograms.uiProgram,"1",-0.55f,0.7f-scrVal,0.0004f,glm::vec4(0.95,0.95,0.95,1.0),0.91f,false);
			ui.renderText(renderPrograms.uiProgram,"Project Title",-0.47f,0.7f-scrVal,0.0004f,glm::vec4(0.06,0.12,0.15,1.0),0.91f,false);
			
			glDepthFunc(GL_LESS);
			renderer.startScreenProjectTitleTextBox.draw(glm::vec3(-0.47f + renderer.startScreenProjectTitleTextBox.width,0.615f-scrVal,0.91f),glm::vec2(mouseXpos,mouseYpos),firstClick);
			if(renderer.startScreenProjectTitleTextBox.buttonEnter)
				nodePanel.pointerCursor = true;
			glDepthFunc(GL_LEQUAL);
            
			
			
			//4 Select a skybox
			glUseProgram(renderPrograms.iconsProgram);
			ui.iconBox(0.05f/1.5f,0.05f,-0.54f,-0.185f-scrVal,0.9f,icons.Circle,0.f,glm::vec4(0.8,0.8,0.8,1.0),glm::vec4(0.06,0.12,0.15,1.0));
			glUseProgram(renderPrograms.uiProgram);
			
			ui.renderText(renderPrograms.uiProgram,"4",-0.55f,-0.2f-scrVal,0.0004f,glm::vec4(0.95,0.95,0.95,1.0),0.91f,false);
			ui.renderText(renderPrograms.uiProgram,"Select a skybox",-0.47f,-0.2f-scrVal,0.0004f,glm::vec4(0.06,0.12,0.15,1.0),0.91f,false);

			glUseProgram(renderPrograms.renderTheTextureProgram);
			glActiveTexture(GL_TEXTURE14);
			gls.bindTexture(icons.sky1);
			ui.renderTheProgram(-0.35f,-0.4f-scrVal,0.1f*1.4705/1.5,0.1f);
			
			gls.bindTexture(icons.sky2);
			ui.renderTheProgram(-0.1f,-0.4f-scrVal,0.1f*1.4705/1.5,0.1f);
			
			gls.bindTexture(icons.sky3);
			ui.renderTheProgram(0.15f,-0.4f-scrVal,0.1f*1.4705/1.5,0.1f);
			
			gls.bindTexture(icons.sky4);
			ui.renderTheProgram(0.4f,-0.4f-scrVal,0.1f*1.4705/1.5,0.1f);
			
			gls.bindTexture(icons.sky5);
			ui.renderTheProgram(-0.35f,-0.6f-scrVal,0.1f*1.4705/1.5,0.1f);
			
			gls.bindTexture(icons.sky6);
			ui.renderTheProgram(-0.1f,-0.6f-scrVal,0.1f*1.4705/1.5,0.1f);

			glUseProgram(renderPrograms.uiProgram);
        	
			bool sky0Enter = ui.isMouseOnButton(window,0.11f*1.4705/1.5,0.08f,-0.35f,-0.4f-scrVal,mouseXpos,mouseYpos,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);
			if(sky0Enter && firstClick)
				selectedSkyBox = 0;
			glm::vec4 sky0BtnColor = glm::vec4(0.06,0.12,0.15,1.0);
			glm::vec4 sky0BtnColorActive = glm::vec4(colorData.LigidPainterThemeColor,1.0);
			if(sky0Enter){
				sky0BtnColor = glm::vec4(0.06,0.12,0.15,0.5);
				sky0BtnColorActive = glm::vec4(colorData.LigidPainterThemeColor,0.5);
				nodePanel.pointerCursor = true;
			}
			ui.box(0.11f*1.4705/1.5,0.08f,-0.35f,-0.4f-scrVal,"",sky0BtnColor,0,false,false,0.92f,40,sky0BtnColorActive,selectedSkyBox == 0,{},{},0,true);
        	
			bool sky1Enter = ui.isMouseOnButton(window,0.11f*1.4705/1.5,0.08f,-0.1f, -0.4f-scrVal,mouseXpos,mouseYpos,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);
			if(sky1Enter && firstClick)
				selectedSkyBox = 1;
			glm::vec4 sky1BtnColor = glm::vec4(0.06,0.12,0.15,1.0);
			glm::vec4 sky1BtnColorActive = glm::vec4(colorData.LigidPainterThemeColor,1.0);
			if(sky1Enter){
				sky1BtnColor = glm::vec4(0.06,0.12,0.15,0.5);
				sky1BtnColorActive = glm::vec4(colorData.LigidPainterThemeColor,0.5);
				nodePanel.pointerCursor = true;
			}
			ui.box(0.11f*1.4705/1.5,0.08f,-0.1f, -0.4f-scrVal,"",sky1BtnColor,0,false,false,0.92f,40,sky1BtnColorActive,selectedSkyBox == 1,{},{},0,true);
        	
			bool sky2Enter = ui.isMouseOnButton(window,0.11f*1.4705/1.5,0.08f, 0.15f,-0.4f-scrVal,mouseXpos,mouseYpos,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);
			if(sky2Enter && firstClick)
				selectedSkyBox = 2;
			glm::vec4 sky2BtnColor = glm::vec4(0.06,0.12,0.15,1.0);
			glm::vec4 sky2BtnColorActive = glm::vec4(colorData.LigidPainterThemeColor,1.0);
			if(sky2Enter){
				sky2BtnColor = glm::vec4(0.06,0.12,0.15,0.5);
				sky2BtnColorActive = glm::vec4(colorData.LigidPainterThemeColor,0.5);
				nodePanel.pointerCursor = true;
			}
			ui.box(0.11f*1.4705/1.5,0.08f, 0.15f,-0.4f-scrVal,"",sky2BtnColor,0,false,false,0.92f,40,sky2BtnColorActive,selectedSkyBox == 2,{},{},0,true);
        	
			bool sky3Enter = ui.isMouseOnButton(window,0.11f*1.4705/1.5,0.08f, 0.4f, -0.4f-scrVal,mouseXpos,mouseYpos,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);
			if(sky3Enter && firstClick)
				selectedSkyBox = 3;
			glm::vec4 sky3BtnColor = glm::vec4(0.06,0.12,0.15,1.0);
			glm::vec4 sky3BtnColorActive = glm::vec4(colorData.LigidPainterThemeColor,1.0);
			if(sky3Enter){
				sky3BtnColor = glm::vec4(0.06,0.12,0.15,0.5);
				sky3BtnColorActive = glm::vec4(colorData.LigidPainterThemeColor,0.5);
				nodePanel.pointerCursor = true;
			}
			ui.box(0.11f*1.4705/1.5,0.08f, 0.4f, -0.4f-scrVal,"",sky3BtnColor,0,false,false,0.92f,40,sky3BtnColorActive,selectedSkyBox == 3,{},{},0,true);
        	
			bool sky4Enter = ui.isMouseOnButton(window,0.11f*1.4705/1.5,0.08f,-0.35f,-0.6f-scrVal,mouseXpos,mouseYpos,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);
			if(sky4Enter && firstClick)
				selectedSkyBox = 4;
			glm::vec4 sky4BtnColor = glm::vec4(0.06,0.12,0.15,1.0);
			glm::vec4 sky4BtnColorActive = glm::vec4(colorData.LigidPainterThemeColor,1.0);
			if(sky4Enter){
				sky4BtnColor = glm::vec4(0.06,0.12,0.15,0.5);
				sky4BtnColorActive = glm::vec4(colorData.LigidPainterThemeColor,0.5);
				nodePanel.pointerCursor = true;
			}
			ui.box(0.11f*1.4705/1.5,0.08f,-0.35f,-0.6f-scrVal,"",sky4BtnColor,0,false,false,0.92f,40,sky4BtnColorActive,selectedSkyBox == 4,{},{},0,true);
        	
			bool sky5Enter = ui.isMouseOnButton(window,0.11f*1.4705/1.5,0.08f,-0.1f, -0.6f-scrVal,mouseXpos,mouseYpos,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);
			if(sky5Enter && firstClick)
				selectedSkyBox = 5;
			glm::vec4 sky5BtnColor = glm::vec4(0.06,0.12,0.15,1.0);
			glm::vec4 sky5BtnColorActive = glm::vec4(colorData.LigidPainterThemeColor,1.0);
			if(sky5Enter){
				sky5BtnColor = glm::vec4(0.06,0.12,0.15,0.5);
				sky5BtnColorActive = glm::vec4(colorData.LigidPainterThemeColor,0.5);
				nodePanel.pointerCursor = true;
			}
			ui.box(0.11f*1.4705/1.5,0.08f,-0.1f, -0.6f-scrVal,"",sky5BtnColor,0,false,false,0.92f,40,sky5BtnColorActive,selectedSkyBox == 5,{},{},0,true);

			
			//5 Project Settings			
			glUseProgram(renderPrograms.iconsProgram);
			ui.iconBox(0.05f/1.5f,0.05f,-0.54f,-0.885f-scrVal,0.9f,icons.Circle,0.f,glm::vec4(0.8,0.8,0.8,1.0),glm::vec4(0.06,0.12,0.15,1.0));
			glUseProgram(renderPrograms.uiProgram);

			ui.renderText(renderPrograms.uiProgram,"5",-0.55f,-0.9f-scrVal,0.0004f,glm::vec4(0.95,0.95,0.95,1.0),0.91f,false);
			ui.renderText(renderPrograms.uiProgram,"Project settings",-0.47f,-0.9f-scrVal,0.0004f,glm::vec4(0.06,0.12,0.15,1.0),0.91f,false);
			
			renderer.startScreenIncludeTexturesCheckBox.draw(glm::vec3(-0.45,-1.0f-scrVal,0.95f),glm::vec2(mouseXpos,mouseYpos),firstClick);
			if(renderer.startScreenIncludeTexturesCheckBox.buttonEnter)
				nodePanel.pointerCursor = true;
			renderer.startScreenIncludeNodesCheckBox.draw(glm::vec3(-0.15,-1.0f-scrVal,0.95f),glm::vec2(mouseXpos,mouseYpos),firstClick);
			if(renderer.startScreenIncludeNodesCheckBox.buttonEnter)
				nodePanel.pointerCursor = true;
			renderer.startScreenIncludeFontsCheckBox.draw(glm::vec3(0.15,-1.0f-scrVal,0.95f),glm::vec2(mouseXpos,mouseYpos),firstClick);
			if(renderer.startScreenIncludeFontsCheckBox.buttonEnter)
				nodePanel.pointerCursor = true;

			//6 Upload 3D Models
			glUseProgram(renderPrograms.iconsProgram);
			ui.iconBox(0.05f/1.5f,0.05f,-0.54f,-1.285f-scrVal,0.9f,icons.Circle,0.f,glm::vec4(0.8,0.8,0.8,1.0),glm::vec4(0.06,0.12,0.15,1.0));
			glUseProgram(renderPrograms.uiProgram);

			ui.renderText(renderPrograms.uiProgram,"6",-0.55f,-1.3f-scrVal,0.0004f,glm::vec4(0.95,0.95,0.95,1.0),0.91f,false);
			ui.renderText(renderPrograms.uiProgram,"Upload 3D Models",-0.47f,-1.3f-scrVal,0.0004f,glm::vec4(0.06,0.12,0.15,1.0),0.91f,false);

			float posX = 0;
			float posY = -0.31f;
			for (size_t i = 0; i < tdModelPaths.size()+1; i++)
			{
				posX += (0.15*2);
				if(i%4 == 0){
					posX = 0.f;
					posY += 0.31f;
				}				
				bool buttonEnter = ui.isMouseOnButton(window,0.15f,0.15f,-0.37f + posX,-1.5f-scrVal - posY,mouseXpos,mouseYpos,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);
        		if(buttonEnter)
					nodePanel.pointerCursor = true;
				ui.box(0.15f,0.15f,-0.37f + posX,-1.5f-scrVal - posY,"",glm::vec4(0.06,0.12,0.15,1.0),0,false,false,0.92f,10,glm::vec4(colorData.LigidPainterThemeColor,1.0),buttonEnter,{},{},0,true);
				
				if(i == 0){
					if(buttonEnter && firstClick){
						char const* lFilterPatterns[11] = { "*.obj","*.gltf", "*.fbx", "*.stp", "*.max","*.x3d","*.obj","*.vrml","*.3ds","*.stl","*.dae" };	
						char * modelFilePathCheck = tinyfd_openFileDialog("Select 3D Model","",11, lFilterPatterns,"",false);
						if(modelFilePathCheck)
							tdModelPaths.push_back(modelFilePathCheck);
					}
					
					glUseProgram(renderPrograms.iconsProgram);
					ui.iconBox(0.05f/1.5f,0.05f,-0.37f + posX,-1.45f-scrVal - posY,0.95f,icons.Plus,buttonEnter,glm::vec4(0.06,0.12,0.15,1.0),glm::vec4(colorData.LigidPainterThemeColor,1.0));
					
					glUseProgram(renderPrograms.uiProgram);
					ui.renderText(renderPrograms.uiProgram,"Add Model", -0.3f + posX - 0.15f,-1.56f-scrVal - posY,0.00032f,glm::vec4(0.06,0.12,0.15,1.0),0.95f,false,10,0,buttonEnter);
				}
				else{
					ui.renderText(renderPrograms.uiProgram,util.getLastWordBySeparatingWithChar(tdModelPaths[i-1],folderDistinguisher), -0.47f + posX,-1.5f-scrVal - posY,0.00032f,glm::vec4(0.06,0.12,0.15,1.0),0.95f,false,-0.37f + posX + 0.14f, false,buttonEnter);
					
					//Delete Button
					bool delButtonEnter = ui.isMouseOnButton(window,0.03f/1.5f,0.03f,-0.477f + posX,-1.585f-scrVal - posY,mouseXpos,mouseYpos,false,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);
					ui.box(0.03f/1.5f,0.03f,-0.477f + posX,-1.585f-scrVal - posY,"",glm::vec4(0.86,0.12,0.15,1.0),0,false,false,0.92f,6,glm::vec4(colorData.LigidPainterThemeColor,1.0),delButtonEnter,{},{},0,false);
					glUseProgram(renderPrograms.iconsProgram);
					ui.iconBox(0.03f/1.5f,0.03f,-0.477f + posX,-1.585f-scrVal - posY,0.95f,icons.Trash,0,glm::vec4(1.0,1.0,1.0,1.0),glm::vec4(0.8,0.8,0.8,1.0));
				
					if(delButtonEnter && firstClick){
						tdModelPaths.erase(tdModelPaths.begin()+i-1);
						break;
					}
					glUseProgram(renderPrograms.uiProgram);
				}
			}

			glUseProgram(renderPrograms.uiProgram);
			
			


			if(createProjectErrorMsg != "")
				createProjectErrorMsgCounter++;
			if(createProjectErrorMsgCounter == 500){
				createProjectErrorMsgCounter = 0;
				createProjectErrorMsg = "";
			}
			ui.box(0.9f,0.08f,-1.0 + 0.4f + 0.9f,-1.0f+0.08,"",glm::vec4(1.0,1.0,1.0,0.8),0,false,false,0.98f,10000,glm::vec4(colorData.LigidPainterThemeColor,1.0),0);

			renderer.startScreenCreateTheProjectButton.draw(glm::vec3(-1.0 + 0.6f + 1.f,-1.0f+0.08,0.99f),glm::vec2(mouseXpos,mouseYpos));

			ui.renderText(renderPrograms.uiProgram,createProjectErrorMsg, -0.1f,-1.0f+0.06,0.0004f,glm::vec4(colorData.LigidPainterThemeColor,1.f),0.99f,false);

			if(renderer.startScreenCreateTheProjectButton.buttonEnter)
				nodePanel.pointerCursor = true;
			if(renderer.startScreenCreateTheProjectButton.buttonEnter && firstClick){
				
				bool success = true;

				if(!std::filesystem::exists(projectPath)){
					createProjectErrorMsg = "Project path doesn't exists"; 
					success = false;
				}
				if(std::filesystem::exists(projectPath + folderDistinguisher + renderer.startScreenProjectTitleTextBox.text)){
					createProjectErrorMsg = "Project path already exists"; 
					success = false;
				}
				if(renderer.startScreenProjectTitleTextBox.text == ""){
					createProjectErrorMsg = "Project title is empty"; 
					success = false;
				}
				if(tdModelPaths.size() == 0){
					createProjectErrorMsg = "There are no 3D Models selected"; 
					success = false;
				}
				if(success){
					ProjectFolder project;
					project.initFolder(projectPath,renderer.startScreenProjectTitleTextBox.text,renderer.startScreenIncludeTexturesCheckBox.checked,renderer.startScreenIncludeNodesCheckBox.checked,
									   renderer.startScreenIncludeFontsCheckBox.checked,tdModelPaths,selectedSkyBox,UIElements);

					project.readFolder(projectPath + folderDistinguisher + renderer.startScreenProjectTitleTextBox.text + ".ligid" ,materials,appNodes,addNodeContexMenu,model,UIElements,
									   albedoTextures,fonts,selectedSkyBox);
					startScreen = false;
				}
			}
		}
		else if(starctScLoadProjectMode){
			ui.renderText(renderPrograms.uiProgram,"Load a project",-0.55f,0.8f,0.0006f,glm::vec4(0.06,0.12,0.15,1.0),0.91f,false);
			
			renderer.startScreenLoadAProjectButton.draw(glm::vec3(-0.35f,0.5f,0.95f),glm::vec2(mouseXpos,mouseYpos));
			if(renderer.startScreenLoadAProjectButton.buttonEnter)
				nodePanel.pointerCursor = true;
			if(renderer.startScreenLoadAProjectButton.buttonEnter && firstClick){
				char const* lFilterPatterns[11] = { "*.ligid" };	
				char * projectFilePathCheck = tinyfd_openFileDialog("Select 3D Model","", 1, lFilterPatterns,"",false);
				if(projectFilePathCheck){
					ProjectFolder project;
					project.readFolder(projectFilePathCheck ,materials,appNodes,addNodeContexMenu,model,UIElements,albedoTextures,fonts,selectedSkyBox);
					projectPath = projectFilePathCheck;
					std::string projectPathName = util.getLastWordBySeparatingWithChar(projectPath,folderDistinguisher);
					projectPath.erase(projectPath.end()-projectPathName.size(),projectPath.end());
					startScreen = false;
				}
			}

			ui.renderText(renderPrograms.uiProgram, "Name" ,-0.4f,0.3f,0.0004f,glm::vec4(0.06,0.12,0.15,1.0),0.93f,false);
			ui.renderText(renderPrograms.uiProgram, "Last Opened" , 0.45f,0.3f,0.0004f,glm::vec4(0.06,0.12,0.15,1.0),0.93f,false);
			ui.renderText(renderPrograms.uiProgram, "Creation Date" , 0.05f,0.3f,0.0004f,glm::vec4(0.06,0.12,0.15,1.0),0.93f,false);

			//Barrier
			bool barrierEnter = ui.isMouseOnButton(window,1.0f,0.45f,0.15f,0.7,mouseXpos,mouseYpos,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);
			ui.box(1.0f,0.45f,0.15f,0.7,"",glm::vec4(0),0,false,false,0.99f,11100,glm::vec4(0),false,{},{},0,false);

			int fileCounter = 0;
			float posY = -0.2 + startScreenLoadPanelScrollVal;
			for (const auto & entry : std::filesystem::directory_iterator("./Projects")){
				std::string filePath = entry.path().string();
				std::string fileName = util.getLastWordBySeparatingWithChar(filePath,folderDistinguisher);
				std::string path = filePath + folderDistinguisher + fileName + ".ligid";

				uint64_t h1 = 0xAB428C9F; 
         		uint64_t h2 = 0xFF8A1C1C; 
         		uint64_t h3 = 0x4B4B9AAA; 
         		std::ifstream rf(path, std::ios::out | std::ios::binary);

		 		if(rf) {
		 			std::string Cdt; //Creation date
	     			std::string Ldt; //Last opening date

		 			uint64_t c1; 
         			uint64_t c2; 
         			uint64_t c3; 
         			rf.read(reinterpret_cast<char*>(&c1),sizeof(uint64_t));
         			rf.read(reinterpret_cast<char*>(&c2),sizeof(uint64_t));
         			rf.read(reinterpret_cast<char*>(&c3),sizeof(uint64_t));

         			if(c1 == h1 && c2 == h2 && c3 == h3){
		 				uint64_t timestrsize;
            	 		rf.read(reinterpret_cast<char*>(&timestrsize),sizeof(uint64_t));
            	 		for (size_t i = 0; i < timestrsize; i++)
            	 		{
		 					char c;
            	 		    rf.read(reinterpret_cast<char*>(&c),sizeof(char));
		 					Cdt.push_back(c);
            	 		}

		 				uint64_t timestrsize2;
            	 		rf.read(reinterpret_cast<char*>(&timestrsize2),sizeof(uint64_t));
            	 		for (size_t i = 0; i < timestrsize2; i++)
            	 		{
		 					char c;
            	 		    rf.read(reinterpret_cast<char*>(&c),sizeof(char));
		 					Ldt.push_back(c);
            	 		}
						bool buttonEnter = ui.isMouseOnButton(window,0.7f,0.06f,0.15f,-0.016f-posY,mouseXpos,mouseYpos,0,glfwGetVideoMode(glfwGetPrimaryMonitor())->height,glfwGetVideoMode(glfwGetPrimaryMonitor())->height/1.5);
						if(barrierEnter)
							buttonEnter = false;
						if(buttonEnter)
							nodePanel.pointerCursor = true;
						if(buttonEnter && firstClick){
							ProjectFolder project;
							project.readFolder(path ,materials,appNodes,addNodeContexMenu,model,UIElements,albedoTextures,fonts,selectedSkyBox);
							projectPath = path;
							std::string projectPathName = util.getLastWordBySeparatingWithChar(projectPath,folderDistinguisher);
							projectPath.erase(projectPath.end()-projectPathName.size(),projectPath.end());
							startScreen = false;
						}
						//Hover button
						ui.box(0.7f,0.06f,0.15f,-0.016f-posY,"",glm::vec4(0),0,false,false,0.9f,20,glm::vec4(glm::vec3(0.1),0.1),buttonEnter,{},{},0,false);
						
						//Seperator line
						ui.box(0.7f,0.06f,0.15f,-0.016f-posY,"",glm::vec4(0.06,0.12,0.15,1.0),0,false,false,0.9f,20,glm::vec4(0.66,0.72,0.75,1.0),0.f,{},{},0,true);

						//Dates
		 				ui.renderText(renderPrograms.uiProgram, (std::string)Cdt ,0.05f,-0.025f-posY,0.0003f,glm::vec4(0.06,0.12,0.15,1.0),0.93f,false);
		 				ui.renderText(renderPrograms.uiProgram, (std::string)Ldt ,0.45f,-0.025f-posY,0.0003f,glm::vec4(0.06,0.12,0.15,1.0),0.93f,false);
						
						//File name
						ui.renderText(renderPrograms.uiProgram,fileName,-0.4f,0.0f-posY,0.00035f,glm::vec4(0.06,0.12,0.15,1.0),0.95f,false,0.04,false,0.f);
						ui.renderText(renderPrograms.uiProgram,std::filesystem::absolute(filePath).string(),-0.4f,-0.05f-posY,0.00025f,glm::vec4(0.06,0.12,0.15,0.5),0.95f,false,0.04,false,0.f);

						//Index
						ui.renderText(renderPrograms.uiProgram,std::to_string(fileCounter),-0.5f,-0.04f-posY,0.0005f,glm::vec4(0.06,0.12,0.15,1.0),0.95f,false);

						posY += 0.15f;
						fileCounter++;
         			}
			}
		}
		if(posY < 1.f){
			startScreenLoadPanelScrollVal = +0.2 - posY;
		}

	}
	startScreenLastMousePosY = mouseYpos;
}