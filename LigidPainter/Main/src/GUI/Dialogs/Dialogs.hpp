
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

#ifndef DIALOGS_HPP
#define DIALOGS_HPP


#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <iostream>
#include <vector>


#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"

//Forward declarations
class ColorPalette;
class ContextMenu;
struct AppSettings;

class DialogControl
{
private:
public:
    bool active = false;
    float mixVal = 0.;
    bool firstFrameActivated = false;

    DialogControl();
    DialogControl(bool active);
    
    void updateStart(Shader buttonShader);
    void updateEnd(Timer timer,Shader buttonShader,float transitionDuration);
    void activate();
    void unActivate();
    bool isActive();
    bool isComplete();
};

//!------------------------------MESSAGE BOX------------------------------

#define MESSAGEBOX_TYPE_ERROR 0
#define MESSAGEBOX_TYPE_WARNING 1
#define MESSAGEBOX_TYPE_INFO 2
#define MESSAGEBOX_TYPE_QUESTION 3

#define MESSAGEBOX_BUTTON_OK 0
#define MESSAGEBOX_BUTTON_OKCANCEL 1
#define MESSAGEBOX_BUTTON_YESNO 2
#define MESSAGEBOX_BUTTON_YESNOCANCEL 3

int showMessageBox(const std::string title, const std::string message, const int type, const int button);

//!------------------------------SELECT FILE SYSTEM OBJECT DIALOG------------------------------


#define FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FILE 0
#define FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_SELECT_FOLDER 1
#define FILE_SYSTEM_OBJECT_SELECTION_DIALOG_TYPE_EXPORT_FILE 2

#define FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_TEXTURE 0
#define FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_MODEL 1
#define FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_MATERIAL 2
#define FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_BRUSH 3
#define FILE_SYSTEM_OBJECT_SELECTION_DIALOG_FILTER_TEMPLATE_LIGID 4

std::string showFileSystemObjectSelectionDialog(const std::string title, const std::string defaultPath, const int filterTemplate, const bool multipleSelection, const int type);

//!------------------------------SETTINGS DIALOG------------------------------

class NewProjectDialog;
class LoadProjectDialog;
class TextureSelectionDialog;

class Skybox;

class SettingsDialog
 {
 private:
    Context context;
 public:
    Panel panel;
    Shader buttonShader;
    DialogControl dialogControl;

    //Constructor
    SettingsDialog();
    SettingsDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures);

    //Public member functions    
    void render(LigidWindow originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,Library &library,
                glm::vec2 videoScale, AppSettings& settings);
 };


//!------------------------------NEW TEXTURE DIALOG------------------------------


class NewTextureDialog
 {
 private:
    Context context;
 public:
     
     Panel panel;
     NewTextureDialog();
     Shader buttonShader;
     DialogControl dialogControl;

    //Constructor
    NewTextureDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures);
    
    //Public member function
    void render(LigidWindow originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,
                Library &library,glm::vec2 videoScale,int textureRes);
 };


//!------------------------------LOAD PROJECT DIALOG------------------------------


class LoadProjectDialog
 {
 public:
    Panel bgPanel;
    Panel projectsPanel;
    Button loadButton;
    Button textBtn1;
    Button textBtn2;
    Button textBtn3;
    Button textBtn4;
    Box box;
    Shader buttonShader;
    AppTextures appTextures;
    AppMaterialModifiers appMaterialModifiers;
    DialogControl dialogControl;

    //Constructors
    LoadProjectDialog();
    LoadProjectDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures, AppMaterialModifiers& appMaterialModifiers);
    
    //Public member functions
    void render(LigidWindow originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,glm::vec2 videoScale,
                Project &project,bool &greetingDialogActive,bool &startScreen,Library &library,Shaders shaders,Model &model,int &textureRes,std::vector<Node> &meshNodeScene);
 };


//!------------------------------GREETING DIALOG DIALOG------------------------------


