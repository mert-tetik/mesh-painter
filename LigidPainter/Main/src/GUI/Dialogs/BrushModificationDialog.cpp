/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Texture selection dialog is responsible of modifying the procedural variables inside of a Texture class & generating displaying textures

*/

#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "GUI/GUI.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"

#include <string>
#include <iostream>
#include <vector>

static const unsigned int brushTextureResolution = 256; 

BrushModificationDialog::BrushModificationDialog(int){
    this->bgPanel = Panel(
                            {
                                Section(
                                    Element(),
                                    {   
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),"Radius"  , Texture(), 2.f, 0.005f, 0.05f, 0.01f)), //0
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),"Opacity"  , Texture(), 1.5f, 0.f, 1.f, 1.f)), //1
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),"Hardness"  , Texture(), 1.5f, -10.f, 10.f, 0.f)), //2
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),"Spacing"  , Texture(), 1.5f, 0.f, 100.f, 0.f)), //3

                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),"Size Jitter"  , Texture(), 4.f, 0.f, 1.f, 0.f)), //4
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),"Fade"  , Texture(), 1.5f, 0.f, 1.f, 0.f)), //5
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Sin Wave Pattern"  , 1.f)), //6
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),"Scatter"  , Texture(), 1.5f, 0.f, 1.f, 0.f)), //7
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,4),"Texture"  , Texture(), 1.5f, false)), //8
                                        Element(Button(ELEMENT_STYLE_SOLID,glm::vec2(2,1),"Remove"  , Texture(), 0.f,false)), //9
                                        Element(CheckBox(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Individual Texture"  , 1.f)), //10
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),"Rotation"  , Texture(), 1.5f, 0.f, 360.f, 0.f)), //11
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),"Rotation Jitter"  , Texture(), 1.5f, 0.f, 1.f, 0.f)), //12
                                        Element(RangeBar(ELEMENT_STYLE_SOLID, glm::vec2(2,1),"Alpha Jitter"  , Texture(), 1.5f, 0.f, 1.f, 0.f)), //13
                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Set Default Properties"  , Texture(), 2.f,false)), //14
                                        Element(Button(ELEMENT_STYLE_BASIC,glm::vec2(2,2),"Cancel Changes"  , Texture(), 1.f,false)),//15
                                    }
                                )
                            }, 
                        glm::vec2(10.f, 20.f), glm::vec3(50.f, 60.f, 0.7), ColorPalette::secondColor, ColorPalette::thirdColor, true, true, false, true, true, 1.f, 1, {}, 20.f, true);
    
    this->libraryBrushSelectionPanel = Panel({Section()}, glm::vec2(10.f, 20.f), glm::vec3(50.f, 50.f, 0.7), ColorPalette::secondColor, ColorPalette::thirdColor, true, true, false, true, true, 1.f, 1, {}, 20.f, true);
    this->libraryBrushSelectionPanel.solidStyle = true;
    this->libraryBrushSelectionButton = Button(ELEMENT_STYLE_SOLID, glm::vec2(2,2), "Apply From Library Brushes", Texture(), 0.f, true);
    this->libraryBrushSelectionButton.color2 = this->libraryBrushSelectionButton.color;
    
    this->bgPanel.solidStyle = true;
    
    this->bgPanel.sections[0].elements[8].button.textureSelection2D = true;
    this->bgPanel.sections[0].elements[8].button.texture.proceduralProps.proceduralID = 24; //Solid white
    this->bgPanel.sections[0].elements[8].button.texture.proceduralProps.proceduralnverted = 0;
    this->bgPanel.sections[0].elements[8].button.texture.proceduralProps.proceduralScale = 1.f;
    this->bgPanel.sections[0].elements[8].button.texture.title = "AutoGeneratedMask";

    this->brushDisplayBtn = Button(ELEMENT_STYLE_SOLID,glm::vec2(2,8), ""  , Texture(), 7.f, false);
}

//Forward declarations for the utility functions
static void drawBG(unsigned int bgTexture);

BrushProperties initialProperties;

Brush displayBrush;

