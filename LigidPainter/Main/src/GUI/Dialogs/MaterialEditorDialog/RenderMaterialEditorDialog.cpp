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

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "NodeSystem/Node/Node.hpp"
#include "SettingsSystem/Settings.hpp"
#include "MouseSystem/Mouse.hpp"
#include "ColorPaletteSystem/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

// Defined in the RenderPanel.cpp
extern bool updateThePreRenderedPanels;

bool __materialEditorDialogESCPressed = false;
bool __materialEditorDialogESCFirstFramePressed = false;

bool __lastDisplayModeComboBoxPressed = false;

bool anyEyeHover = false;

void MaterialEditorDialog::render
                                (
                                    Timer &timer,
                                    TextureSelectionDialog &textureSelectionDialog,
                                    LogDialog& logDialog,
                                    glm::mat4 projection
                                )
{
    dialogControl.updateStart();
    // ------- Positioning & Prepearing the panels to rendering -------
    navPanel.scale.x = bgPanel.scale.x - shortcutPanel.scale.x - layerPanel.scale.x - modifiersPanel.scale.x;
    navPanel.pos = shortcutPanel.pos;
    navPanel.pos.x += shortcutPanel.scale.x + navPanel.scale.x;
    navPanel.pos.y = bgPanel.pos.y - bgPanel.scale.y + navPanel.scale.y;

    this->materialDisplayer.texture = this->displayingFBO.colorBuffer;
    this->materialDisplayer.pos = navPanel.pos;
    materialDisplayer.scale.y = bgPanel.scale.y - navPanel.scale.y;
    materialDisplayer.scale.x = navPanel.scale.x;
    this->materialDisplayer.pos.y += navPanel.scale.y + materialDisplayer.scale.y;

    barButton.pos.x = bgPanel.pos.x;
    barButton.pos.y = bgPanel.pos.y - bgPanel.scale.y - barButton.scale.y;
    
    bool mouseTrackingFlag = !(textureSelectionDialog.dialogControl.isActive() || ContextMenus::materialModifier.dialogControl.isActive() || ContextMenus::addMaterialModifier.dialogControl.isActive());
    
    if(this->selectedMaterialModifierIndex < material->materialModifiers.size())
        modifiersPanel.sections = material->materialModifiers[this->selectedMaterialModifierIndex].sections;
    else
        modifiersPanel.sections.clear();
    

    displayModeComboBox.scale.y = navPanel.scale.y - 0.1;
    matDisplayerBallModeBtn.scale.y = navPanel.scale.y - 0.1;
    twoDModelModeBtn.scale.y = navPanel.scale.y - 0.1;
    customModelModeBtn.scale.y = navPanel.scale.y - 0.1;
    selectedModifierResultMode.scale.y = navPanel.scale.y - 0.1;
    overallResultMode.scale.y = navPanel.scale.y - 0.1;

    matDisplayerBallModeBtn.pos = navPanel.pos;
    matDisplayerBallModeBtn.pos.x += displayModeComboBox.scale.x - selectedModifierResultMode.scale.x * 3.f;
    twoDModelModeBtn.pos = matDisplayerBallModeBtn.pos; 
    twoDModelModeBtn.pos.x -= matDisplayerBallModeBtn.scale.x + twoDModelModeBtn.scale.x; 
    customModelModeBtn.pos = matDisplayerBallModeBtn.pos; 
    customModelModeBtn.pos.x += matDisplayerBallModeBtn.scale.x + customModelModeBtn.scale.x; 

    selectedModifierResultMode.pos = navPanel.pos;
    selectedModifierResultMode.pos.x += navPanel.scale.x - selectedModifierResultMode.scale.x * 4.f;
    overallResultMode.pos = selectedModifierResultMode.pos; 
    overallResultMode.pos.x -= selectedModifierResultMode.scale.x + overallResultMode.scale.x; 

    // ------- Rendering the panels -------
    bgPanel.render(timer, mouseTrackingFlag);
    layerPanel.pos.x = modifiersPanel.pos.x - modifiersPanel.scale.x - layerPanel.scale.x; 
    layerPanel.render(timer, mouseTrackingFlag && !anyEyeHover);

    anyEyeHover = false;

    if(layerPanel.sections.size()){
        for (size_t i = 0; i < layerPanel.sections[0].elements.size(); i++)
        {
            Button btn = layerPanel.sections[0].elements[i].button;
            Button eyeBtn = Button(ELEMENT_STYLE_SOLID, glm::vec2(0.6f), "", Settings::appTextures().eyeOpenedIcon, 0.f, false);
            eyeBtn.color.a = 0.f; 
            eyeBtn.outline = false; 
            eyeBtn.scale.y = btn.scale.y;
            eyeBtn.pos = btn.pos;
            eyeBtn.pos.x += btn.scale.x - eyeBtn.scale.x * 2.f;
        
            glm::vec3 resultPos = glm::vec3( 
                        UTIL::getPercent(*Settings::videoScale(), glm::vec2(eyeBtn.pos.x,eyeBtn.pos.y))
                        ,eyeBtn.pos.z); 

    
            glm::vec2 resultScale = UTIL::getPercent(*Settings::videoScale(), eyeBtn.scale);

            if(Mouse::isMouseHover(resultScale, resultPos)){
                anyEyeHover = true;
                eyeBtn.pos.y -= 0.2f;    
                eyeBtn.scale += 0.05f;    
            }

            if(i < material->materialModifiers.size() && btn.scale.x > 3.f){
                if(material->materialModifiers[i].hide)
                    eyeBtn.texture = Settings::appTextures().eyeClosedIcon;

                eyeBtn.render(timer, true);

                if(eyeBtn.hover && *Mouse::LClick()){
                    this->updateTheMaterial = true;
                    material->materialModifiers[i].hide = !material->materialModifiers[i].hide;
                }
            }
        }
    }

    modifiersPanel.render(timer, mouseTrackingFlag);
    barButton.render(timer, mouseTrackingFlag);
    navPanel.render(timer, mouseTrackingFlag);
    if(this->selectedMaterialModifierIndex < material->materialModifiers.size()){
        for (size_t secI = 0; secI < material->materialModifiers[this->selectedMaterialModifierIndex].sections.size(); secI++){
            for (size_t elementI = 0; elementI < material->materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements.size(); elementI++){
                bool ctrlShiftWCondition = getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_CONTROL) && getContext()->window.isKeyPressed(LIGIDGL_KEY_LEFT_SHIFT) && getContext()->window.isKeyPressed(LIGIDGL_KEY_W);
                if((material->materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[elementI].button.hover || material->materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[elementI].rangeBar.hover || material->materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[elementI].checkBox.hover) && *Mouse::RClick() && ctrlShiftWCondition){
                    registerMaterialAction("New material shortcut", *this->material);
                    this->material->materialShortcuts.push_back(MaterialShortcut("New_Shortcut", &material->materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[elementI], this->selectedMaterialModifierIndex, secI, elementI));
                }
            }
        }
    }

    shortcutPanel.sections.erase(shortcutPanel.sections.begin() + 1, shortcutPanel.sections.end());  

    for (size_t i = 0; i < this->material->materialShortcuts.size(); i++)
    {
        Section section = Section(
                                    SectionHolder(ColorPalette::secondColor, ColorPalette::thirdColor, 3.f, this->material->materialShortcuts[i].title),
                                    {
                                        *this->material->materialShortcuts[i].element,
                                        Button(ELEMENT_STYLE_STYLIZED, glm::vec2(1.f), "Rename", Texture(), 2.f, false),
                                        Button(ELEMENT_STYLE_STYLIZED, glm::vec2(1.f), "Delete", Texture(), 1.f, false),
                                    }
                                );

        section.elements[0].panelOffset = 1.f;
        section.elements[2].button.color = glm::vec4(1.f, 0.f, 0.f, 0.5f);
        if(shortcutRenamingIndex == i){
            section.header.sectionHolder.textColor = ColorPalette::secondColor;
            section.header.sectionHolder.textColor.a = 1.f;
        }
        section.header.sectionHolder.sectionColor.a = 1.f;
        section.header.sectionHolder.active = true;
        section.header.sectionHolder.fullActive = true;

        shortcutPanel.sections.push_back(section);    
    }
    
    shortcutPanel.render(timer, mouseTrackingFlag);
    
    if(shortcutRenamingIndex != -1 && shortcutRenamingIndex + 1 < shortcutPanel.sections.size() && shortcutRenamingIndex < this->material->materialShortcuts.size()){
        shortcutRenamingTextbox.active = true;
        shortcutRenamingTextbox.pos = shortcutPanel.sections[shortcutRenamingIndex + 1].header.pos;
        shortcutRenamingTextbox.scale = shortcutPanel.sections[shortcutRenamingIndex + 1].header.scale;
        shortcutRenamingTextbox.scale.y += 0.2f;
        shortcutRenamingTextbox.render(timer, false);

        this->material->materialShortcuts[shortcutRenamingIndex].title = shortcutRenamingTextbox.text; 

        if(*Mouse::LClick() && !shortcutRenamingTextbox.hover || getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) || getContext()->window.isKeyPressed(LIGIDGL_KEY_ENTER)){
            shortcutRenamingIndex = -1;
        }
    }

    for (size_t i = 0; i < this->material->materialShortcuts.size(); i++){
        if(shortcutPanel.sections[i + 1].elements[1].button.hover && *Mouse::LClick()){
            registerMaterialAction("Material shortcut renamed", *this->material);
            this->shortcutRenamingIndex = i;
            shortcutRenamingTextbox.text = this->material->materialShortcuts[i].title; 
            shortcutRenamingTextbox.activeChar = shortcutRenamingTextbox.text.size()-1; 
            shortcutRenamingTextbox.activeChar2 = shortcutRenamingTextbox.text.size()-1;
        }
        if(shortcutPanel.sections[i + 1].elements[2].button.hover && *Mouse::LClick()){
            registerMaterialAction("Material shortcut removed", *this->material);
            this->material->materialShortcuts.erase(this->material->materialShortcuts.begin() + i);    
            break;
        }

        *this->material->materialShortcuts[i].element = shortcutPanel.sections[i + 1].elements[0];
        if(shortcutPanel.sections[i + 1].elements[0].isInteracted())
            this->updateTheMaterial = true;
    }
    
    ShaderSystem::textureRenderingShader().use();
    ShaderSystem::textureRenderingShader().setMat4("projection", projection);
    ShaderSystem::textureRenderingShader().setVec2("scale", this->materialDisplayer.resultScale);
    ShaderSystem::textureRenderingShader().setVec3("pos", this->materialDisplayer.resultPos);
    ShaderSystem::textureRenderingShader().setInt("txtr", 0);
    ShaderSystem::textureRenderingShader().setFloat("opacity", this->dialogControl.mixVal);
    ShaderSystem::textureRenderingShader().setFloat("rotation", 0.f);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, this->skyboxFBO.colorBuffer.ID);

    glDrawArrays(GL_TRIANGLES, 0, 6);

    ShaderSystem::buttonShader().use();
    
    materialDisplayer.render(timer, false);
    
    if(this->displayerCamera.radius > 3.5f)
        this->displayerCamera.setCameraRadius(3.5f);
    if(this->displayerCamera.radius < 0.f)
        this->displayerCamera.setCameraRadius(0.f);

    bool displayZoomPanel = this->displayerCamera.radius != 3.5f;
    timer.transition(displayZoomPanel, this->zoomPanelMixVal, 0.2f);

    if(this->zoomPanelMixVal){
        zoomPanel.sections[0].elements[0].button.text = std::to_string(this->displayerCamera.radius);
        if(zoomPanel.sections[0].elements[0].button.text.size() > 3)
            zoomPanel.sections[0].elements[0].button.text.erase(zoomPanel.sections[0].elements[0].button.text.begin() + 3, zoomPanel.sections[0].elements[0].button.text.end());
        
        glClear(GL_DEPTH_BUFFER_BIT);

        zoomPanel.sections[0].elements[0].button.text += " z";
        zoomPanel.pos = shortcutPanel.pos;
        zoomPanel.pos.y = navPanel.pos.y;
        
        zoomPanel.scale.x = this->zoomPanelMixVal * 6.f;
        zoomPanel.sections[0].elements[0].scale.x = this->zoomPanelMixVal * 2.f;
        zoomPanel.sections[0].elements[1].scale.x = this->zoomPanelMixVal * 4.f;
        
        zoomPanel.pos.y += navPanel.scale.y + zoomPanel.scale.y;
        zoomPanel.pos.x += shortcutPanel.scale.x + zoomPanel.scale.x;
        zoomPanel.render(timer, mouseTrackingFlag);
    
        if(zoomPanel.sections[0].elements[1].button.clicked)
            this->displayerCamera.setCameraRadius(3.5f);
    }

    twoDModelModeBtn.render(timer, mouseTrackingFlag);
    matDisplayerBallModeBtn.render(timer, mouseTrackingFlag);
    customModelModeBtn.render(timer, mouseTrackingFlag);
    overallResultMode.render(timer, mouseTrackingFlag);
    selectedModifierResultMode.render(timer, mouseTrackingFlag);

    if(*Mouse::RPressed() || dialogControl.firstFrameActivated){
        this->updateSkyboxTxtr();
    }
    
    if(this->selectedMaterialModifierIndex < material->materialModifiers.size() && !shortcutPanel.hover)
        material->materialModifiers[this->selectedMaterialModifierIndex].sections = modifiersPanel.sections;

    if(twoDModelModeBtn.clicked){
        this->selectedModelModeIndex = 0;
        this->updateTheMaterial = true;
    }
    if(this->selectedModelModeIndex == 0)
        twoDModelModeBtn.color = ColorPalette::thirdColor;
    else
        twoDModelModeBtn.color = ColorPalette::secondColor;

    if(matDisplayerBallModeBtn.clicked){
        this->selectedModelModeIndex = 1;
        this->updateTheMaterial = true;
    }
    if(this->selectedModelModeIndex == 1)
        matDisplayerBallModeBtn.color = ColorPalette::thirdColor;
    else
        matDisplayerBallModeBtn.color = ColorPalette::secondColor;

    if(customModelModeBtn.clicked){
        this->selectedModelModeIndex = 2;
        this->updateTheMaterial = true;
    }
    if(this->selectedModelModeIndex == 2)
        customModelModeBtn.color = ColorPalette::thirdColor;
    else
        customModelModeBtn.color = ColorPalette::secondColor;

    if(overallResultMode.clicked){
        this->selectedResultModeIndex = 0;
        this->updateTheMaterial = true;
    }
    if(this->selectedResultModeIndex == 0)
        overallResultMode.color = ColorPalette::thirdColor;
    else
        overallResultMode.color = ColorPalette::secondColor;

    if(selectedModifierResultMode.clicked){
        this->selectedResultModeIndex = 1;
        this->updateTheMaterial = true;
    }
    if(this->selectedResultModeIndex == 1)
        selectedModifierResultMode.color = ColorPalette::thirdColor;
    else
        selectedModifierResultMode.color = ColorPalette::secondColor;


    //If texture selection dialog is not active reset the index values used to navigate textures
    if(textureSelectionDialog.dialogControl.isActive() == false){
        textureModifierTextureSelectingButtonIndex = 1000;
        textureSelectionDialog.selectedTextureIndex = 0;
    }

    //Make sure selectedMaterialModifierIndex won't cause any vector out of range error
    if(selectedMaterialModifierIndex >= material->materialModifiers.size()){
        selectedMaterialModifierIndex = material->materialModifiers.size()-1;
        if(selectedMaterialModifierIndex < 0)
            selectedMaterialModifierIndex = 0;
    }

    //Check layerpanel if any modifier is clicked & change selectedMaterialModifierIndex if clicked
    checkLayerPanel(*material);
    
    //Check the modifier's panel and update the material if interacted with any of the GUI element (show the texture selection dialog if pressed to a texture modifier's channel button)
    checkModifiersPanel(*material, textureSelectionDialog);
    
    //Manage actions of the context menus 
    this->manageContextMenuActions(*material);

    if(dialogControl.firstFrameActivated){
        this->selectedResultModeIndex = 0;
        updateLayerPanel(*material);
    }
    
    this->displayModeComboBox.pos = navPanel.pos;
    this->displayModeComboBox.pos.x -= navPanel.scale.x - displayModeComboBox.scale.x * 1.5f;
    this->displayModeComboBox.render(timer, true);
    
    dialogControl.updateEnd(timer,0.15f);

    if(!this->updateTheMaterial && this->prevUpdateTheMaterial){
        // Updating the material textures
        int specificUpdateI = -1;
        if(this->selectedResultModeIndex == 1)
            specificUpdateI = this->selectedMaterialModifierIndex;
        material->updateMaterialDisplayingTexture((float)Settings::properties()->textureRes, true, this->displayerCamera, this->displayModeComboBox.selectedIndex, true, this->displayingFBO, *this->getDisplayModel(), specificUpdateI);
    }
    
    this->prevUpdateTheMaterial = this->updateTheMaterial;
    this->updateTheMaterial = false;

    if(getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS){
        if(!__materialEditorDialogESCPressed)
            __materialEditorDialogESCFirstFramePressed = true;
        __materialEditorDialogESCPressed = true;
    }
    else{
        __materialEditorDialogESCPressed = false;
    } 

    //Close the dialog
    if(__materialEditorDialogESCFirstFramePressed || ((!bgPanel.hover && !barButton.hover && !logDialog.isHovered()) && *Mouse::LClick()) || (barButton.hover && *Mouse::LDoubleClick())){
        if(!wasTextureSelectionDialogActive() && !ContextMenus::materialModifier.dialogControl.isActive() && !ContextMenus::addMaterialModifier.dialogControl.isActive()){
            this->displayModeComboBox.selectedIndex = 0;
            // Update the material displaying texture one more time before closing the dialog
            material->updateMaterialDisplayingTexture((float)Settings::properties()->textureRes, false, Camera(), 0, false);

            updateThePreRenderedPanels = true;

            this->deactivate(textureSelectionDialog);
        }
    }

    __materialEditorDialogESCFirstFramePressed = false; 


    // Update the displaying texture
    material->updateMaterialDisplayingTexture((float)Settings::properties()->textureRes, false, this->displayerCamera, this->displayModeComboBox.selectedIndex, true, this->displayingFBO, *this->getDisplayModel(), -1);

    __lastDisplayModeComboBoxPressed = this->displayModeComboBox.pressed;

    glClear(GL_DEPTH_BUFFER_BIT);
}




















