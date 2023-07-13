/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Callback.cpp - Contains the callback functions

*/

#include <iostream>
#include <filesystem>

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include "../../thirdparty/include/glm/glm.hpp"
#include "../../thirdparty/include/glm/gtc/matrix_transform.hpp"
#include "../../thirdparty/include/glm/gtc/type_ptr.hpp"
#include "../../thirdparty/include/glm/gtx/string_cast.hpp"

#include "GUI/Elements.hpp"
#include "GUI/GUI.hpp"
#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"
#include "Renderer.h"

void Renderer::mouseButtonCallback(
                                        LigidWindow window, 
                                        int button, 
                                        int action, 
                                        int mods
                                    )
{
    //Calculate double click
    if (button == LIGIDGL_MOUSE_BUTTON_LEFT && action == LIGIDGL_PRESS) {
        double currentTime = LigidGL::getTime();
        double timeSinceLastClick = currentTime - previousClickTime;

        //Double click done if clicked in 0.3s
        if (timeSinceLastClick < 0.3) {
            this->mouse.LDoubleClick = true;
        }
    
        previousClickTime = currentTime;
    }
    
    //Left click
    if(button == 0){ 
        if(action == 1)
            this->mouse.LClick = true;
    }  
    //Right click
    if(button == 1){
        if(action == 1)
            this->mouse.RClick = true;
    }  
    //Mid click
    if(button == 2){
        if(action == 1)
            this->mouse.MClick = true;
    }  

    //Left pressed
    if(button == 0){ 
        this->mouse.LPressed = action;
    }  
    //Right pressed
    if(button == 1){
        this->mouse.RPressed = action;
    }  
    //Mid pressed
    if(button == 2){
        this->mouse.MPressed = action;
    }  
    
    //Mods
    this->mouse.mods = mods;
}

void Renderer::framebufferSizeCallback(
                                            LigidWindow window, 
                                            int width, 
                                            int height
                                        )
{
    //Update the window size from context
    this->context.windowScale.x = width;
    this->context.windowScale.y = height;

    //Update the perspective projection matrix to keep the ratio of the 3D model
    this->updateProjectionMatrix(); 

    //And ofc update the OpenGL viewport
    this->updateViewport(); 
}

void Renderer::scrollCallback(
                                LigidWindow window, 
                                double xoffset, 
                                double yoffset
                            )
{
    if(!this->userInterface.anyDialogActive && !this->userInterface.anyPanelHover )
    {
        //The distance between the camera & center 
        float originCameraDistance = glm::distance(this->scene.camera.originPos,this->scene.camera.cameraPos)/10;

        //Update the scroll value of the mouse class
        this->mouse.mouseScroll = yoffset;

        //Change the distance between camera & center (radius)
        if (yoffset > 0 && this->scene.camera.radius > 1) {
            this->scene.camera.radius -= originCameraDistance;
        }
        else if (yoffset < 0) {
            this->scene.camera.radius += originCameraDistance;
        }
        
        //Zoom in-out
        this->scene.camera.cameraPos.x = cos(glm::radians(this->scene.camera.yaw)) * cos(glm::radians(this->scene.camera.pitch)) * this->scene.camera.radius + this->scene.camera.originPos.x;
        this->scene.camera.cameraPos.y = sin(glm::radians(this->scene.camera.pitch)) * -this->scene.camera.radius + this->scene.camera.originPos.y;
        this->scene.camera.cameraPos.z = sin(glm::radians(this->scene.camera.yaw)) * cos(glm::radians(this->scene.camera.pitch)) * this->scene.camera.radius + this->scene.camera.originPos.z;

        //Update the view matrix after the camera position is changed
        this->updateViewMatrix();

        //Since the 3D model's position in the screen is changed update the painter's depth texture
        this->painter.updateTheDepthTexture = true;
    }
}

