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

#ifndef LGDUIBUTTON_HPP
#define LGDUIBUTTON_HPP

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Shader.hpp"
#include "Box.hpp"
#include "Renderer.h"
#include "Util.hpp"
#include "Timer.hpp"

#include "GUI/Panel.hpp"
#include "GUI/Button.hpp"
#include "Mouse.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>

class Button
{
private:
    void render(glm::vec3 resultPos,glm::vec2 resultScale){
        
        //Set the transform data (used by vertex shader)
        shader.setVec3("pos"    ,     resultPos );
        shader.setVec2("scale"  ,     resultScale);
        
        if(clickState1)//If button is pressed
            shader.setVec4("color"  ,     color * glm::vec4(2.f,2.f,2.f,1.f)     ); //Button pressing color
        else
            shader.setVec4("color"  ,     color     ); //Default button color
        
        shader.setVec4("color2"  ,     color2     ); //Second color that is used by hover or click animations
        
        if(animationStyle == 1) //If hover or clicked change the color of the button
            shader.setFloat("colorMixVal"  ,     (clickedMixVal + hoverMixVal)/2.f   );
        else //If clicked change the color of the button
            shader.setFloat("colorMixVal"  ,     (clickedMixVal)   );

        //Set the resolution of the button (used by fragment shader)
        shader.setFloat("width" ,     scale.x   );
        shader.setFloat("height",     scale.y   );

        //Properties
        shader.setFloat("radius",     radius    );
        shader.setInt("outline" ,     outline      );

        if(animationStyle == 0) //Increase the thicness of the button if hover
            shader.setFloat("thickness" ,    100.f + hoverMixVal*100.f );
        else  //Set the thickness value of the button
            shader.setFloat("thickness" ,    100.f);
        
        glDrawArrays(GL_TRIANGLES, 0, 6);
    }
public:
    //For now buttons can only be usable with panels

    Shader shader;

    bool clickState1 = false;


    //---Button properties
    std::string text; //Text of the button

    glm::vec4 color; //Original color
    glm::vec4 color2; //Hover or clicked transition color
    glm::vec4 textColor; //Original color of the text
    glm::vec4 textColor2; //Hover or clicked transition color of the text

    bool outline; //Whether will only has outlines or be solid
    float radius; //Radius of the corners
    int animationStyle; //determines what type of mouse hover or click animation will be used
    //0 = Change thickness for mousehover
    //1 = Change color for mousehover

    //Those values are adepting to the panel if button is attached to one
    //Values are percentage of the monitor size
    glm::vec2 scale;  //Example : w : 20, h : 30 means cover %20 of the window in x axis & cover 30% of the window in y axis
    glm::vec3 pos; //Same here
    
    bool hover = false;

    float hoverMixVal = 0.f;
    float clickedMixVal = 0.f;

    Button(){}
    Button(Shader shader, glm::vec2 scale, glm::vec4 color, glm::vec4 color2, bool outline, float radius, int animationStyle,glm::vec4 textColor,glm::vec4 textColor2){
        //animationStyle determines what type of mouse hover or click animation will be used
        //0 = Change thickness for mousehover
        //1 = Change color for mousehover

        this->shader = shader;
        this->color = color;
        this->color2 = color2;
        this->scale = scale;
        this->outline = outline;
        this->radius = radius;
        this->animationStyle = animationStyle;
        this->textColor = textColor;
        this->textColor2 = textColor2;
    }

    void render(glm::vec2 videoScale,Mouse& mouse, Timer &timer,TextRenderer &textRenderer){
        Util util;

        // pos value % of the video scale
        glm::vec3 resultPos = glm::vec3( 
                              util.getPercent(videoScale,glm::vec2(pos.x,pos.y)) //Don't include the depth
                              ,pos.z); //Use the original depth value

        // scale value % of the video scale
        glm::vec2 resultScale = util.getPercent(videoScale,scale);
        
        // Real size of the text
        float resultScaleText = util.getPercent(videoScale.x,20);

        //Check if mouse on top of the button
        hover = mouse.isMouseHover(resultScale,glm::vec2(resultPos.x,resultPos.y));

        if(hover && mouse.LClick){
            //Mouse left button pressed on top of the button
            //(if release on top of the button, the button will be clicked)
            clickState1 = true;
        }
        if(!mouse.LPressed){
            //If clicked to the button
            if(clickState1 && hover){
                //Clicked
                clickedMixVal = 1.f;
            }
            clickState1 = false;
        }

        timer.transition(hover,hoverMixVal,0.2f); 
        timer.transition(false,clickedMixVal,0.5f); 
        
        //Render the button
        render(resultPos,resultScale);

        //Render the text
        shader.setVec4("color"  ,     textColor     );
        shader.setVec4("color2"  ,     textColor2     );
        textRenderer.renderText(shader,"ASDFAS ASDFASFASDF ASDF ASDDAF SAFSDAF SDAF",resultPos.x - resultScale.x,resultPos.y,1,resultPos.x + resultScale.x,false,resultScaleText/videoScale.x);
    }
};
#endif