// -------------------- UTILITY FUNCTIONS --------------------
void MaterialEditorDialog::updateLayerPanel(Material &material){
    //Clear the elements of the layerPanel
    layerPanel.sections.clear();
    
    //Create the section
    Section layerPanelSection;
    
    //Give empty header to the new layer panel section
    layerPanelSection.header = Element(Button());
    
    //Push the elements one by one from the materialModifiers of the material
    for (size_t i = 0; i < material.materialModifiers.size(); i++)
    {
        Element btn = Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(2,1.5f), material.materialModifiers[i].title , material.materialModifiers[i].maskTexture, 0.f, false));
        layerPanelSection.elements.push_back(btn);
    }

    //Give the section to the layerPanel
    layerPanel.sections.push_back(layerPanelSection);
    
    //Update the material after updating layerPanel
    this->updateTheMaterial = true;
}

static bool modMoved = false; 

void MaterialEditorDialog::checkLayerPanel(Material &material){
    //Update the selected material modifier index (pressed to a modifier)
    if(layerPanel.sections.size()){
        bool anyPressed = false;
        for (size_t i = 0; i < layerPanel.sections[0].elements.size(); i++)
        {
            if(layerPanel.sections[0].elements[i].button.clicked){
                selectedMaterialModifierIndex = i; //Select the clicked button
                if(this->selectedResultModeIndex == 1)
                    this->updateTheMaterial = true;
                    
                if(selectedMaterialModifierIndex < material.materialModifiers.size() && selectedMaterialModifierIndex >= 0)
                    modifiersPanel.sections = material.materialModifiers[selectedMaterialModifierIndex].sections;
            } 

            if(i == selectedMaterialModifierIndex){
                layerPanel.sections[0].elements[i].button.color = layerPanel.sections[0].elements[i].button.color2;
                layerPanel.sections[0].elements[i].button.textColor = ColorPalette::secondColor;
            }
            else{
                layerPanel.sections[0].elements[i].button.color = ColorPalette::secondColor;
                layerPanel.sections[0].elements[i].button.textColor = ColorPalette::oppositeColor;
            }

            if(layerPanel.sections[0].elements[i].button.clickState1){ //If a modifier button is clicked 

                anyPressed = true;

                layerPanel.sections[0].elements[i].button.color.a = 0.0f;
                layerPanel.sections[0].elements[i].button.color2.a = 0.0f;
                layerPanel.sections[0].elements[i].button.textColor.a = 0.0f;
                layerPanel.sections[0].elements[i].button.textColor2.a = 0.0f;

                // Check from bottom to top                
                for (int checkI = layerPanel.sections[0].elements.size() - 1; checkI >= 0; checkI--){
                    float btnPosY = layerPanel.sections[0].elements[checkI].button.resultPos.y - layerPanel.sections[0].elements[checkI].button.resultScale.y;
                    float cursorPosY = Mouse::cursorPos()->y;

                    if(cursorPosY > btnPosY){        
                        if(i < material.materialModifiers.size() && checkI < material.materialModifiers.size()){
                            if(i != checkI){
                                registerMaterialAction("Modifier moved", material);
                                modMoved = true;
                            }

                            selectedMaterialModifierIndex = checkI; //Select the clicked button

                            MaterialModifier topModifier = material.materialModifiers[checkI];
                            MaterialModifier currentModifier = material.materialModifiers[i];

                            material.materialModifiers[i] = topModifier;
                            material.materialModifiers[checkI] = currentModifier;

                            Element btn = layerPanel.sections[0].elements[i];
                            layerPanel.sections[0].elements.erase(layerPanel.sections[0].elements.begin() + i);
                            layerPanel.sections[0].elements.insert(layerPanel.sections[0].elements.begin() + checkI, btn);
 
                            if(i != checkI){
                                // Update the material shortcuts
                                for (size_t ii = 0; ii < material.materialShortcuts.size(); ii++)
                                {
                                    if(material.materialShortcuts[ii].modI == checkI)
                                        material.materialShortcuts[ii].updateElement(material, i);
                                    else if(material.materialShortcuts[ii].modI == i)
                                        material.materialShortcuts[ii].updateElement(material, checkI);
                                }
                            }
                        }

                        break;
                    }
                }

                break;
            }
            else{
                layerPanel.sections[0].elements[i].button.color.a = 1.0f;
                layerPanel.sections[0].elements[i].button.color2.a = 1.0f;
                layerPanel.sections[0].elements[i].button.textColor.a = 1.0f;
                layerPanel.sections[0].elements[i].button.textColor2.a = 1.0f;
            }
        }

        if(!anyPressed && modMoved){
            this->dialogControl.firstFrameActivated = true;
            modMoved = false;
        }
    }
}


