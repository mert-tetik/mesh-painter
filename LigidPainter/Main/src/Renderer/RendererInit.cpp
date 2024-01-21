/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Init.cpp - Contains the constructor of the class Renderer - from Renderer.h

*/

#include <iostream>
#include <filesystem>
#include <thread>
#include <atomic>
#include <mutex>

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include "../../thirdparty/include/glm/glm.hpp"
#include "../../thirdparty/include/glm/gtc/matrix_transform.hpp"
#include "../../thirdparty/include/glm/gtc/type_ptr.hpp"
#include "../../thirdparty/include/glm/gtx/string_cast.hpp"

#include "GUI/Elements/Elements.hpp"
#include "GUI/GUI.hpp"
#include "UTIL/Util.hpp"
#include "ShaderSystem/Shader.hpp"
#include "NodeSystem/Node/Node.hpp"
#include "SettingsSystem/Settings.hpp"
#include "MouseSystem/Mouse.hpp"
#include "LibrarySystem/Library.hpp"
#include "3D/ThreeD.hpp"
#include "Renderer.h"

#include "LigidGL/LigidGL.hpp"

#include "tinyfiledialogs.h"


void Renderer::initRenderer(){
    if(!getContext()->window.setWindowIcon(L"./LigidPainter/Resources/Icons/logo-1080x.ico"))
        LGDLOG::start<< "ERROR : Can't change the icon of the window" << LGDLOG::end;

    Debugger::block("LOAD : Init context"); //Start
    getContext()->window.setWindowUserPointer(this);

    auto cursorPosFunc = [](LigidWindow w, double x, double y)
    {
        Renderer* renderer = static_cast<Renderer*>(w.getWindowUserPointer());
        if (renderer)
        {
            renderer->cursorPositionCallback(w, x, y);
        }
    };
    
    getContext()->window.setMousePosCallback(cursorPosFunc);
    
    //Scroll callback function casting
    auto scrollFunc = [](LigidWindow w, double x, double y)
    {
        static_cast<Renderer*>(w.getWindowUserPointer())->scrollCallback(w,x,y);
    };
    getContext()->window.setScrollCallback(scrollFunc);

    //Framebuffer size callback function casting
    auto framebufferSizeFunc = [](LigidWindow w, int x, int y)
    {
        static_cast<Renderer*>(w.getWindowUserPointer())->framebufferSizeCallback(w,x,y);
    };
    getContext()->window.setWindowSizeCallback(framebufferSizeFunc);

    //Mouse button callback function casting
    auto mouseButtonFunc = [](LigidWindow w, int button, int action, int mods)
    {
        static_cast<Renderer*>(w.getWindowUserPointer())->mouseButtonCallback(w,button,action,mods);
    };

    getContext()->window.setMouseButtonCallback(mouseButtonFunc);

    //Key button callback function casting
    auto keyFunc = [](LigidWindow w, int key, int action, int mods)
    {
        static_cast<Renderer*>(w.getWindowUserPointer())->keyCallback(w, key, 0, action, mods);
    };

    getContext()->window.setKeyCallback(keyFunc);

    //Get the window size
    getContext()->windowScale = *Settings::videoScale();

    getScene()->tiles.init();
    getScene()->axisDisplayer.init();

    //Update necessary data before callbacks
    glViewport(0, 0, getContext()->windowScale.x, getContext()->windowScale.y);    
    getScene()->updateViewMatrix();
    getScene()->updateTransformMatrix();
    getScene()->updateProjectionMatrix();
    Debugger::block("LOAD : Init context"); //End

    Debugger::block("LOAD : Models"); //Start
    
    //Load the 3D models 345235456
    getModel()->loadModel("./LigidPainter/Resources/3D Models/sphere.fbx", true, false);
    getSphereModel()->loadModel("./LigidPainter/Resources/3D Models/sphere.fbx",true, true);
    getPlaneModel()->loadModel("./LigidPainter/Resources/3D Models/plane.fbx",true, true);
    getMaterialDisplayerModel()->loadModel("./LigidPainter/Resources/3D Models/MaterialDisplayer.obj", true, true);
    
    Debugger::block("LOAD : Models"); //End

    //Create 2D square vertex buffers
    getBox()->init();
    
    Debugger::block("LOAD : Skybox"); //Start 305233920 291.12 MB
    
    //Init the skybox    
    skybox.init();
    //Load the default skybox
    skybox.load("./LigidPainter/Resources/Cubemap/Skybox/sky6"); //Skybox's itself
    skybox.createPrefilterMap(); //Create prefiltered skybox
    skybox.createDisplayingTxtr(); //Create displaying texture
    
    Debugger::block("LOAD : Skybox"); //End 
    
    Debugger::block("LOAD : Font"); //Start 2654208
    
    //Load the fonts
    fonts.Arial.loadFont("./LigidPainter/Resources/Fonts/Arial.ttf");
    
    Debugger::block("LOAD : Font"); //End

    //Init the text renderer
    textRenderer = TextRenderer(fonts.Arial);
    
    //Init websites
    websites.ligidTools     =   Website("https://ligidtools.com/");
    websites.youTube        =   Website("https://www.youtube.com/channel/UCMVLfsYsd5WAKEWsgM7fjtA");

    Debugger::block("LOAD : GUI"); //Start 143695872 137 MB
    
    //Init the userinterface
    userInterface.init(websites, painter);
    
    Debugger::block("LOAD : GUI"); //End

    Debugger::block("LOAD : Cursors"); //Start
    //Load the cursors of the LigidPainter
    Mouse::loadCursors();
    Debugger::block("LOAD : Cursors"); //End

    Debugger::block("LOAD : Init painter"); //Start
    //Init the painter
    painter.initPainter();
    Debugger::block("LOAD : Init painter"); //End

    Debugger::block("LOAD : Rest"); //Start
    
    //Create the mesh node
    NodeScene::addNode(Node(MESH_NODE, 0));

    //Load the inputs of the mesh node
    NodeScene::getNode(0)->uploadNewIOs();

    try
    {
        //Create the LigidPainter folder in the app data folder if not exists
        if(!std::filesystem::exists(UTIL::environmentSpecificAppDataFolderPath() + "LigidPainter")){
            if(!std::filesystem::create_directory(UTIL::environmentSpecificAppDataFolderPath() + "LigidPainter"))
                LGDLOG::start<< "ERROR : Creating LigidPainter folder in the appdata." << LGDLOG::end;
        }
        
        //Create the projects folder if not exists
        if(!std::filesystem::exists(UTIL::environmentSpecificAppDataFolderPath() + "LigidPainter" + UTIL::folderDistinguisher() + "Projects")){
            if(!std::filesystem::create_directory(UTIL::environmentSpecificAppDataFolderPath() + "LigidPainter" + UTIL::folderDistinguisher() + "Projects"))
                LGDLOG::start<< "ERROR : Creating projects folder." << LGDLOG::end;
        }
        
        //If the tmp directory doesn't exist then create 
        if(!std::filesystem::exists(UTIL::environmentSpecificAppDataFolderPath() + "LigidPainter" + UTIL::folderDistinguisher() + "tmp")){
            if(!std::filesystem::create_directory(UTIL::environmentSpecificAppDataFolderPath() + "LigidPainter" + UTIL::folderDistinguisher() + "tmp"))
                LGDLOG::start<< "ERROR : Creating tmp folder." << LGDLOG::end;
        }

        //If the tmp directory exists empty the folder
        else{
            if(!std::filesystem::remove_all(UTIL::environmentSpecificAppDataFolderPath() + "LigidPainter" + UTIL::folderDistinguisher() + "tmp"))
                LGDLOG::start<< "ERROR : Deleting tmp folder." << LGDLOG::end;
            if(!std::filesystem::create_directory(UTIL::environmentSpecificAppDataFolderPath() + "LigidPainter" + UTIL::folderDistinguisher() + "tmp"))
                LGDLOG::start<< "ERROR : Creating tmp folder. (after deleting)" << LGDLOG::end;
        }
    }
    catch (const std::filesystem::filesystem_error& ex) {
        LGDLOG::start << "ERROR : Filesystem : Location ID 963654 " << ex.what() << LGDLOG::end;
    }

    Settings::defaultFramebuffer()->init(*Settings::videoScale());
    Debugger::block("LOAD : Rest"); //End

    Debugger::block("LOAD : SOURCE LIB TEXTURES"); //Start 16916480

    // Load the source library textures
    Library::loadSourceLibTextures();
    
    Debugger::block("LOAD : SOURCE LIB TEXTURES"); //End
}