
 /*
 ---------------------------------------------------------------------------
 LigidPainter - 3D Model texturing software / Texture generator   
 ---------------------------------------------------------------------------

 Copyright (c) 2022-2023, Mert Tetik

 All rights reserved.

 Official GitHub Link : https:github.com/mert-tetik/LigidPainter
 Official Web Page : https:ligidtools.com/ligidpainter

 ---------------------------------------------------------------------------
 */

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "3D/ThreeD.hpp" 
#include "GUI/GUI.hpp" 
#include "NodeSystem/Node/Node.hpp" 
#include "MouseSystem/Mouse.hpp"
#include "SettingsSystem/Settings.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"
#include "LibrarySystem/Library.hpp"

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

static void drawBG(
                    unsigned int bgTexture
                )
{
    ShaderSystem::defaultFramebufferShader().use();
    ShaderSystem::defaultFramebufferShader().setMat4("projection", glm::ortho(0.f, 1.f, 1.f, 0.f));
    ShaderSystem::defaultFramebufferShader().setVec3("pos", glm::vec3(0.5f, 0.5f, 0.9f));
    ShaderSystem::defaultFramebufferShader().setVec2("scale", glm::vec2(0.5f));
    
    ShaderSystem::defaultFramebufferShader().setVec2("resolution", Settings::defaultFramebuffer()->resolution);
    ShaderSystem::defaultFramebufferShader().setInt("txtr", 0);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, bgTexture);

    glDrawArrays(GL_TRIANGLES, 0, 6);
    
    ShaderSystem::buttonShader().use();
}

ProjectRecoverDialog::ProjectRecoverDialog(){
    //Create the panel
    this->panel = Panel(
                            
                            {
                                Section(
                                    Element(Button()),
                                    {
                                    }
                                )
                            },
                            glm::vec2(40,40),
                            glm::vec3(50.f,50.f,0.8f),
                            ColorPalette::mainColor,
                            ColorPalette::thirdColor,
                            true,
                            true,
                            true,
                            true,
                            true,
                            1.f,
                            1,
                            {},
                            0.25f,
                            false
                        );

    lpIconBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(5.f), "", Settings::appTextures().ligidPainterIcon, 0.f, false);
    lpIconBtn.color = glm::vec4(0.);
    lpIconBtn.outlineColor = glm::vec4(0.);
    
    recover1Btn = Button(ELEMENT_STYLE_BASIC, glm::vec2(10.f, 20.f), "Slot 1", Texture(), 0.f, false);
    recover2Btn = Button(ELEMENT_STYLE_BASIC, glm::vec2(10.f, 20.f), "Slot 2", Texture(), 0.f, false);
    recover3Btn = Button(ELEMENT_STYLE_BASIC, glm::vec2(10.f, 20.f), "Slot 3", Texture(), 0.f, false);

    recover1Btn.textScale = 0.9f;
    recover2Btn.textScale = 0.9f;
    recover3Btn.textScale = 0.9f;

    projectPanel = Panel(
                            
                            {
                                Section(
                                    Element(Button()),
                                    {
                                    }
                                )
                            },
                            glm::vec2(30,30),
                            glm::vec3(50.f,50.f,0.8f),
                            ColorPalette::mainColor,
                            ColorPalette::thirdColor,
                            true,
                            true,
                            true,
                            true,
                            true,
                            1.f,
                            1,
                            {},
                            0.25f,
                            false
                        );

    glm::vec2 projectPnSc = glm::vec2(projectPanel.scale.x/3, projectPanel.scale.y/1.2f);

    project_texturesPanel = Panel(
                            
                            {
                                Section(
                                    Element(Button()),
                                    {
                                    }
                                )
                            },
                            projectPnSc,
                            glm::vec3(projectPanel.pos.x - projectPnSc.x * 2.f, projectPanel.pos.y - projectPanel.scale.y + projectPnSc.y, 0.8f),
                            ColorPalette::secondColor,
                            ColorPalette::thirdColor,
                            true,
                            true,
                            true,
                            true,
                            true,
                            1.f,
                            1,
                            {},
                            0.25f,
                            false
                        );
    project_materialsPanel = Panel(
                            
                            {
                                Section(
                                    Element(Button()),
                                    {
                                    }
                                )
                            },
                            projectPnSc,
                            glm::vec3(projectPanel.pos.x, projectPanel.pos.y - projectPanel.scale.y + projectPnSc.y, 0.8f),
                            ColorPalette::secondColor,
                            ColorPalette::thirdColor,
                            true,
                            true,
                            true,
                            true,
                            true,
                            1.f,
                            1,
                            {},
                            0.25f,
                            false
                        );
    project_brushesPanel = Panel(
                            
                            {
                                Section(
                                    Element(Button()),
                                    {
                                    }
                                )
                            },
                            projectPnSc,
                            glm::vec3(projectPanel.pos.x + projectPnSc.x * 2.f, projectPanel.pos.y - projectPanel.scale.y + projectPnSc.y, 0.8f),
                            ColorPalette::secondColor,
                            ColorPalette::thirdColor,
                            true,
                            true,
                            true,
                            true,
                            true,
                            1.f,
                            1,
                            {},
                            0.25f,
                            false
                        );
}