void BrushModificationDialog::show(Timer &timer, BrushProperties* brushProperties){
        
    Settings::defaultFramebuffer()->setViewport(); 

    char whiteTxtr[brushTextureResolution * brushTextureResolution * 4];
    for (size_t i = 0; i < brushTextureResolution * brushTextureResolution * 4; i++)
    {
        whiteTxtr[i] = 127;
    }
        
    if(!this->bgPanel.sections[0].elements[8].button.texture.ID){
        this->bgPanel.sections[0].elements[8].button.texture = Texture(whiteTxtr, brushTextureResolution, brushTextureResolution);
    }
    if(!brushProperties->brushTexture.ID){
        brushProperties->brushTexture = Texture(whiteTxtr, brushTextureResolution, brushTextureResolution);
    }
    

    this->outToIn(brushProperties);
    this->updateDisplayingTexture(brushProperties);

    initialProperties = *brushProperties;

    this->bgPanel.slideVal = 0.f;
    
    this->dialogControl.activate();

    while (!getContext()->window.shouldClose())
    {
        dialogControl.updateStart(true);

        //Render the panel
        this->bgPanel.render(timer, !dialog_log.isHovered());
        this->brushDisplayBtn.texture = displayBrush.displayingTexture;
        this->brushDisplayBtn.scale.x = this->bgPanel.scale.x;
        this->brushDisplayBtn.pos = glm::vec3(this->bgPanel.pos.x, this->bgPanel.pos.y - this->bgPanel.scale.y - this->brushDisplayBtn.scale.y, this->bgPanel.pos.z);
        this->brushDisplayBtn.render(timer, false);

        // Library selection panel
        this->libraryBrushSelectionButton.texture = appTextures.stripes1Icon;
        this->libraryBrushSelectionButton.textureSizeScale = 1.4f;
        this->libraryBrushSelectionButton.scale.x = (this->libraryBrushSelectionButton.scale.y / (Settings::videoScale()->x / Settings::videoScale()->y)) + this->libraryBrushSelectionButton.clickedMixVal * this->bgPanel.scale.x / 1.5f;
        this->libraryBrushSelectionButton.pos = glm::vec3(this->bgPanel.pos.x - this->bgPanel.scale.x - this->libraryBrushSelectionButton.scale.x, this->bgPanel.pos.y - this->bgPanel.scale.y + this->libraryBrushSelectionButton.scale.y, this->bgPanel.pos.z);
        this->libraryBrushSelectionButton.render(timer, !dialog_log.isHovered());
        
        if(*Mouse::LClick() && !this->libraryBrushSelectionPanel.hover && !this->libraryBrushSelectionButton.hover)
            this->libraryBrushSelectionButton.clickState1 = false;

        bool anyInteractions = false;
        
        this->libraryBrushSelectionPanel.scale.x = this->libraryBrushSelectionButton.clickedMixVal * ((this->libraryBrushSelectionButton.scale.y / (Settings::videoScale()->x / Settings::videoScale()->y)) + this->bgPanel.scale.x / 1.5f);
        this->libraryBrushSelectionPanel.scale.y = this->libraryBrushSelectionButton.clickedMixVal * (this->bgPanel.scale.y - this->libraryBrushSelectionButton.scale.y);
        this->libraryBrushSelectionPanel.pos = this->libraryBrushSelectionButton.pos;
        this->libraryBrushSelectionPanel.pos.y += this->libraryBrushSelectionButton.scale.y + this->libraryBrushSelectionPanel.scale.y;
        
        this->libraryBrushSelectionPanel.sections[0].elements.clear();
        for (size_t i = 0; i < Library::getBrushArraySize(); i++)
        {
            Button btn = Button(ELEMENT_STYLE_SOLID, glm::vec2(2,2), Library::getBrush(i)->title, Library::getBrush(i)->displayingTexture, 0.f, true);
            this->libraryBrushSelectionPanel.sections[0].elements.push_back(btn);
        }
        this->libraryBrushSelectionPanel.render(timer, !dialog_log.isHovered());
        for (size_t i = 0; i < Library::getBrushArraySize(); i++)
        {
            if(this->libraryBrushSelectionPanel.sections[0].elements[i].button.hover && *Mouse::LClick()){
                anyInteractions = true;
                *brushProperties = Library::getBrush(i)->properties;

                brushProperties->brushTexture.generateProceduralDisplayingTexture(brushTextureResolution, false);
                this->outToIn(brushProperties);
            }
        }
        
        for (size_t i = 0; i < this->bgPanel.sections[0].elements.size(); i++)
        {
            if(this->bgPanel.sections[0].elements[i].isInteracted()){

                // Set Default Properties button clicked 
                if(i == 9){
                    char whiteTxtr[brushTextureResolution * brushTextureResolution * 4];
                    for (size_t i = 0; i < brushTextureResolution * brushTextureResolution * 4; i++)
                    {
                        whiteTxtr[i] = 127;
                    }
                    
                    this->bgPanel.sections[0].elements[8].button.texture.update(whiteTxtr, brushTextureResolution, brushTextureResolution);
                    this->bgPanel.sections[0].elements[8].button.texture.proceduralProps = ProceduralProperties();
                    this->bgPanel.sections[0].elements[8].button.texture.proceduralProps.proceduralID = 24; //Solid white
                }

                if(i == 14){
                    this->applyDefaultBrushProperties(brushProperties);
                }
                if(i == 15){
                    this->cancelChanges(brushProperties);
                }
                
                anyInteractions = true;
            }
        }
        
        if(anyInteractions){
            this->inToOut(brushProperties);
            this->updateDisplayingTexture(brushProperties);
        }

        //End the dialog
        if((getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE)) || (!this->bgPanel.hover && !this->libraryBrushSelectionButton.hover && !this->libraryBrushSelectionPanel.hover && !this->brushDisplayBtn.hover && !dialog_log.isHovered() && *Mouse::LClick())){
            dialogControl.unActivate();
        }

        dialogControl.updateEnd(timer,0.15f);

        if(dialogControl.mixVal == 0.f)
            break;
    }

    Settings::defaultFramebuffer()->FBO.bind();
    Settings::defaultFramebuffer()->setViewport(); 
}





