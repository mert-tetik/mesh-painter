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

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/string_cast.hpp>

#include "GUI/Elements.hpp"

#include <string>
#include <iostream>
#include <vector>

void Button::render(glm::vec3 resultPos,glm::vec2 resultScale,float resultRadius,float resultOutlineThickness){
    
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
    else if(animationStyle == 2) //If hover or clicked change the color of the button
        shader.setFloat("colorMixVal"  ,     (hover/2.f)   );
    else //If clicked change the color of the button
        shader.setFloat("colorMixVal"  ,     (clickedMixVal)   );
    //Properties
    shader.setFloat("radius",     resultRadius    );
    
    if(outline)
        shader.setInt("outlineState" ,     1      ); 
    else if(outlineExtra)
        shader.setInt("outlineState" ,     2      ); 
    else
        shader.setInt("outlineState" ,    0      ); 
    
    //Outline extra color (affected by the colorMixVal)
    shader.setVec3("outlineColor" ,     outlineColor     );  
    shader.setVec3("outlineColor2" ,     outlineColor2     );   
    if(animationStyle == 0) //Increase the thicness of the button if hover
        shader.setFloat("thickness" ,    resultOutlineThickness + hoverMixVal*2.f ); 
    else if(animationStyle == 2)
        shader.setFloat("thickness" ,    resultOutlineThickness + clickedMixVal*2.f ); 
    else  //Set the thickness value of the button
        shader.setFloat("thickness" ,    resultOutlineThickness); 
    
    glDrawArrays(GL_TRIANGLES, 0, 6);
}

bool Button::renderTheTexture(glm::vec3 resultPos,glm::vec2 resultScale,float resultScaleText,glm::vec2 videoScale,TextRenderer &textRenderer,float &textureRadius){
    //returns the renderthetext
    //If returns yes render the text
    //If not don't render the text
        
    //Get the position data of the text before rendering
    //The texture's position will be interpreted by the text's position data
    shader.setVec4("color"  ,     glm::vec4(0)     );
    shader.setVec4("color2"  ,     glm::vec4(0)     );
    //Update the parameters if original renderer's parameters are changed
    glm::vec3 textPosData;
    if(textAlignLeft){
        textPosData = textRenderer.renderText(shader,text,resultPos.x-resultScale.x,resultPos.y,-10000.1,resultPos.x + resultScale.x,false,resultScaleText,resultPos.x-resultScale.x,true,false);
    }
    else{
        textPosData = textRenderer.renderText(shader,text,resultPos.x,resultPos.y,-10000.1,resultPos.x + resultScale.x,false,resultScaleText,resultPos.x-resultScale.x,false,false);
    }
    //textPosData.x is where the text is started in x axis
    //textPosData.y is where the text is ended in x axis
    //textPosData.z is boolean value if text is reached to the boundaries
    //Render the texture
    bool renderTheText = true;
    if(texture.ID != 0){
        //Scale of the texture
        glm::vec2 txtrResDivider = glm::vec2(1);
       
        if(texture.getResolution().x > texture.getResolution().y){
            txtrResDivider.y = (float)texture.getResolution().x / (float)texture.getResolution().y;
        }
        else{
            txtrResDivider.x = (float)texture.getResolution().y / (float)texture.getResolution().x;
        }


        glm::vec2 resultScaleTexture = glm::vec2(std::min(resultScale.x/buttonImageScaleDivider,resultScale.y/buttonImageScaleDivider)) / glm::vec2(textureSizeScale);
        resultScaleTexture /= txtrResDivider;
        
        textureRadius = resultScaleTexture.x;
        //Position of the texture
        glm::vec3 resultPosTexture = resultPos;
        
        if(textureStickToTop)
            resultPosTexture.y = resultPosTexture.y - resultScale.y + resultScaleTexture.y;
        
        if(text == ""){
            //If there is no text get the texture into the middle
            resultPosTexture.x = textPosData.x; 
        }
        else{
            //If there is text get the texture left side of the button
            resultPosTexture.x = resultPos.x - resultScale.x + resultScaleTexture.x * 2.f; 
        }
        //If the button is small enough get the texture into middle and don't render the text
        if(textPosData.x - resultScaleTexture.x * 4.f < resultPos.x - resultScale.x){
            resultPosTexture.x = resultPos.x; //Button's position
            renderTheText = false;
        }
        resultPosTexture.z += 0.02f; //Take texture to the front of the button
        //Tell the button shader to render the texture
        shader.setInt("renderTexture"  ,     1    );
        shader.setInt("txtr"  ,     0    );
        //Set transform data of the texture
        shader.setVec3("pos"    ,     resultPosTexture);
        shader.setVec2("scale"  ,     resultScaleTexture);
        //Bind the texture
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D,texture.ID);
        //Draw the texture
        glDrawArrays(GL_TRIANGLES, 0, 6);
        //Prevent unrelated texture rendering
        shader.setInt("renderTexture"  ,     0    );
    }
    return renderTheText;
}

Button::Button(){}

