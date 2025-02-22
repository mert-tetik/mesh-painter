
/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/


#include<glad/glad.h>
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "Renderer.h"
#include "GUI/GUI.hpp"
#include "3D/ThreeD.hpp"
#include "UTIL/Shader/Shader.hpp"
#include "UTIL/Library/Library.hpp"
#include "UTIL/Mouse/Mouse.hpp"
#include "UTIL/Settings/Settings.hpp"
#include "UTIL/ColorPalette/ColorPalette.hpp"
#include "Toolkit/Layers/Layers.hpp"
#include "GUI/Panels.hpp"

#include <string>
#include <vector>

Panel panel_layers;
Panel panel_add_layer;
Button button_mesh_selection;
ComboBox comboBox_layers_resolution;
ComboBox comboBox_PBR_displaying_mode;

static int lastLayerResolution = 1024;

void panel_layers_render(Timer& timer, bool doMouseTracking)
{
    // Render the layers panel
    panel_layers.render(timer, doMouseTracking && !comboBox_layers_resolution.pressed && !comboBox_PBR_displaying_mode.pressed);
    if(panel_layers.resizingDone){
        panels_transform();
    }
    
    int layersResolution = std::stoi(comboBox_layers_resolution.texts[comboBox_layers_resolution.selectedIndex]);

    // Render the layers
    getScene()->get_selected_mesh()->layerScene.render(timer, panel_layers, doMouseTracking && !comboBox_layers_resolution.pressed && !comboBox_PBR_displaying_mode.pressed, layersResolution, *getScene()->get_selected_mesh());

    // Render the add layer panel
    panel_add_layer.render(timer, doMouseTracking && !comboBox_layers_resolution.pressed && !comboBox_PBR_displaying_mode.pressed);
    
    button_mesh_selection.render(timer, doMouseTracking && !comboBox_layers_resolution.pressed && !comboBox_PBR_displaying_mode.pressed);
    
    Button comboBox_layers_resolutionBG = Button(ELEMENT_STYLE_SOLID, glm::vec2(comboBox_layers_resolution.scale.x, comboBox_layers_resolution.scale.y + comboBox_PBR_displaying_mode.scale.y), "", Texture(), 0.f, 0);
    comboBox_layers_resolutionBG.pos = glm::mix(comboBox_layers_resolution.pos, comboBox_PBR_displaying_mode.pos, 0.5f);
    comboBox_layers_resolutionBG.render(timer, false);

    comboBox_layers_resolution.render(timer, doMouseTracking);
    
    comboBox_PBR_displaying_mode.render(timer, doMouseTracking);
    
    // Height map selected
    if(comboBox_PBR_displaying_mode.selectedIndex == 5){
        dialog_heightMapDisplayer.show(timer, getScene()->model);
        comboBox_PBR_displaying_mode.selectedIndex = 0;
    }

    bool any_layer_hovered = false;
    for (Layer* layer : getScene()->get_selected_mesh()->layerScene.layers)
    {
        if(layer->layerGUI.layerButton.hover)
            any_layer_hovered = true;
    }

    int context_menu_res = -1;
    if(panel_layers.hover && *Mouse::RClick() && !any_layer_hovered){
        context_menu_res = show_context_menu(timer, {"Add Texture Layer", "Add Painting Layer", "Add Material Layer", "Add Vector Layer"});
    }

    // Add texture layer 
    if(panel_add_layer.sections[0].elements[0].button.clicked || context_menu_res == 0){
        TextureLayer* txtrLayer = new TextureLayer(layersResolution);
        getScene()->get_selected_mesh()->layerScene.add_new(txtrLayer);
    }
   
    // Add painting layer 
    if(panel_add_layer.sections[0].elements[1].button.clicked || context_menu_res == 1){
        PaintingLayer* paintingLayer = new PaintingLayer(layersResolution);
        getScene()->get_selected_mesh()->layerScene.add_new(paintingLayer);
    }
    
    // Add material layer 
    if(panel_add_layer.sections[0].elements[2].button.clicked || context_menu_res == 2){
        MaterialLayer* materialLayer = new MaterialLayer(layersResolution);
        getScene()->get_selected_mesh()->layerScene.add_new(materialLayer);
    }
    
    // Add vector layer 
    if(panel_add_layer.sections[0].elements[3].button.clicked || context_menu_res == 3){
        VectorLayer* vectorLayer = new VectorLayer(layersResolution);
        getScene()->get_selected_mesh()->layerScene.add_new(vectorLayer);
    }

    if(getScene()->model->newModelAdded || lastLayerResolution != layersResolution){
        if(lastLayerResolution != layersResolution){
            if(getScene()->get_selected_mesh()->layerScene.layers.size()){
                int res = showMessageBox(
                                            "Warning!", 
                                            "You are altering the resolution value of the layers!" 
                                            " This action will resize each textures in the layers."
                                            " And is irreversible for painting layers (once you go low resolution there is no covering painting textures to high resolution)"
                                            " Are you sure you want to proceed?",
                                            MESSAGEBOX_TYPE_WARNING,
                                            MESSAGEBOX_BUTTON_YESNO
                                            );
                
                // Pressed to no
                if(res == 0){
                    comboBox_layers_resolution.selectedIndex = std::log2(lastLayerResolution / 256); 
                    layersResolution = lastLayerResolution;
                }
            }
        }

        getScene()->get_selected_mesh()->layerScene.update_all_layers(layersResolution, glm::vec3(0.f), *getScene()->get_selected_mesh());
    }

    lastLayerResolution = layersResolution;

}