void MaterialEditorDialog::checkModifiersPanel(Material &material, TextureSelectionDialog &textureSelectionDialog){
    
    //Clear the modifiers panel as the panel starts
    if(dialogControl.firstFrameActivated){
        for (size_t modI = 0; modI < material.materialModifiers.size(); modI++)
        {
            for (size_t secI = 0; secI < material.materialModifiers[modI].sections.size(); secI++)
            {
                for (size_t elementI = 0; elementI < material.materialModifiers[modI].sections[secI].elements.size(); elementI++)
                {
                    //If button is clicked update the material
                    if(material.materialModifiers[modI].sections[secI].elements[elementI].state == 0)
                        material.materialModifiers[modI].sections[secI].elements[elementI].button.clicked = false;

                    //If range bar's value changed update the material
                    if(material.materialModifiers[modI].sections[secI].elements[elementI].state == 1){
                        material.materialModifiers[modI].sections[secI].elements[elementI].rangeBar.pointerPressed = false;
                        material.materialModifiers[modI].sections[secI].elements[elementI].rangeBar.leftArrowHover = false;
                        material.materialModifiers[modI].sections[secI].elements[elementI].rangeBar.rightArrowHover = false;
                        material.materialModifiers[modI].sections[secI].elements[elementI].rangeBar.defaultPointBtnPressed = false;

                    }

                    //If checkbox clicked update the material
                    if(material.materialModifiers[modI].sections[secI].elements[elementI].state == 2)
                        material.materialModifiers[modI].sections[secI].elements[elementI].checkBox.hover = false;
                }
            }
        }
    }
    
    
    //Update the material if interacted with modifier's panel
    for (size_t secI = 0; secI < modifiersPanel.sections.size(); secI++)
    {
        if(true){
            for (size_t elementI = 0; elementI < modifiersPanel.sections[secI].elements.size(); elementI++)
            {
                //If button is clicked update the material
                if(modifiersPanel.sections[secI].elements[elementI].state == 0)
                    if(modifiersPanel.sections[secI].elements[elementI].button.clicked){

                        registerMaterialAction("Modifier updated - " + modifiersPanel.sections[secI].elements[elementI].button.text, material);

                        if(secI == modifiersPanel.sections.size()-1 && elementI == modifiersPanel.sections[secI].elements.size()-1){
                            modifiersPanel.sections[secI].elements[0].button.filter.shader.ID = 0;
                            glDeleteTextures(1, &modifiersPanel.sections[secI].elements[0].button.texture.ID);
                            modifiersPanel.sections[secI].elements[0].button.texture = Texture();
                            material.materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[0].button.filter.shader.ID = 0; 
                            material.materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[0].button.texture = Texture(); 
                        }
                        if(secI == 0 && elementI == 0 && material.materialModifiers[this->selectedMaterialModifierIndex].modifierIndex == TEXTURE_MATERIAL_MODIFIER){
                            Texture* txtr = &modifiersPanel.sections[secI].elements[elementI].button.texture;
                            
                            if(txtr->ID){
                                Texture albedo;
                                Texture roughness;
                                Texture metallic;
                                Texture normalMap;
                                Texture heightMap;
                                Texture ambientOcc;

                                albedo.proceduralProps = txtr->proceduralProps;
                                albedo.generateProceduralDisplayingTexture(256, true);
                                albedo.title = "AutoFilledTexture";

                                roughness.proceduralProps = txtr->proceduralProps;
                                roughness.proceduralProps.proceduralBrightness = 1.f;
                                roughness.proceduralProps.proceduralGrayScale = true;
                                roughness.proceduralProps.proceduralNormalMap = false;
                                roughness.proceduralProps.proceduralNormalGrayScale = false;
                                roughness.proceduralProps.proceduralNormalStrength = 0.f;
                                roughness.proceduralProps.proceduralnverted = false;
                                roughness.generateProceduralDisplayingTexture(256, true);
                                roughness.title = "AutoFilledTexture";

                                metallic.proceduralProps = txtr->proceduralProps;
                                metallic.proceduralProps.proceduralBrightness = 1.f;
                                metallic.proceduralProps.proceduralGrayScale = true;
                                metallic.proceduralProps.proceduralNormalMap = false;
                                metallic.proceduralProps.proceduralNormalGrayScale = false;
                                metallic.proceduralProps.proceduralNormalStrength = 0.f;
                                metallic.proceduralProps.proceduralnverted = true;
                                metallic.generateProceduralDisplayingTexture(256, true);
                                metallic.title = "AutoFilledTexture";
                                
                                normalMap.proceduralProps = txtr->proceduralProps;
                                normalMap.proceduralProps.proceduralBrightness = 1.f;
                                normalMap.proceduralProps.proceduralGrayScale = false;
                                normalMap.proceduralProps.proceduralNormalMap = true;
                                normalMap.proceduralProps.proceduralNormalGrayScale = false;
                                normalMap.proceduralProps.proceduralNormalStrength = 1.f;
                                normalMap.proceduralProps.proceduralnverted = false;
                                normalMap.generateProceduralDisplayingTexture(256, true);
                                normalMap.title = "AutoFilledTexture";
                                
                                heightMap.proceduralProps = txtr->proceduralProps;
                                heightMap.proceduralProps.proceduralBrightness = 1.f;
                                heightMap.proceduralProps.proceduralGrayScale = true;
                                heightMap.proceduralProps.proceduralNormalMap = false;
                                heightMap.proceduralProps.proceduralNormalGrayScale = false;
                                heightMap.proceduralProps.proceduralNormalStrength = 0.f;
                                heightMap.proceduralProps.proceduralnverted = true;
                                heightMap.generateProceduralDisplayingTexture(256, true);
                                heightMap.title = "AutoFilledTexture";
                                
                                ambientOcc.proceduralProps = txtr->proceduralProps;
                                ambientOcc.proceduralProps.proceduralBrightness = 2.f;
                                ambientOcc.proceduralProps.proceduralGrayScale = true;
                                ambientOcc.proceduralProps.proceduralNormalMap = false;
                                ambientOcc.proceduralProps.proceduralNormalGrayScale = false;
                                ambientOcc.proceduralProps.proceduralNormalStrength = 0.f;
                                ambientOcc.proceduralProps.proceduralnverted = false;
                                ambientOcc.generateProceduralDisplayingTexture(256, true);
                                ambientOcc.title = "AutoFilledTexture";
                                
                                material.materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[1].button.texture = albedo;
                                material.materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[2].button.texture = roughness;
                                material.materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[3].button.texture = metallic;
                                material.materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[4].button.texture = normalMap;
                                material.materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[5].button.texture = heightMap;
                                material.materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[6].button.texture = ambientOcc;
                                
                                modifiersPanel.sections[secI].elements[1].button.texture = albedo;
                                modifiersPanel.sections[secI].elements[2].button.texture = roughness;
                                modifiersPanel.sections[secI].elements[3].button.texture = metallic;
                                modifiersPanel.sections[secI].elements[4].button.texture = normalMap;
                                modifiersPanel.sections[secI].elements[5].button.texture = heightMap;
                                modifiersPanel.sections[secI].elements[6].button.texture = ambientOcc;

                                txtr->title = "";
                                txtr->ID = 0;
                            }

                        }

                        this->updateTheMaterial = true;
                        //material.materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[elementI].button = modifiersPanel.sections[secI].elements[elementI].button; 
                    }

                //If range bar's value changed update the material
                if(modifiersPanel.sections[secI].elements[elementI].state == 1)
                    if(modifiersPanel.sections[secI].elements[elementI].rangeBar.pointerPressed == true || modifiersPanel.sections[secI].elements[elementI].rangeBar.defaultPointBtnPressed || (modifiersPanel.sections[secI].elements[elementI].rangeBar.leftArrowHover && *Mouse::LClick()) || (modifiersPanel.sections[secI].elements[elementI].rangeBar.rightArrowHover && *Mouse::LClick())){
                        if(modifiersPanel.sections[secI].elements[elementI].rangeBar.hover && *Mouse::LClick() || modifiersPanel.sections[secI].elements[elementI].rangeBar.defaultPointBtnPressed || (modifiersPanel.sections[secI].elements[elementI].rangeBar.leftArrowHover && *Mouse::LClick()) || (modifiersPanel.sections[secI].elements[elementI].rangeBar.rightArrowHover && *Mouse::LClick()))
                            registerMaterialAction("Modifier updated - " + modifiersPanel.sections[secI].elements[elementI].rangeBar.text, material);
                        
                        this->updateTheMaterial = true;
                        //material.materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[elementI].rangeBar = modifiersPanel.sections[secI].elements[elementI].rangeBar; 
                    }

                //If checkbox clicked update the material
                if(modifiersPanel.sections[secI].elements[elementI].state == 2)
                    if(modifiersPanel.sections[secI].elements[elementI].checkBox.hover && *Mouse::LClick() == true){
                        registerMaterialAction("Modifier updated - " + modifiersPanel.sections[secI].elements[elementI].checkBox.text, material);

                        this->updateTheMaterial = true;
                        //material.materialModifiers[this->selectedMaterialModifierIndex].sections[secI].elements[elementI].checkBox = modifiersPanel.sections[secI].elements[elementI].checkBox; 
                    }
            }

        }
    }
}