// ---------- UTILITY FUNCTIONS -----------


void BrushModificationDialog::updateDisplayingTexture(BrushProperties* brushProperties){
    if(!displayBrush.displayingTexture.ID)
        displayBrush = Brush(
                                    brushProperties->radius, 
                                    brushProperties->spacing,
                                    brushProperties->hardness,
                                    brushProperties->sizeJitter, 
                                    brushProperties->scatter,
                                    brushProperties->fade,
                                    brushProperties->rotation,
                                    brushProperties->rotationJitter,
                                    brushProperties->alphaJitter,
                                    brushProperties->individualTexture,
                                    brushProperties->sinWavePattern,
                                    "",
                                    brushProperties->brushTexture
                                );
/**/
    displayBrush.update(
                                brushProperties->radius, 
                                brushProperties->spacing,
                                brushProperties->hardness,
                                brushProperties->sizeJitter, 
                                brushProperties->scatter,
                                brushProperties->fade,
                                brushProperties->rotation,
                                brushProperties->rotationJitter,
                                brushProperties->alphaJitter,
                                brushProperties->individualTexture,
                                brushProperties->sinWavePattern,
                                brushProperties->brushTexture
                            );
    
    displayBrush.updateDisplayTexture(brushProperties->radius * 2.f);
}

void BrushModificationDialog::inToOut(BrushProperties* brushProperties){

    if(brushProperties->brushTexture.getResolution() != this->bgPanel.sections[0].elements[8].button.texture.getResolution())
        brushProperties->brushTexture.resize(this->bgPanel.sections[0].elements[8].button.texture.getResolution());

    brushProperties->radius = this->bgPanel.sections[0].elements[0].rangeBar.value;
    brushProperties->opacity = this->bgPanel.sections[0].elements[1].rangeBar.value;
    brushProperties->hardness = this->bgPanel.sections[0].elements[2].rangeBar.value;
    brushProperties->spacing = this->bgPanel.sections[0].elements[3].rangeBar.value;

    brushProperties->sizeJitter = this->bgPanel.sections[0].elements[4].rangeBar.value;
    brushProperties->fade = this->bgPanel.sections[0].elements[5].rangeBar.value;
    brushProperties->sinWavePattern = this->bgPanel.sections[0].elements[6].checkBox.clickState1;
    brushProperties->scatter = this->bgPanel.sections[0].elements[7].rangeBar.value;
    this->bgPanel.sections[0].elements[8].button.texture.duplicateTextureSub(brushProperties->brushTexture, "BrushModificationDialog::inToOut");

    brushProperties->individualTexture = this->bgPanel.sections[0].elements[10].checkBox.clickState1;
    brushProperties->rotation = this->bgPanel.sections[0].elements[11].rangeBar.value;
    brushProperties->rotationJitter = this->bgPanel.sections[0].elements[12].rangeBar.value;
    brushProperties->alphaJitter = this->bgPanel.sections[0].elements[13].rangeBar.value;
}

