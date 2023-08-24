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
#include "ColorPaletteSystem/ColorPalette.hpp"
#include "MouseSystem/Mouse.hpp"

#include <string>
#include <iostream>
#include <vector>

ContextMenu::ContextMenu(){}

ContextMenu::ContextMenu(std::vector<std::string> elements){
    
    //Create the context panel
    contextPanel = Panel(
        {
            //Sections
        },
        glm::vec2(5.f,5.f), //Scale
        glm::vec3(50.f,50.f,0.95f), //Pos
        ColorPalette::mainColor,
        ColorPalette::secondColor,
        true,
        true,
        true,
        true,
        true,
        1.f,
        1,
        {},
        20.f,
        false
    );

    //Push the elements taken from parameter into the context panel
    Section section;
    for (size_t i = 0; i < elements.size(); i++)
    {
        section.elements.push_back(Element(Button(ELEMENT_STYLE_SOLID, glm::vec2(1,1), elements[i], Texture(), 0.f, false)));
    }
    
    contextPanel.sections.push_back(section);
    contextPanel.scale.y = elements.size();
}

void ContextMenu::render(Timer &timer){
    dialogControl.updateStart();
    
    //Update the position of the context panel
    contextPanel.pos = pos;

    //Render the context panel
    contextPanel.render(timer,true);
    
    dialogControl.updateEnd(timer, 0.15f);

    if (   //Conditions to turn any context menu off
        *Mouse::LClick() || //Mouse left cli                       2.ck
        *Mouse::RClick() || //Mouse right click
        *Mouse::MClick() || //Mouse middle click11
        *Mouse::mouseScroll() || //Mouse middle click
        getContext()->window.isKeyPressed(LIGIDGL_KEY_ESCAPE) == LIGIDGL_PRESS || //Pressed to escape key 
        getContext()->window.isKeyPressed(LIGIDGL_KEY_ENTER) == LIGIDGL_PRESS || //Pressed to enter key
        !this->contextPanel.hover
    )
    {
        this->dialogControl.unActivate(); //Turn the context menu offs
    }
}