void MaterialEditorDialog::moveModifierToTop(int index, Material &material){
    if(index != 0){
        registerMaterialAction("Modifier moved to top", material);

        MaterialModifier topModifier = material.materialModifiers[index - 1];
        MaterialModifier currentModifier = material.materialModifiers[index];

        material.materialModifiers[index] = topModifier;
        material.materialModifiers[index - 1] = currentModifier;
        
        // Update the material shortcuts
        for (size_t i = 0; i < material.materialShortcuts.size(); i++)
        {
            if(material.materialShortcuts[i].modI == index)
                material.materialShortcuts[i].updateElement(material, index - 1);
            else if(material.materialShortcuts[i].modI == index - 1)
                material.materialShortcuts[i].updateElement(material, index);
        }

        selectedMaterialModifierIndex--;
    }
}

void MaterialEditorDialog::moveModifierToBottom(int index, Material &material){
    if(ContextMenus::materialModifier.selectedElement != material.materialModifiers.size()-1){
        registerMaterialAction("Modifier moved to bottom", material);
        
        MaterialModifier bottomModifier = material.materialModifiers[index + 1];
        MaterialModifier currentModifier = material.materialModifiers[index];

        material.materialModifiers[index] = bottomModifier;
        material.materialModifiers[index + 1] = currentModifier;

        // Update the material shortcuts
        for (size_t i = 0; i < material.materialShortcuts.size(); i++)
        {
            if(material.materialShortcuts[i].modI == index)
                material.materialShortcuts[i].updateElement(material, index + 1);
            else if(material.materialShortcuts[i].modI == index + 1)
                material.materialShortcuts[i].updateElement(material, index);
        }

        selectedMaterialModifierIndex++;
    }
}