//Manual constructor
Button::Button(Shader shader,std::string text, glm::vec2 scale, glm::vec4 color, glm::vec4 color2, bool outline, float radius, int animationStyle,glm::vec4 textColor,glm::vec4 textColor2,
       Texture texture,float textScale,float panelOffset,bool outlineExtra,glm::vec3 outlineColor,glm::vec3 outlineColor2,float outlineThickness,bool keepPressingState){
    
    //animationStyle determines what type of mouse hover or click animation will be used
    //0 = Change thickness for mousehover
    //1 = Change color for mousehover
    //2 = Full outline

    this->shader = shader;
    this->text = text;
    this->color = color;
    this->color2 = color2;
    this->scale = scale;
    this->outline = outline;
    this->radius = radius;
    this->animationStyle = animationStyle;
    this->textColor = textColor;
    this->textColor2 = textColor2;
    this->texture = texture;
    this->textScale = textScale;
    this->panelOffset = panelOffset;
    this->outlineExtra = outlineExtra;
    this->outlineColor = outlineColor;
    this->outlineColor2 = outlineColor2;
    this->outlineThickness = outlineThickness;
    this->keepPressingState = keepPressingState;
}

//Style constructor
Button::Button(int style,glm::vec2 scale,ColorPalette colorPalette,Shader shader,std::string text,Texture texture,float panelOffset,bool keepPressingState){
    //Style 0 = Stylized
    //style 1 = Solid
    //style 2 = both
    
    this->shader = shader;
    this->text = text;
    this->scale = scale;
    this->texture = texture;
    this->panelOffset = panelOffset;
    this->keepPressingState = keepPressingState;
    
    if(style == 0){
        this->color = colorPalette.oppositeColor;
        this->color2 = colorPalette.themeColor;
        this->outline = true;
        this->radius = 0.5f;
        this->animationStyle = 0;
        this->textColor = colorPalette.oppositeColor;
        this->textColor2 = colorPalette.themeColor;
        this->textScale = 0.5f;
        this->outlineExtra = false;
        this->outlineColor = {};
        this->outlineColor2 = {};
        this->outlineThickness = 2.f;
    }
    if(style == 1){
        this->color = colorPalette.secondColor;
        this->color2 = colorPalette.themeColor;
        this->outline = false;
        this->radius = 0.1f;
        this->animationStyle = 1;
        this->textColor = colorPalette.oppositeColor;
        this->textColor2 = colorPalette.thirdColor;
        this->textScale = 0.5f;
        this->outlineExtra = true;
        this->outlineColor = colorPalette.oppositeColor;
        this->outlineColor2 = colorPalette.thirdColor;
        this->outlineThickness = 1.f;
        this->buttonImageScaleDivider = 1.f;
    }
    if(style == 2){
        this->color = colorPalette.secondColor;
        this->color2 = colorPalette.themeColor;
        this->outline = false;
        this->radius = 0.25f;
        this->animationStyle = 1;
        this->textColor = colorPalette.oppositeColor;
        this->textColor2 = colorPalette.thirdColor;
        this->textScale = 0.5f;
        this->outlineExtra = true;
        this->outlineColor = colorPalette.oppositeColor;
        this->outlineColor2 = colorPalette.thirdColor;
        this->outlineThickness = 2.f;
        this->buttonImageScaleDivider = 2.f;
    }


}



void Button::render(glm::vec2 videoScale,Mouse& mouse, Timer &timer,TextRenderer &textRenderer,bool doMouseTracking){
    Util util;
    this->doMouseTracking = doMouseTracking;

    resultPos = glm::vec3( 
                          util.getPercent(videoScale,glm::vec2(pos.x,pos.y)) //Don't include the depth
                          ,pos.z); //Use the original depth value

    resultScale = util.getPercent(videoScale,scale);
    
    float resultRadius = util.getPercent(videoScale.x,radius);
    
    float resultScaleText = videoScale.x/1920/2*textScale;
    
    float resultOutlineThickness = videoScale.x/1920/2*outlineThickness;

    //Check if mouse on top of the button
    if(doMouseTracking)
        hover = mouse.isMouseHover(resultScale,glm::vec2(resultPos.x,resultPos.y));
    else
        hover = false;

    if(hover)//Set the cursor as pointer if cursor on button
        mouse.setCursor(mouse.pointerCursor);// mouse.activeCursor = mouse.pointerCursor
    
    if(hover && mouse.LClick){
        //Mouse left button pressed on top of the button
        //(if release on top of the button, the button will be clicked)
        if(keepPressingState)
            clickState1 = !clickState1;
        else
            clickState1 = true;
    }

    timer.transition(hover,hoverMixVal,0.2f); 
    
    if(keepPressingState)
        timer.transition(clickState1,clickedMixVal,0.5f); 
    else
        timer.transition(false,clickedMixVal,0.5f); 
    
    if(!mouse.LPressed && !keepPressingState){
        //If clicked to the button
        if(clickState1 && hover){
            //Clicked
            clickedMixVal = 1.f;
        }
        clickState1 = false;
    }
    
    //Render the button
    render(resultPos,resultScale,resultRadius,resultOutlineThickness);
    float textureRadius = 0.f;
    bool renderTheText = renderTheTexture(resultPos,resultScale,resultScaleText,videoScale,textRenderer,textureRadius);
    //Render the text
    if(renderTheText){
        shader.setVec4("color"  ,     textColor     );
        shader.setVec4("color2"  ,     textColor2     );
        //Update the parameters of the renderText function in the renderTheTexture function if this function's parameters are changed
        if(textAlignLeft)
            textRenderer.renderText(shader,text,resultPos.x+textureRadius - resultScale.x ,resultPos.y,resultPos.z+0.002f,resultPos.x + resultScale.x ,false,resultScaleText,resultPos.x-resultScale.x,true,false);
        else
            textRenderer.renderText(shader,text,resultPos.x+textureRadius ,resultPos.y,resultPos.z+0.002f,resultPos.x + resultScale.x ,false,resultScaleText,resultPos.x-resultScale.x,false,false);
    }
}