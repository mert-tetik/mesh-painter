
/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

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
#include "UTIL/Mouse/Mouse.hpp" 
#include "UTIL/Library/Library.hpp" 
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp" 

#include <string>
#include <iostream>
#include <vector>
#include <filesystem>

void LogDialog::render_elements(Timer& timer)
{
    // Move to the center
    if(getContext()->window.shouldClose()){
        glm::vec2 center = glm::vec2(50.);
        pos -= (pos - center) / 10.f;
    }

    if(*Mouse::LClick()){
        if(logBtn.hover){
            actionHistoryActive = false;
            messagesActive = false;
        }
        
        else if(logBtnL.hover && !messageInfoActive){
            messagesActive = !messagesActive;
        }
        
        else if(logBtnR.hover){
            actionHistoryActive = !actionHistoryActive;
        }

    }

    if(logBtn.clickState1){ 
        pos.x += Mouse::mouseOffset()->x / Settings::videoScale()->x * 100.f;
        pos.y += Mouse::mouseOffset()->y / Settings::videoScale()->y * 100.f;
    }

    logBtn.pos.x = pos.x;
    logBtn.pos.y = pos.y;
    
    logBtnL.pos.x = pos.x - logBtn.scale.x - logBtnL.scale.x / 4.f;
    if(messagesActive)
        logBtnL.pos.x -= messagesPanel.scale.x * 1.85f;
    else
        logBtnL.pos.x -= messageInfoBtn.scale.x * 1.9f;
    
    logBtnL.pos.y = pos.y + std::sin(LigidGL::getTime() * 2.f + 1.f) / 4.f;

    logBtnR.pos.x = pos.x + logBtn.scale.x + logBtnR.scale.x / 4.f;
    if(actionHistoryActive)
        logBtnR.pos.x += historyPanel.scale.x * 1.85f;
    logBtnR.pos.y = pos.y + std::sin(LigidGL::getTime() * 2.f) / 4.f;

    timer.transition(messagesActive, messagesPanelXAxisMixVal, 0.1f);
    timer.transition(messagesActive, messagesPanelYAxisMixVal, 0.4f);
    
    timer.transition(actionHistoryActive, historyPanelXAxisMixVal, 0.1f);
    timer.transition(actionHistoryActive, historyPanelYAxisMixVal, 0.4f);
    
    messagesPanel.scale.x = messagesPanelXAxisMixVal * 10.f;
    messagesPanel.scale.y = messagesPanelYAxisMixVal * 10.f;
    historyPanel.scale.x = historyPanelXAxisMixVal * 10.f;
    historyPanel.scale.y = historyPanelYAxisMixVal * 10.f;
    
    messagesPanel.pos.x = pos.x - messagesPanel.scale.x;
        
    historyPanel.pos.x = pos.x + historyPanel.scale.x;

    messagesPanel.pos.y = pos.y + messagesPanel.scale.y - messagesPanelXAxisMixVal * 2.f;
    
    historyPanel.pos.y = pos.y + historyPanel.scale.y - historyPanelXAxisMixVal * 2.f;

    messageInfoBtn.scale.x = messageInfoBtnMixVal * (messageInfoBtn.text.size() / 4.5f);
    messageInfoBtn.pos.x = pos.x - messageInfoBtn.scale.x;
    messageInfoBtn.pos.y = pos.y;

    if(getContext()->window.shouldClose()){
        yesBtn.pos = messageInfoBtn.pos;
        yesBtn.pos.y += messageInfoBtn.scale.y + yesBtn.scale.y;
        yesBtn.scale.x = messageInfoBtn.scale.x / 4.f;
       
        noBtn.scale = yesBtn.scale;
        noBtn.pos = yesBtn.pos;
        
        yesBtn.pos.x -= noBtn.scale.x;
        noBtn.pos.x += yesBtn.scale.x;

        yesBtn.render(timer, true);
        noBtn.render(timer, true);
    }
    else{
        yesBtn.hover = false;
        noBtn.hover = false;
    }

    libraryHistoryBtn.pos = historyPanel.pos;
    libraryHistoryBtn.pos.y += historyPanel.scale.y + libraryHistoryBtn.scale.y;
    libraryHistoryBtn.scale.x = historyPanel.scale.x / 2.1f;
    
    otherHistoryBtn.scale = libraryHistoryBtn.scale;
    otherHistoryBtn.pos = libraryHistoryBtn.pos;
    
    libraryHistoryBtn.pos.x -= otherHistoryBtn.scale.x;
    otherHistoryBtn.pos.x += libraryHistoryBtn.scale.x;

    if(historyPanel.scale.x > 0.1f){
        libraryHistoryBtn.render(timer, true);
        otherHistoryBtn.render(timer, true);
    }
    else{
        libraryHistoryBtn.hover = false;
        otherHistoryBtn.hover = false;
    }

    if(messagesActive)
        messagesPanel.render(timer, true);
    else
        messagesPanel.hover = false;
    
    if(actionHistoryActive)
        historyPanel.render(timer, true);
    else
        historyPanel.hover = false;
    ShaderSystem::buttonShader().setFloat("rotation", std::sin(LigidGL::getTime() * 2.f) * 10.f * (float)!sleepingCat + (std::sin(logBtn.clickedMixVal) * 360.f));
    logBtn.render(timer, true);
    ShaderSystem::buttonShader().setFloat("rotation", 0.f);
    logBtnL.render(timer, true);
    logBtnR.render(timer, true);
    messageInfoBtn.render(timer, false);

    if(dizzyCounter)
        logBtn.texture = appTextures.mascotCat_dizzy;
    //else if(cryCounter)
    //    logBtn.texture = appTextures.mascotCat_crying;
    //else if(sleepingCat)
    //    logBtn.texture = appTextures.mascotCat_sleeping;
    else if(messageInfoActive && !painter.refreshable)
        logBtn.texture = msgFace;
    else if((logBtn.hover) || painter.refreshable)
        logBtn.texture = appTextures.mascotCat_smile;
    else if(messagesActive || actionHistoryActive)
        logBtn.texture = appTextures.mascotCat_relaxed;
    else
        logBtn.texture = appTextures.mascotCat_default;
}