void ProjectRecoverDialog::render(Timer timer, Project &project, AppMaterialModifiers appMaterialModifiers){
    
    dialogControl.updateStart();

    this->dialogControl.activate();

    while (!getContext()->window.shouldClose())
    {
        project.discardUpdateProjectFlag = true;

        getContext()->window.pollEvents();

        // Prevent rendering the application if the window is minimized
        while (getContext()->window.isMinimized()){
            getContext()->window.pollEvents();
        }
        
        glClearColor(0,0,0,0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        drawBG(Settings::defaultFramebuffer()->bgTxtr.ID);
        
        glClear(GL_DEPTH_BUFFER_BIT);

        dialogControl.updateStart();

        // Render the background panel
        this->panel.render(timer, false);
        // Render the ligidpainter icon
        this->lpIconBtn.pos = this->panel.pos;
        this->lpIconBtn.pos.y -= this->panel.scale.y - this->lpIconBtn.scale.y * 1.5f;

        this->recover1Btn.pos = this->panel.pos;
        this->recover1Btn.pos.y += 10.f;
        this->recover1Btn.pos.x -= 25.f;

        this->recover2Btn.pos = this->recover1Btn.pos;
        this->recover2Btn.pos.x += 25.f;
        
        this->recover3Btn.pos = this->recover2Btn.pos;
        this->recover3Btn.pos.x += 25.f;

        recover1Btn.locked = !std::filesystem::exists(project.recoverSlotPath(1));
        recover2Btn.locked = !std::filesystem::exists(project.recoverSlotPath(2));
        recover3Btn.locked = !std::filesystem::exists(project.recoverSlotPath(3));

        recover1Btn.render(timer, true);
        recover2Btn.render(timer, true);
        recover3Btn.render(timer, true);

        if(recover1Btn.clicked || recover2Btn.clicked || recover3Btn.clicked){
            projectSelectionMode = true;

            int slot = 1;

            if(recover2Btn.clicked)
                slot = 2;
            
            else if(recover3Btn.clicked)
                slot = 3;
        
            project_texturesPanel.sections[0].elements.clear();
            project_brushesPanel.sections[0].elements.clear();
            project_materialsPanel.sections[0].elements.clear();

            project_texturesPanel.sections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(10.f, 2.f), "Textures", Texture(), 0.f, false));
            project_brushesPanel.sections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(10.f, 2.f), "Brushes", Texture(), 0.f, false));
            project_materialsPanel.sections[0].elements.push_back(Button(ELEMENT_STYLE_SOLID, glm::vec2(10.f, 2.f), "Materials", Texture(), 0.f, false));

            try
            {
                if(std::filesystem::exists(project.recoverSlotPath(slot) + UTIL::folderDistinguisher() + "Textures")){
                    for (const auto& entry : std::filesystem::directory_iterator(project.recoverSlotPath(slot) + UTIL::folderDistinguisher() + "Textures")) {
                        std::string path = entry.path().string();

                        if(std::filesystem::is_regular_file(path)){
                            Texture txtr;
                            txtr.load(path.c_str(), glm::ivec2(256));

                            std::string title = UTIL::removeExtension(UTIL::getLastWordBySeparatingWithChar(path, UTIL::folderDistinguisher()));

                            project_texturesPanel.sections[0].elements.push_back(Button(ELEMENT_STYLE_BASIC, glm::vec2(10.f, 3.f), title, txtr, 0.f, false));
                            project_texturesPanel.sections[0].elements[project_texturesPanel.sections[0].elements.size() - 1].button.textureSizeScale = 0.98f;
                        }
                    }            
                }
                if(std::filesystem::exists(project.recoverSlotPath(slot) + UTIL::folderDistinguisher() + "Brushes")){
                    for (const auto& entry : std::filesystem::directory_iterator(project.recoverSlotPath(slot) + UTIL::folderDistinguisher() + "Brushes")) {
                        std::string path = entry.path().string();

                        if(std::filesystem::is_regular_file(path)){
                            std::string title = UTIL::removeExtension(UTIL::getLastWordBySeparatingWithChar(path, UTIL::folderDistinguisher()));
                            project_brushesPanel.sections[0].elements.push_back(Button(ELEMENT_STYLE_BASIC, glm::vec2(10.f, 2.f), title, Texture(), 0.f, false));
                        }
                    }            
                }
                if(std::filesystem::exists(project.recoverSlotPath(slot) + UTIL::folderDistinguisher() + "Materials")){
                    for (const auto& entry : std::filesystem::directory_iterator(project.recoverSlotPath(slot) + UTIL::folderDistinguisher() + "Materials")) {
                        std::string path = entry.path().string();

                        if(std::filesystem::is_regular_file(path)){
                            Material mat = Material("", 0);
                            FileHandler::readLGDMATERIALFile(path, mat, appMaterialModifiers);

                            Camera cam;
                            cam.cameraPos = glm::vec3(0,0,-5.f);
                            cam.radius = 5.f;                            
                            mat.updateMaterialDisplayingTexture(256, true, cam, 0);

                            std::string title = UTIL::removeExtension(UTIL::getLastWordBySeparatingWithChar(path, UTIL::folderDistinguisher()));
                            project_materialsPanel.sections[0].elements.push_back(Button(ELEMENT_STYLE_BASIC, glm::vec2(10.f, 3.f), title, mat.displayingTexture, 0.f, false));
                            project_materialsPanel.sections[0].elements[project_materialsPanel.sections[0].elements.size() - 1].button.textureSizeScale = 0.98f;
                        }
                    }            
                }
            }
            catch (const std::filesystem::filesystem_error& ex) {
                LGDLOG::start << "ERROR : Filesystem : Location ID 784689 " << ex.what() << LGDLOG::end;
            }
        }
            
        if(projectSelectionMode){
            projectPanel.render(timer, false);
            project_texturesPanel.render(timer, false);
            project_materialsPanel.render(timer, false);
            project_brushesPanel.render(timer, false);
        }


        this->lpIconBtn.render(timer, false);

        dialogControl.updateEnd(timer,0.15f);

        //End the dialog
        if((getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE)) || (!this->panel.hover && *Mouse::LClick()) && !projectSelectionMode){
            dialogControl.unActivate();
        }
        
        if((getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE)) || (!this->projectPanel.hover && *Mouse::LClick()) && projectSelectionMode){
            projectSelectionMode = false;
        }


        if(!this->dialogControl.isActive())
            break;
            
        dialogControl.updateEnd(timer,0.15f);

        getContext()->window.swapBuffers();

        //Set mouse states to default
        *Mouse::LClick() = false;
        *Mouse::RClick() = false;
        *Mouse::MClick() = false;
        *Mouse::LDoubleClick() = false;
        *Mouse::mouseOffset() = glm::vec2(0);
        *Mouse::mods() = 0;
        *Mouse::mouseScroll() = 0;
        *Mouse::action() = 0;
        Mouse::updateCursor();  


        //Set keyboard states to default
        textRenderer.keyInput = false;
        textRenderer.mods = 0;

        Settings::defaultFramebuffer()->render();    
        Settings::defaultFramebuffer()->setViewport();   
    }

    ShaderSystem::buttonShader().use();
    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport(); 

}