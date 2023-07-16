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

#include "GUI/Elements/Elements.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

ComboBox::ComboBox(){}

//Manual constructor
ComboBox::ComboBox(Shader shader,std::vector<std::string> texts, glm::vec2 scale, glm::vec4 color, glm::vec4 color2,glm::vec4 textColor,glm::vec4 textColor2,
        float textScale,float panelOffset,glm::vec3 outlineColor,glm::vec3 outlineColor2){
    
    this->shader = shader;
    this->texts = texts;
    this->color = color;
    this->color2 = color2;
    this->scale = scale;
    this->textColor = textColor;
    this->textColor2 = textColor2;
    this->textScale = textScale;
    this->panelOffset = panelOffset;
    this->outlineColor = outlineColor;
    this->outlineColor2 = outlineColor2;

    for (size_t i = 0; i < texts.size(); i++)
    {
        hover.push_back(0);
        hoverMixVal.push_back(0);
        clickedMixVal.push_back(0);
    }
}

//Style constructor
ComboBox::ComboBox(int style,glm::vec2 scale,ColorPalette colorPalette,Shader shader,std::vector<std::string> texts,std::string text,float panelOffset){
    this->shader = shader;
    this->texts = texts;
    this->text = text;
    this->scale = scale;
    this->panelOffset = panelOffset;
    
    if(style == ELEMENT_STYLE_BASIC){
        this->bgColor = colorPalette.mainColor;
        this->color = colorPalette.oppositeColor;
        this->color2 = colorPalette.themeColor;
        this->textColor = colorPalette.oppositeColor;
        this->textColor2 = colorPalette.themeColor;
        this->textScale = 0.5f;
        this->outlineColor = {};
        this->outlineColor2 = {};
    }

    //Init the arrays regarding the elements
    for (size_t i = 0; i < texts.size(); i++)
    {
        hover.push_back(0);
        hoverMixVal.push_back(0);
        clickedMixVal.push_back(0);
    }
}