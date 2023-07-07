/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

Renderer.h : Renders the whole screen
*/

#ifndef LGDRENDERER_HPP
#define LGDRENDERER_HPP

#include <iostream>
#include <filesystem>

#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include "../../thirdparty/include/glm/glm.hpp"
#include "../../thirdparty/include/glm/gtc/matrix_transform.hpp"
#include "../../thirdparty/include/glm/gtc/type_ptr.hpp"
#include "../../thirdparty/include/glm/gtx/string_cast.hpp"

#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"
#include "GUI/GUI.hpp"

class Renderer
{
public:
    Context context; //Holds data related to the window
    Scene scene; //3D Scene structure
    Model model; //Loaded 3D Model
    Model sphereModel; //Sphere 3D Model to display the materials

    //Structure that holds all the shaders
    //*Define shaders there then init like that shaders.tdModelShader = Shader("a.vert","a.frag");
    Shaders shaders; 

    Mouse mouse; //Everything related to the mouse is done via that class

    Box box; //A class that used render 2D square vertices 

    UI userInterface; 

    //Manage time
    //Commonly used to support the transition effects (to be make sure the transition is done in certain time & not effect by rendering speed)
    Timer timer;    

    //Structure that holds the font classes that will be used by the text renderer (TextRenderer.hpp)
    Fonts fonts;

    Websites websites;

    //That class is responisble of rendering texts
    //Constructor takes a font class
    //You can change the font in the runtime
    TextRenderer textRenderer; 

    //Textures those will be used in UI of the app
    AppTextures appTextures;

    std::vector<Node> meshNodeScene; //Contains app nodes

    Library library;

    ColorPalette colorPalette;

    std::vector<ContextMenu> contextMenus; //0 for textures , 1 for materials, 2 for brushes

    int textureRes = 1024; //Textures will be generated with that resolution

    Project project;

    Skybox skybox;

    Painter painter;

    bool VSync = false; //Set swapInterval(1) if true
    bool backfaceCulling = false;



    //Constructor
    Renderer(glm::vec2 videoScale);

    void render();
private:
    bool previousModelNewModelAdded = false;

    //Init.cpp
    void initGlad();
    void loadAppTextures();
    void loadShaders();
    void createContextMenus();
#
    //Stuff
    void updateViewMatrix();
    void updateProjectionMatrix();
    void updateViewport();
    
    //Rendering stuff
    void renderSkyBox();
    void set3DUniforms();

    //CALLBACKS
    double previousClickTime = 0.0;
    glm::vec2 lastMousePos;//This will be used as "last frame's cursor pos" for the cursor offset 
    
    void mouseButtonCallback(LigidWindow window, int button, int action, int mods);
    void framebufferSizeCallback(LigidWindow window, int width, int height);
    void scrollCallback(LigidWindow window, double xoffset, double yoffset);
    void cursorPositionCallback(LigidWindow window, double xpos, double ypos);
    void keyCallback(LigidWindow window,int key,int scancode,int action,int mods);

};
#endif