class DisplayerDialog
 {
 private:
    Context context;
    Shader prefilteringShader;
    Shader skyboxBallShader;
    Model sphereModel;
 public:
    Panel panel;
    Shader buttonShader;
    DialogControl dialogControl;
    std::vector<Element> skyboxes;

    //Constructors
    DisplayerDialog();
    DisplayerDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures,
                    Shader prefilteringShader,Shader skyboxBallShader,Model &sphereModel);
    
    //Public member functions
    void render(LigidWindow originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,
                Library &library,glm::vec2 videoScale,Skybox &skybox);
 };


//!------------------------------GREETING DIALOG DIALOG------------------------------


class GreetingDialog
{
public:
   Button loadProjectButton;
   Button createProjectButton;
   Button textureDisplayerButton;
   Button textButton1;
   Panel bgPanel;
   Shader buttonShader;
   Box box;
   DialogControl dialogControl;

   bool startScreen = true;
   
   //Constructors
   GreetingDialog();
   GreetingDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures);
   
   //Public member functions
   void render(LigidWindow originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,
               glm::vec2 videoScale, NewProjectDialog &newProjectDialog,LoadProjectDialog &loadProjectDialog);
};

//!------------------------------MATERIAL EDITOR DIALOG------------------------------


class MaterialEditorDialog
{
private:
    Shader buttonShader;
    Shader heightToNormalShader;
    Shader boundaryExpandingShader;
    Shader tdModelShader;
    ColorPalette colorPalette;
    Model sphereModel;

    //Private member functions
    void updateLayerPanel(Material &material,int &textureRes,Box &box,Context &context);
    void checkLayerPanel(Material &material,Mouse &mouse,std::vector<ContextMenu> &contextMenus,glm::vec2 videoScale);
    void checkModifiersPanel(Material &material,float textureRes,Box box,Context context,Mouse &mouse,TextureSelectionDialog &textureSelectionDialog);
    void updateLayerPanelElements(Material &material,int &textureRes,Box &box,Context &context,Mouse& mouse, glm::vec2 videoScale, std::vector<ContextMenu> contextMenus);
    void checkTextureSelectionDialog(TextureSelectionDialog &textureSelectionDialog, Material &material,Library &library,float textureRes,Box box,Context context);
    void manageContextMenuActions(Library &library, Mouse &mouse, Material &material, int textureRes, Box box, Context context, std::vector<ContextMenu> &contextMenus);

    bool updateTheMaterial = false;
    bool prevUpdateTheMaterial = false;
public:
   DialogControl dialogControl;
   glm::vec3 pos = glm::vec3(50.f,50.f,0.8f); ///Position of the dialog
   glm::vec2 scale = glm::vec2(40,40); ///Scale of the dialog
   glm::vec3 posLayer = glm::vec3(20.f,50.f,0.8f); ///Position of the layer panel
   glm::vec2 scaleLayer = glm::vec2(10,40); ///Scale of the layer panel
   glm::vec3 posModifier = glm::vec3(80.f,50.f,0.8f); ///Position of the layer panel
   glm::vec2 scaleModifier = glm::vec2(10,40); ///Scale of the layer panel
   Button materialDisplayer; //That buttons displays the material with a texture
   Panel bgPanel; //To cover the bg
   Panel layerPanel; //Modifiers will be displayed in there
   Panel modifiersPanel; //Modifiers will be displayed in there
   ComboBox displayModeComboBox;
   Button barButton; //Modifiers will be displayed in there
   int textureModifierTextureSelectingButtonIndex = 1000; //1000 if none of them is selecting TODO REMOVE THAT
   AppMaterialModifiers appMaterialModifiers;
   int selectedMaterialModifierIndex = 0;

   Camera displayerCamera;
    