void MaterialEditorDialog::manageContextMenuActions( Material &material)
{
    if(ContextMenus::materialModifier.dialogControl.isActive()){ //If material modifier context menu is active
        // Delete button pressed
        if(ContextMenus::materialModifier.contextPanel.sections[0].elements[0].button.clicked){            
            registerMaterialAction("Modifier deleted", material);

            for (size_t i = 0; i < material.materialShortcuts.size(); i++)
            {
                if(material.materialShortcuts[i].modI == ContextMenus::materialModifier.selectedElement)
                    material.materialShortcuts.erase(material.materialShortcuts.begin() + i);
            }
            
            // Deleting the mask texture
            glDeleteTextures(1, &material.materialModifiers[ContextMenus::materialModifier.selectedElement].maskTexture.ID); 

            // Deleting the albedo filter's displaying texture
            glDeleteTextures(1, &modifiersPanel.sections[modifiersPanel.sections.size()-1].elements[0].button.texture.ID);
            glDeleteTextures(1, &modifiersPanel.sections[modifiersPanel.sections.size()-1].elements[1].button.texture.ID);
            
            if(material.materialModifiers[ContextMenus::materialModifier.selectedElement].modifierIndex == TEXTURE_MATERIAL_MODIFIER){
                glDeleteTextures(1, &material.materialModifiers[ContextMenus::materialModifier.selectedElement].sections[0].elements[1].button.texture.ID); 
                glDeleteTextures(1, &material.materialModifiers[ContextMenus::materialModifier.selectedElement].sections[0].elements[2].button.texture.ID); 
                glDeleteTextures(1, &material.materialModifiers[ContextMenus::materialModifier.selectedElement].sections[0].elements[3].button.texture.ID); 
                glDeleteTextures(1, &material.materialModifiers[ContextMenus::materialModifier.selectedElement].sections[0].elements[4].button.texture.ID); 
                glDeleteTextures(1, &material.materialModifiers[ContextMenus::materialModifier.selectedElement].sections[0].elements[5].button.texture.ID); 
                glDeleteTextures(1, &material.materialModifiers[ContextMenus::materialModifier.selectedElement].sections[0].elements[6].button.texture.ID); 
            }

            material.materialModifiers.erase(material.materialModifiers.begin() + ContextMenus::materialModifier.selectedElement);
            dialogControl.firstFrameActivated = true;
            selectedMaterialModifierIndex = 0;
        }
        
        //Move to top button pressed
        else if(ContextMenus::materialModifier.contextPanel.sections[0].elements[1].button.clicked){ 
            this->moveModifierToTop(ContextMenus::materialModifier.selectedElement, material);
            dialogControl.firstFrameActivated = true;

        }

        //Move to bottom button pressed
        else if(ContextMenus::materialModifier.contextPanel.sections[0].elements[2].button.clicked){ 
            this->moveModifierToBottom(ContextMenus::materialModifier.selectedElement, material);
            dialogControl.firstFrameActivated = true;
        }
        
        //Change mask button pressed
        else if(ContextMenus::materialModifier.contextPanel.sections[0].elements[3].button.clicked){ 
            registerMaterialAction("Modifier change mask", material);
            
            showTextureSelectionDialog(material.materialModifiers[ContextMenus::materialModifier.selectedElement].maskTexture, 128, false);

            dialogControl.firstFrameActivated = true;
        }
    }

    if(ContextMenus::addMaterialModifier.dialogControl.isActive()){ //If material modifier context menu is active
        
        char whitePixel[] = { 127, 127, 127, 127 }; // 1 pixel, RGBA format (white)
        
        // Texture Modifier
        if(ContextMenus::addMaterialModifier.contextPanel.sections[0].elements[0].button.clicked){
            registerMaterialAction("Texture modifier added", material);
            material.materialModifiers.insert(material.materialModifiers.begin(), MaterialModifier(TEXTURE_MATERIAL_MODIFIER));
            material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material.materialModifiers[0].maskTexture.proceduralProps.proceduralID = 24;
            updateLayerPanel(material);
            selectedMaterialModifierIndex = 0;
        }
        
        // Dust Modifier
        else if(ContextMenus::addMaterialModifier.contextPanel.sections[0].elements[1].button.clicked){
            registerMaterialAction("Dust modifier added", material);
            material.materialModifiers.insert(material.materialModifiers.begin(), MaterialModifier(DUST_MATERIAL_MODIFIER));
            material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material.materialModifiers[0].maskTexture.proceduralProps.proceduralID = 24;
            updateLayerPanel(material);
            selectedMaterialModifierIndex = 0;
        }
        
        // Asphalt Modifier
        else if(ContextMenus::addMaterialModifier.contextPanel.sections[0].elements[2].button.clicked){
            registerMaterialAction("Asphalt modifier added", material);
            material.materialModifiers.insert(material.materialModifiers.begin(), MaterialModifier(ASPHALT_MATERIAL_MODIFIER));
            material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material.materialModifiers[0].maskTexture.proceduralProps.proceduralID = 24;
            updateLayerPanel(material);
            selectedMaterialModifierIndex = 0;
        }
        
        // Liquid Modifier
        else if(ContextMenus::addMaterialModifier.contextPanel.sections[0].elements[3].button.clicked){
            registerMaterialAction("Liquid modifier added", material);
            material.materialModifiers.insert(material.materialModifiers.begin(), MaterialModifier(LIQUID_MATERIAL_MODIFIER));
            material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material.materialModifiers[0].maskTexture.proceduralProps.proceduralID = 24;
            updateLayerPanel(material);
            selectedMaterialModifierIndex = 0;
        }
        
        // Moss Modifier
        else if(ContextMenus::addMaterialModifier.contextPanel.sections[0].elements[4].button.clicked){
            registerMaterialAction("Moss modifier added", material);
            material.materialModifiers.insert(material.materialModifiers.begin(), MaterialModifier(MOSS_MATERIAL_MODIFIER));
            material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material.materialModifiers[0].maskTexture.proceduralProps.proceduralID = 24;
            updateLayerPanel(material);
            selectedMaterialModifierIndex = 0;
        }
        
        // Rust Modifier
        else if(ContextMenus::addMaterialModifier.contextPanel.sections[0].elements[5].button.clicked){
            registerMaterialAction("Rust modifier added", material);
            material.materialModifiers.insert(material.materialModifiers.begin(), MaterialModifier(RUST_MATERIAL_MODIFIER));
            material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material.materialModifiers[0].maskTexture.proceduralProps.proceduralID = 24;
            updateLayerPanel(material);
            selectedMaterialModifierIndex = 0;
        }
        
        // Skin Modifier
        else if(ContextMenus::addMaterialModifier.contextPanel.sections[0].elements[6].button.clicked){
            registerMaterialAction("Skin modifier added", material);
            material.materialModifiers.insert(material.materialModifiers.begin(), MaterialModifier(SKIN_MATERIAL_MODIFIER));
            material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material.materialModifiers[0].maskTexture.proceduralProps.proceduralID = 24;
            updateLayerPanel(material);
            selectedMaterialModifierIndex = 0;
        }
        
        // Solid Modifier
        else if(ContextMenus::addMaterialModifier.contextPanel.sections[0].elements[7].button.clicked){
            registerMaterialAction("Solid modifier added", material);
            material.materialModifiers.insert(material.materialModifiers.begin(), MaterialModifier(SOLID_MATERIAL_MODIFIER));
            material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material.materialModifiers[0].maskTexture.proceduralProps.proceduralID = 24;
            updateLayerPanel(material);
            selectedMaterialModifierIndex = 0;
        }
        
        // Wooden Modifier
        else if(ContextMenus::addMaterialModifier.contextPanel.sections[0].elements[8].button.clicked){
            registerMaterialAction("Wooden modifier added", material);
            material.materialModifiers.insert(material.materialModifiers.begin(), MaterialModifier(WOODEN_MATERIAL_MODIFIER));
            material.materialModifiers[0].maskTexture = Texture(whitePixel, 1, 1, GL_NEAREST);
            material.materialModifiers[0].maskTexture.proceduralProps.proceduralID = 24;
            updateLayerPanel(material);
            selectedMaterialModifierIndex = 0;
        }
    }
}