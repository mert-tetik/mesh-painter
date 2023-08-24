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

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <filesystem>

//Forward declarations for the interaction functions defined in the Interactions directory 
void libraryPanelDisplayerInteraction(Panel &libraryPanelDisplayer, Panel &paintingPanel, Painter &painter, NewTextureDialog &newTextureDialog, AppMaterialModifiers& appMaterialModifiers, int frameCounter);
void updateLibraryPanelDisplayerElements(Panel &libraryPanelDisplayer, int frameCounter);
void paintingPanelInteraction(Panel &paintingPanel, Painter &painter, Dropper &dropper);
void windowPanelInteraction(Panel &windowPanel, Painter &painter, SettingsDialog &settingsDialog, DisplayerDialog &displayerDialog,ExportDialog &exportDialog);
void paintingModesPanelInteraction(Panel &paintingModesPanel, Painter &painter);
void nodeInteraction();
void displayingModesPanelInteraction(Panel &displayingModesPanel, Painter &painter);

void UI::elementInteraction(
                                Painter &painter,
                                Timer &timer, 
                                float screenGapPerc,
                                Project& project,
                                AppMaterialModifiers& appMaterialModifiers
                            )
{
    //!Dialog & panel state
    this->anyDialogActive = 
                    textureSelectionDialog.dialogControl.isActive() || 
                    materialEditorDialog.dialogControl.isActive()   || 
                    anyContextMenuActive                            || 
                    displayerDialog.dialogControl.isActive()        || 
                    greetingDialog.dialogControl.isActive()         || 
                    settingsDialog.dialogControl.isActive()         || 
                    newProjectDialog.dialogControl.isActive()       || 
                    newTextureDialog.dialogControl.isActive()       || 
                    textureEditorDialog.dialogControl.isActive()    || 
                    loadProjectDialog.dialogControl.isActive();   

    this->anyPanelHover = 
                    navigationPanel.hover               ||
                    windowPanel.hover                   ||
                    paintingPanel.hover                 ||
                    libraryPanelLeft.hover              ||
                    libraryPanelDisplayer.hover         ||
                    nodeEditorDisplayer.hover           ||
                    selectedTextureDisplayer.hover      || 
                    paintingModesPanel.hover;

    this->contextMenuInteraction(timer, project, painter);
    
    updateLibraryPanelDisplayerElements(libraryPanelDisplayer, frameCounter);
    
    libraryPanelDisplayerInteraction(libraryPanelDisplayer, paintingPanel, painter, newTextureDialog, appMaterialModifiers,frameCounter);
    
    this->libraryPanelLeftInteraction(libraryPanelLeft);

    paintingPanelInteraction(paintingPanel, painter, dropper);
    
    windowPanelInteraction(windowPanel, painter, settingsDialog, displayerDialog,exportDialog);

    paintingModesPanelInteraction(paintingModesPanel,painter);

    displayingModesPanelInteraction(displayingModesPanel, painter);

    nodeInteraction();
    
    this->panelPositioning(screenGapPerc,painter);
}