void BrushModificationDialog::outToIn(BrushProperties* brushProperties){
    
    if(brushProperties->brushTexture.getResolution() != this->bgPanel.sections[0].elements[8].button.texture.getResolution())
        brushProperties->brushTexture.resize(this->bgPanel.sections[0].elements[8].button.texture.getResolution());
    
    this->bgPanel.sections[0].elements[0].rangeBar.value = brushProperties->radius;
    this->bgPanel.sections[0].elements[1].rangeBar.value = brushProperties->opacity;
    this->bgPanel.sections[0].elements[2].rangeBar.value = brushProperties->hardness;
    this->bgPanel.sections[0].elements[3].rangeBar.value = brushProperties->spacing;

    this->bgPanel.sections[0].elements[4].rangeBar.value = brushProperties->sizeJitter;
    this->bgPanel.sections[0].elements[5].rangeBar.value = brushProperties->fade;
    this->bgPanel.sections[0].elements[6].checkBox.clickState1 = brushProperties->sinWavePattern;
    this->bgPanel.sections[0].elements[7].rangeBar.value = brushProperties->scatter;
    brushProperties->brushTexture.duplicateTextureSub(this->bgPanel.sections[0].elements[8].button.texture, "BrushModificationDialog::outToIn");
    
    this->bgPanel.sections[0].elements[10].checkBox.clickState1 = brushProperties->individualTexture;
    this->bgPanel.sections[0].elements[11].rangeBar.value = brushProperties->rotation;
    this->bgPanel.sections[0].elements[12].rangeBar.value = brushProperties->rotationJitter;
    this->bgPanel.sections[0].elements[13].rangeBar.value = brushProperties->alphaJitter;
}

void BrushModificationDialog::applyDefaultBrushProperties(BrushProperties* brushProperties){
    if(brushProperties->brushTexture.getResolution() != this->bgPanel.sections[0].elements[8].button.texture.getResolution())
        brushProperties->brushTexture.resize(this->bgPanel.sections[0].elements[8].button.texture.getResolution());

    brushProperties->radius = this->bgPanel.sections[0].elements[0].rangeBar.constructValue;
    brushProperties->opacity = this->bgPanel.sections[0].elements[1].rangeBar.constructValue;
    brushProperties->hardness = this->bgPanel.sections[0].elements[2].rangeBar.constructValue;
    brushProperties->spacing = this->bgPanel.sections[0].elements[3].rangeBar.constructValue;

    brushProperties->sizeJitter = this->bgPanel.sections[0].elements[4].rangeBar.constructValue;
    brushProperties->fade = this->bgPanel.sections[0].elements[5].rangeBar.constructValue;
    brushProperties->sinWavePattern = false;
    brushProperties->scatter = this->bgPanel.sections[0].elements[7].rangeBar.constructValue;

    brushProperties->brushTexture.proceduralProps = ProceduralProperties();
    brushProperties->brushTexture.proceduralProps.proceduralID = 24; //Solid white
    brushProperties->brushTexture.generateProceduralDisplayingTexture(brushTextureResolution, false);

    brushProperties->individualTexture = false;
    brushProperties->rotation = this->bgPanel.sections[0].elements[11].rangeBar.constructValue;
    brushProperties->rotationJitter = this->bgPanel.sections[0].elements[12].rangeBar.constructValue;
    brushProperties->alphaJitter = this->bgPanel.sections[0].elements[13].rangeBar.constructValue;
    
    this->outToIn(brushProperties);
}

void BrushModificationDialog::cancelChanges(BrushProperties* brushProperties){
    *brushProperties = initialProperties;
    brushProperties->brushTexture.generateProceduralDisplayingTexture(brushTextureResolution, false);

    this->outToIn(brushProperties);
}