   //Constructors
   MaterialEditorDialog();
   MaterialEditorDialog(Shader buttonShader, Shader heightToNormalShader, Shader boundaryExpandingShader, Shader tdModelShader, ColorPalette colorPalette,AppTextures appTextures,Model &sphereModel);

   //Public member functions
   void render(glm::vec2 videoScale,Mouse& mouse,Timer &timer,TextRenderer &textRenderer,TextureSelectionDialog &textureSelectionDialog,Library &library,
               Material &material, int textureRes,Box box,Context context,std::vector<ContextMenu> &contextMenus,std::vector<Node>& meshNodeScene,Model& model,
               Scene scene);
   void activate();
   void deactivate(TextureSelectionDialog &textureSelectionDialog);
};


//!------------------------------NEW PROJECT DIALOG------------------------------


class NewProjectDialog
 {
 private:
    AppTextures appTextures;
    AppMaterialModifiers appMaterialModifiers;
 public:
    Panel panel;
    Box box;
    Shader buttonShader;
    DialogControl dialogControl;

    //Constructors
    NewProjectDialog();
    NewProjectDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures, AppMaterialModifiers& appMaterialModifiers);

    //Public member functions
    void render(LigidWindow originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,glm::vec2 videoScale,
                Project &project,bool &greetingDialogActive,bool &startScreen,Library &library,Shaders shaders,Model &model,int &textureRes,std::vector<Node> &meshNodeScene);
 };


//!------------------------------EXPORT DIALOG------------------------------

class ExportDialog
 {
 private:
    AppTextures appTextures;
    Context context;
 public:
    Panel panel;
    Box box;
    Shader buttonShader;
    DialogControl dialogControl;

    //Constructors
    ExportDialog();
    ExportDialog(Context context,glm::vec2 videoScale,ColorPalette colorPalette,Shader buttonShader,AppTextures appTextures);

    //Public member functions
    void render(LigidWindow originalWindow,ColorPalette colorPalette,Mouse& mouse,Timer timer,TextRenderer &textRenderer,glm::vec2 videoScale,
                Project &project,bool &greetingDialogActive,Library &library,Shaders shaders,Model &model,MaterialEditorDialog &materialEditorDialog
                ,std::vector<Node> &meshNodeScene, Model sphereModel,Scene scene);
 };


//!------------------------------TEXTURE SELECTION DIALOG------------------------------

void showTextureSelectionDialog(Texture& txtr);
bool wasTextureSelectionDialogActive();

class TextureSelectionDialog
{
private:
    ColorPalette colorPalette;
    Shader buttonShader;
    int selectedTextureMode = 0;
public:
    Panel bgPanel;
    Panel subPanel;
    Panel selectedTextureDisplayingPanel;
    Panel textureSelectingPanel;

    Shader proceduralDisplayerShader;

    int selectedTextureIndex = 0; 
    bool clicked = false; 
    glm::vec3 pos = glm::vec3(50,50,0.95f);
    glm::vec2 scale = glm::vec2(40); 
    DialogControl dialogControl;

    //Constructors
    TextureSelectionDialog();
    TextureSelectionDialog(Shader buttonShader, ColorPalette colorPalette, AppTextures appTextures);

    //Public member functions
    void show(glm::vec2 videoScale,Timer &timer,Library library, glm::mat4 guiProjection, Texture& receivedTexture, LigidWindow& window, TextRenderer& textRenderer);
};

//!------------------------------CONTEXT MENU------------------------------


class ContextMenu
{
private:
    Shader buttonShader;
public:
    Panel contextPanel;
    glm::vec3 pos;
    int selectedElement = 0;
    DialogControl dialogControl; 
    
    //Constructors
    ContextMenu();
    ContextMenu(Shader buttonShader,ColorPalette colorPalette,std::vector<std::string> elements);
    
    /// @brief Public member function to render the context menu
    void render(glm::vec2 videoScale,Mouse& mouse,Timer &timer,TextRenderer &textRenderer);
};

#endif