void Renderer::cursorPositionCallback(
                                        LigidWindow window,
                                        double xpos, //Mouse x position
                                        double ypos  //Mouse y position
                                    )
{
    //Update the mouse position of the mouse class
    this->mouse.cursorPos.x = xpos;
    this->mouse.cursorPos.y = ypos;
    
    //Get the mouse offset by subtracting current cursor position from last frame's cursor pos
    this->mouse.mouseOffset.x = this->mouse.cursorPos.x - lastMousePos.x;
    this->mouse.mouseOffset.y = this->mouse.cursorPos.y - lastMousePos.y;

    const float sensitivity = 0.2f; //Mouse sensivity (Increase the value to go brrrrrbrbrbrb) (effects the 3D model)

    if ((
            this->context.window.isMouseButtonPressed(LIGIDGL_MOUSE_BUTTON_RIGHT) == LIGIDGL_PRESS) && //If pressed to right mouse button
            window.isKeyPressed( LIGIDGL_KEY_LEFT_CONTROL) == LIGIDGL_PRESS &&  //If pressed to CTRL button
            !this->userInterface.anyDialogActive && //If there is no active dialog (don't move the camera if a dialog is active)
            !this->userInterface.anyPanelHover        //Don't move the camera if cursor hover a panel
        ) 
    { 
        // Straight Movement
      
        // Calculate half the sensitivity for later use
        const float half_sensitivity = sensitivity / 2.0f;

        // Calculate sine and cosine of yaw and pitch angles in radians
        const float sin_yaw = sin(glm::radians(this->scene.camera.yaw));
        const float cos_yaw = cos(glm::radians(this->scene.camera.yaw));
        const float sin_pitch = sin(glm::radians(this->scene.camera.pitch));
        const float cos_pitch = cos(glm::radians(this->scene.camera.pitch));

        // Calculate the x and z offsets based on yaw angle, mouse movement, sensitivity, and half sensitivity
        float x_offset = sin_yaw * this->mouse.mouseOffset.x * sensitivity * half_sensitivity;
        float z_offset = cos_yaw * this->mouse.mouseOffset.x * sensitivity * half_sensitivity;

        // Check if pitch is greater than 60 degrees or less than -60 degrees
        if (this->scene.camera.pitch > 60.0f || this->scene.camera.pitch < -60.0f) {
            
            // Add additional x and z offsets based on yaw, pitch, mouse movement, sensitivity, and half sensitivity
            x_offset += cos_yaw * sin_pitch * this->mouse.mouseOffset.y * sensitivity * half_sensitivity;
            z_offset -= sin_yaw * sin_pitch * this->mouse.mouseOffset.y * sensitivity * half_sensitivity;
        }

        // Calculate the y offset based on pitch, mouse movement, sensitivity, and half sensitivity
        const float y_offset = cos_pitch * this->mouse.mouseOffset.y * sensitivity * half_sensitivity;

        // Update camera's x position and origin position by subtracting x offset
        this->scene.camera.cameraPos.x -= x_offset;
        this->scene.camera.originPos.x -= x_offset;

        // Update camera's z position and origin position by adding z offset
        this->scene.camera.cameraPos.z += z_offset;
        this->scene.camera.originPos.z += z_offset;

        // Update camera's y position and origin position by adding y offset
        this->scene.camera.cameraPos.y += y_offset;
        this->scene.camera.originPos.y += y_offset;

        //Since the 3D model's position in the screen is changed update the painter's depth texture
        this->painter.updateTheDepthTexture = true;
    }


    else if (
                this->context.window.isMouseButtonPressed(LIGIDGL_MOUSE_BUTTON_RIGHT) == LIGIDGL_PRESS && //If pressed to right mouse button
                !this->userInterface.anyDialogActive && //If there is no active dialog (don't move the camera if a dialog is active)
                !this->userInterface.anyPanelHover  //Don't move the camera if cursor hover a panel
            ) 
    { 
        
        this->scene.camera.yaw += this->mouse.mouseOffset.x * sensitivity;
        this->scene.camera.pitch -= this->mouse.mouseOffset.y * sensitivity;

        //Disable 90+ degrees rotations in y axis
        if (this->scene.camera.pitch > 89.0f)
            this->scene.camera.pitch = 89.0f;
        if (this->scene.camera.pitch < -89.0f)
            this->scene.camera.pitch = -89.0f;

        //Helical Movement
        //Rotates the Camera in 3 axis using yaw, pitch & radius values
        this->scene.camera.cameraPos.y = sin(glm::radians(this->scene.camera.pitch)) * -this->scene.camera.radius + this->scene.camera.originPos.y;
        this->scene.camera.cameraPos.x = cos(glm::radians(this->scene.camera.yaw)) * cos(glm::radians(this->scene.camera.pitch)) * this->scene.camera.radius + this->scene.camera.originPos.x;
        this->scene.camera.cameraPos.z = sin(glm::radians(this->scene.camera.yaw)) * cos(glm::radians(this->scene.camera.pitch)) * this->scene.camera.radius + this->scene.camera.originPos.z;
        
        //Since the 3D model's position in the screen is changed update the painter's depth texture
        this->painter.updateTheDepthTexture = true;
    }


    //Update the view matrix after the camera position is changed
    this->updateViewMatrix();
    
    //This will be used as "last frame's cursor pos" for the cursor offset
    this->lastMousePos.x = this->mouse.cursorPos.x;
    this->lastMousePos.y = this->mouse.cursorPos.y;
}

void Renderer::keyCallback(
                                LigidWindow window, //Window
                                int key, //Pressed key
                                int scancode, //Scancode (related to the keyboard I guess)
                                int action, //LIGIDGL_PRESS , LIGIDGL_REPEAT , LIGIDGL_RELEASE
                                int mods //Pressed with CTRL, CTRL + ALT , ALT , etc.
                            )
{
    if(action == LIGIDGL_PRESS || action == LIGIDGL_REPEAT){ //1 or 2
        this->textRenderer.keyInput = true;
        this->textRenderer.key = key;
        this->textRenderer.mods = mods;
    }
}