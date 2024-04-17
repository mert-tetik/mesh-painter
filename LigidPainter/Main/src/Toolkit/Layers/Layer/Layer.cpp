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

#include <string>
#include <iostream>
#include <vector>

#include "GUI/Elements.hpp"
#include "GUI/Dialogs.hpp"
#include "Toolkit/Layers/Layers.hpp"

static bool __enteredPanelOnce = false;


int Layer::render_graphics(Timer& timer, bool doMosueTracking, glm::vec3 pos, glm::vec2 scale, float opacity, const unsigned int resolution, Mesh& mesh){

    layerButton.pos = pos;
    layerButton.scale = scale;

    layerButton.update_result_transform_values();

    if(opacity == 1.f)
        this->render_layer_bg();

    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", opacity);
    layerButton.color.a = 0.4f;
    layerButton.text = this->title;
    layerButton.render(timer, doMosueTracking && !eyeBtn.hover);


    if(scale.x > 8.f){
        ShaderSystem::buttonShader().setFloat("properties.groupOpacity", opacity / 2.f);
        Button layerIconDisplayer = Button(ELEMENT_STYLE_SOLID, glm::vec2(1, scale.y), "", this->layerIcon, 0.f, false);
        layerIconDisplayer.color = glm::vec4(0.f);
        layerIconDisplayer.outline = false;
        layerIconDisplayer.pos = pos;
        layerIconDisplayer.pos.x -= scale.x - layerIconDisplayer.scale.x;
        layerIconDisplayer.render(timer, false);
        ShaderSystem::buttonShader().setFloat("properties.groupOpacity", opacity);
        
        if(hiden)
            eyeBtn.texture = appTextures.eyeClosedIcon;
        else{
            eyeBtn.texture = appTextures.eyeOpenedIcon;
        }
        eyeBtn.color = glm::vec4(0.f);
        eyeBtn.color2 = glm::vec4(0.f);
        eyeBtn.outline = false;
        eyeBtn.pos = layerIconDisplayer.pos;
        eyeBtn.pos.x += layerIconDisplayer.scale.x + 0.7f * 1.5f;
        eyeBtn.scale = glm::vec2(0.7f, scale.y) + glm::vec2(0.1f) * eyeBtn.hoverMixVal + glm::vec2(0.1f) * eyeBtn.clickedMixVal;
        if(eyeBtn.clicked)
            this->hiden = !this->hiden;

        eyeBtn.render(timer, doMosueTracking);
        
        Button generalOpacityMapDisplayer = Button(ELEMENT_STYLE_SOLID, glm::vec2(1, scale.y), "", this->alpha.general_Alpha.alphaMap, 0.f, false);
        generalOpacityMapDisplayer.color = glm::vec4(0.f);
        generalOpacityMapDisplayer.outline = false;
        generalOpacityMapDisplayer.pos = pos;
        generalOpacityMapDisplayer.pos.x += scale.x - generalOpacityMapDisplayer.scale.x;
        generalOpacityMapDisplayer.render(timer, false);
        
        Button generalOpacityDisplayer = Button(ELEMENT_STYLE_SOLID, glm::vec2(1.5f, scale.y), "", Texture(), 0.f, false);
        generalOpacityDisplayer.color = glm::vec4(0.f);
        generalOpacityDisplayer.outline = false;
        std::string generalOpacityDisplayerTxt = std::to_string(this->alpha.general_Alpha.alphaValue);
        generalOpacityDisplayerTxt.erase(generalOpacityDisplayerTxt.begin() + 4, generalOpacityDisplayerTxt.end());
        generalOpacityDisplayer.text = generalOpacityDisplayerTxt; 
        generalOpacityDisplayer.pos = generalOpacityMapDisplayer.pos;
        generalOpacityDisplayer.pos.x -= generalOpacityMapDisplayer.scale.x + generalOpacityDisplayer.scale.x;
        generalOpacityDisplayer.render(timer, false);
    }

    if(mainSelected){
        layerButton.color = ColorPalette::themeColor;
        layerButton.color.r /= 2.0f;
        layerButton.color.g /= 2.0f;
        layerButton.color.b /= 2.0f;
        
        layerButton.textColor = ColorPalette::oppositeColor;
    }
    else if(subSelected){
        layerButton.color = ColorPalette::themeColor;
        layerButton.color.r /= 4.0f;
        layerButton.color.g /= 4.0f;
        layerButton.color.b /= 4.0f;

        layerButton.textColor = ColorPalette::oppositeColor;
        layerButton.textColor.r /= 1.3f;
        layerButton.textColor.g /= 1.3f;
        layerButton.textColor.b /= 1.3f;
    }
    else{
        layerButton.color = ColorPalette::secondColor;
        layerButton.textColor = ColorPalette::oppositeColor;
    }


    if(layerButton.clicked){
        mainSelected = true;
        subSelected = true;
        return 2;
    }

    if(layerButton.hover && *Mouse::RClick()){
        mainSelected = true;
        subSelected = true;

        int res;
        if(this->layerType == "painting")
            res = show_context_menu(timer, {"Layer Info", "Opacity Settings", "Rename" , "Delete"});
        else
            res = show_context_menu(timer, {"Layer Info", "Opacity Settings", "Rename" , "Delete" , "Set Elements"});
        
        if(res == 0){
            infoMode = true;
        }
        if(res == 1){
            alphaSettingsMode = true;
        }
        if(res == 2){
            dialog_renaming.show(timer, this->layerButton.pos, this->layerButton.scale.x, &this->title, 25);
        }
        if(res == 3){
            return 1;
        }
        if(res == 4){
            elementSelectionMode = true;
        }

        return 2;
    }

    if(elementSelectionMode)
        this->render_element_selection_panel(timer, true, resolution, mesh);

    if(alphaSettingsMode)
        this->renderAlphaSettingsPanel(timer, true, resolution, mesh);

    if(infoMode)
        this->renderInfoPanel(timer, true);
    
    ShaderSystem::buttonShader().setFloat("properties.groupOpacity", 1.f);

    return 0;
}






























void Layer::updateLayerButton(){
    this->layerButton = Button(ELEMENT_STYLE_SOLID, glm::vec2(1,1.5f), this->title, Texture(), 0.f, false);
}

void Layer::genResultChannels(const unsigned int resolution){
    result.albedo = Texture(nullptr, resolution, resolution);
    result.roughness = Texture(nullptr, resolution, resolution);
    result.metallic = Texture(nullptr, resolution, resolution);
    result.normalMap = Texture(nullptr, resolution, resolution);
    result.heightMap = Texture(nullptr, resolution, resolution);
    result.ambientOcclusion = Texture(nullptr, resolution, resolution);

    this->alpha.general_Alpha.genTxtrs();
    this->alpha.albedo_Alpha.genTxtrs();
    this->alpha.roughness_Alpha.genTxtrs();
    this->alpha.metallic_Alpha.genTxtrs();
    this->alpha.normalMap_Alpha.genTxtrs();
    this->alpha.heightMap_Alpha.genTxtrs();
    this->alpha.ambientOcclusion_Alpha.genTxtrs();
}

void Layer::updateProceduralMaskTextures(const unsigned int resolution, Mesh& mesh){
    this->alpha.general_Alpha.alphaMapProceduralTxtr.update(nullptr, resolution, resolution);
    this->alpha.albedo_Alpha.alphaMapProceduralTxtr.update(nullptr, resolution, resolution);
    this->alpha.roughness_Alpha.alphaMapProceduralTxtr.update(nullptr, resolution, resolution);
    this->alpha.metallic_Alpha.alphaMapProceduralTxtr.update(nullptr, resolution, resolution);
    this->alpha.normalMap_Alpha.alphaMapProceduralTxtr.update(nullptr, resolution, resolution);
    this->alpha.heightMap_Alpha.alphaMapProceduralTxtr.update(nullptr, resolution, resolution);
    this->alpha.ambientOcclusion_Alpha.alphaMapProceduralTxtr.update(nullptr, resolution, resolution);

    this->alpha.general_Alpha.alphaMap.generateProceduralTexture(mesh, this->alpha.general_Alpha.alphaMapProceduralTxtr, resolution);
    this->alpha.albedo_Alpha.alphaMap.generateProceduralTexture(mesh, this->alpha.albedo_Alpha.alphaMapProceduralTxtr, resolution);
    this->alpha.roughness_Alpha.alphaMap.generateProceduralTexture(mesh, this->alpha.roughness_Alpha.alphaMapProceduralTxtr, resolution);
    this->alpha.metallic_Alpha.alphaMap.generateProceduralTexture(mesh, this->alpha.metallic_Alpha.alphaMapProceduralTxtr, resolution);
    this->alpha.normalMap_Alpha.alphaMap.generateProceduralTexture(mesh, this->alpha.normalMap_Alpha.alphaMapProceduralTxtr, resolution);
    this->alpha.heightMap_Alpha.alphaMap.generateProceduralTexture(mesh, this->alpha.heightMap_Alpha.alphaMapProceduralTxtr, resolution);
    this->alpha.ambientOcclusion_Alpha.alphaMap.generateProceduralTexture(mesh, this->alpha.ambientOcclusion_Alpha.alphaMapProceduralTxtr, resolution);
} 

void Layer::updateResultTextureResolutions(const unsigned int resolution, Mesh& mesh){
    if(result.albedo.getResolution().x != resolution){
        result.albedo.resize(glm::ivec2(resolution));
        result.roughness.resize(glm::ivec2(resolution));
        result.metallic.resize(glm::ivec2(resolution));
        result.normalMap.resize(glm::ivec2(resolution));
        result.heightMap.resize(glm::ivec2(resolution));
        result.ambientOcclusion.resize(glm::ivec2(resolution));

        this->updateProceduralMaskTextures(resolution, mesh);
    }
}

void Layer::renderAlphaSettingsPanel(Timer& timer, bool doMouseTracking, const unsigned int resolution, Mesh& mesh){
    alphaSettingsPanel.sections[0].elements[1].button.textureSelection3D = true;
    alphaSettingsPanel.sections[0].elements[3].button.textureSelection3D = true;
    alphaSettingsPanel.sections[0].elements[5].button.textureSelection3D = true;
    alphaSettingsPanel.sections[0].elements[7].button.textureSelection3D = true;
    alphaSettingsPanel.sections[0].elements[9].button.textureSelection3D = true;
    alphaSettingsPanel.sections[0].elements[11].button.textureSelection3D = true;
    alphaSettingsPanel.sections[0].elements[13].button.textureSelection3D = true;


    alphaSettingsPanel.sections[0].elements[0].rangeBar.value = this->alpha.general_Alpha.alphaValue;
    alphaSettingsPanel.sections[0].elements[1].button.texture = this->alpha.general_Alpha.alphaMap;
    alphaSettingsPanel.sections[0].elements[2].rangeBar.value = this->alpha.albedo_Alpha.alphaValue;
    alphaSettingsPanel.sections[0].elements[3].button.texture = this->alpha.albedo_Alpha.alphaMap;
    alphaSettingsPanel.sections[0].elements[4].rangeBar.value = this->alpha.roughness_Alpha.alphaValue;
    alphaSettingsPanel.sections[0].elements[5].button.texture = this->alpha.roughness_Alpha.alphaMap;
    alphaSettingsPanel.sections[0].elements[6].rangeBar.value = this->alpha.metallic_Alpha.alphaValue;
    alphaSettingsPanel.sections[0].elements[7].button.texture = this->alpha.metallic_Alpha.alphaMap;
    alphaSettingsPanel.sections[0].elements[8].rangeBar.value = this->alpha.normalMap_Alpha.alphaValue;
    alphaSettingsPanel.sections[0].elements[9].button.texture = this->alpha.normalMap_Alpha.alphaMap;
    alphaSettingsPanel.sections[0].elements[10].rangeBar.value = this->alpha.heightMap_Alpha.alphaValue;
    alphaSettingsPanel.sections[0].elements[11].button.texture = this->alpha.heightMap_Alpha.alphaMap;
    alphaSettingsPanel.sections[0].elements[12].rangeBar.value = this->alpha.ambientOcclusion_Alpha.alphaValue;
    alphaSettingsPanel.sections[0].elements[13].button.texture = this->alpha.ambientOcclusion_Alpha.alphaMap;

    alphaSettingsPanel.pos = this->layerButton.pos;
    alphaSettingsPanel.pos.x -= this->layerButton.scale.x + alphaSettingsPanel.scale.x;
    alphaSettingsPanel.pos.y -= this->layerButton.scale.y - alphaSettingsPanel.scale.y;
    alphaSettingsPanel.render(timer, doMouseTracking);

    if(alphaSettingsPanel.hover)
        __enteredPanelOnce = true;

    if(*Mouse::LPressed())
        __enteredPanelOnce = false;

    if(*Mouse::LClick() && !__enteredPanelOnce && !alphaSettingsPanel.hover){
        this->alphaSettingsMode = false;
        __enteredPanelOnce = false;
    }

    if(__enteredPanelOnce && !alphaSettingsPanel.hover){
        this->alphaSettingsMode = false;
        __enteredPanelOnce = false;
    }
    
    this->alpha.general_Alpha.alphaValue = alphaSettingsPanel.sections[0].elements[0].rangeBar.value;
    this->alpha.general_Alpha.alphaMap = alphaSettingsPanel.sections[0].elements[1].button.texture;
    this->alpha.albedo_Alpha.alphaValue = alphaSettingsPanel.sections[0].elements[2].rangeBar.value;
    this->alpha.albedo_Alpha.alphaMap = alphaSettingsPanel.sections[0].elements[3].button.texture;
    this->alpha.roughness_Alpha.alphaValue = alphaSettingsPanel.sections[0].elements[4].rangeBar.value;
    this->alpha.roughness_Alpha.alphaMap = alphaSettingsPanel.sections[0].elements[5].button.texture;
    this->alpha.metallic_Alpha.alphaValue = alphaSettingsPanel.sections[0].elements[6].rangeBar.value;
    this->alpha.metallic_Alpha.alphaMap = alphaSettingsPanel.sections[0].elements[7].button.texture;
    this->alpha.normalMap_Alpha.alphaValue = alphaSettingsPanel.sections[0].elements[8].rangeBar.value;
    this->alpha.normalMap_Alpha.alphaMap = alphaSettingsPanel.sections[0].elements[9].button.texture;
    this->alpha.heightMap_Alpha.alphaValue = alphaSettingsPanel.sections[0].elements[10].rangeBar.value;
    this->alpha.heightMap_Alpha.alphaMap = alphaSettingsPanel.sections[0].elements[11].button.texture;
    this->alpha.ambientOcclusion_Alpha.alphaValue = alphaSettingsPanel.sections[0].elements[12].rangeBar.value;
    this->alpha.ambientOcclusion_Alpha.alphaMap = alphaSettingsPanel.sections[0].elements[13].button.texture;

    if(!this->alphaSettingsMode){
        this->updateProceduralMaskTextures(resolution, mesh);
    }
}

void Layer::renderInfoPanel(Timer& timer, bool doMouseTracking){
    Button detailed_display_btn = Button(ELEMENT_STYLE_SOLID, glm::vec2(2.f), "", appTextures.eyeOpenedIcon, 0.f, false);
    
    infoPanel.sections[0].elements[0].button.text = "Title : " + this->title;
    infoPanel.sections[0].elements[1].button.text = "Type : " + this->layerType;
    infoPanel.sections[0].elements[1].button.texture = this->layerIcon;

    infoPanel.sections[0].elements[2].button.texture = this->alpha.albedo_Alpha.alphaMapProceduralTxtr;
    infoPanel.sections[0].elements[4].button.texture = this->alpha.albedo_Alpha.alphaMapProceduralTxtr;
    infoPanel.sections[0].elements[7].button.texture = this->alpha.roughness_Alpha.alphaMapProceduralTxtr;
    infoPanel.sections[0].elements[10].button.texture = this->alpha.metallic_Alpha.alphaMapProceduralTxtr;
    infoPanel.sections[0].elements[13].button.texture = this->alpha.normalMap_Alpha.alphaMapProceduralTxtr;
    infoPanel.sections[0].elements[16].button.texture = this->alpha.heightMap_Alpha.alphaMapProceduralTxtr;
    infoPanel.sections[0].elements[19].button.texture = this->alpha.ambientOcclusion_Alpha.alphaMapProceduralTxtr;
    
    infoPanel.sections[0].elements[5].button.texture = this->result.albedo;
    infoPanel.sections[0].elements[8].button.texture = this->result.roughness;
    infoPanel.sections[0].elements[11].button.texture = this->result.metallic;
    infoPanel.sections[0].elements[14].button.texture = this->result.normalMap;
    infoPanel.sections[0].elements[17].button.texture = this->result.heightMap;
    infoPanel.sections[0].elements[20].button.texture = this->result.ambientOcclusion;
    
    infoPanel.pos = this->layerButton.pos;
    infoPanel.pos.x -= this->layerButton.scale.x + infoPanel.scale.x;
    infoPanel.pos.y -= this->layerButton.scale.y - infoPanel.scale.y;
    infoPanel.clearDepthBuffer = false;
    infoPanel.render(timer, doMouseTracking);

    Button detailed_display_btn_albedo = Button(ELEMENT_STYLE_SOLID, glm::vec2(1.5f), "", appTextures.eyeOpenedIcon, 0.f, false);
    Button detailed_display_btn_roughness = Button(ELEMENT_STYLE_SOLID, glm::vec2(1.5f), "", appTextures.eyeOpenedIcon, 0.f, false);
    Button detailed_display_btn_metallic = Button(ELEMENT_STYLE_SOLID, glm::vec2(1.5f), "", appTextures.eyeOpenedIcon, 0.f, false);
    Button detailed_display_btn_normalMap = Button(ELEMENT_STYLE_SOLID, glm::vec2(1.5f), "", appTextures.eyeOpenedIcon, 0.f, false);
    Button detailed_display_btn_heightMap = Button(ELEMENT_STYLE_SOLID, glm::vec2(1.5f), "", appTextures.eyeOpenedIcon, 0.f, false);
    Button detailed_display_btn_ao = Button(ELEMENT_STYLE_SOLID, glm::vec2(1.5f), "", appTextures.eyeOpenedIcon, 0.f, false);
    
    detailed_display_btn_albedo.color.a = 0.f; detailed_display_btn_albedo.outlineExtra = false;
    detailed_display_btn_roughness.color.a = 0.f; detailed_display_btn_roughness.outlineExtra = false;
    detailed_display_btn_metallic.color.a = 0.f; detailed_display_btn_metallic.outlineExtra = false;
    detailed_display_btn_normalMap.color.a = 0.f; detailed_display_btn_normalMap.outlineExtra = false;
    detailed_display_btn_heightMap.color.a = 0.f; detailed_display_btn_heightMap.outlineExtra = false;
    detailed_display_btn_ao.color.a = 0.f; detailed_display_btn_ao.outlineExtra = false;

    detailed_display_btn_albedo.pos = glm::vec3(infoPanel.sections[0].elements[5].button.pos.x + infoPanel.sections[0].elements[5].button.scale.x - detailed_display_btn_albedo.scale.x-0.5f, infoPanel.sections[0].elements[5].button.pos.y - infoPanel.sections[0].elements[5].button.scale.y + detailed_display_btn_albedo.scale.y+0.5f, infoPanel.sections[0].elements[5].button.pos.z);
    detailed_display_btn_roughness.pos = glm::vec3(infoPanel.sections[0].elements[8].button.pos.x + infoPanel.sections[0].elements[8].button.scale.x - detailed_display_btn_roughness.scale.x-0.5f, infoPanel.sections[0].elements[8].button.pos.y - infoPanel.sections[0].elements[8].button.scale.y + detailed_display_btn_roughness.scale.y+0.5f, infoPanel.sections[0].elements[8].button.pos.z);
    detailed_display_btn_metallic.pos = glm::vec3(infoPanel.sections[0].elements[11].button.pos.x + infoPanel.sections[0].elements[11].button.scale.x - detailed_display_btn_metallic.scale.x-0.5f, infoPanel.sections[0].elements[11].button.pos.y - infoPanel.sections[0].elements[11].button.scale.y + detailed_display_btn_metallic.scale.y+0.5f, infoPanel.sections[0].elements[11].button.pos.z);
    detailed_display_btn_normalMap.pos = glm::vec3(infoPanel.sections[0].elements[14].button.pos.x + infoPanel.sections[0].elements[14].button.scale.x - detailed_display_btn_normalMap.scale.x-0.5f, infoPanel.sections[0].elements[14].button.pos.y - infoPanel.sections[0].elements[14].button.scale.y + detailed_display_btn_normalMap.scale.y+0.5f, infoPanel.sections[0].elements[14].button.pos.z);
    detailed_display_btn_heightMap.pos = glm::vec3(infoPanel.sections[0].elements[17].button.pos.x + infoPanel.sections[0].elements[17].button.scale.x - detailed_display_btn_heightMap.scale.x-0.5f, infoPanel.sections[0].elements[17].button.pos.y - infoPanel.sections[0].elements[17].button.scale.y + detailed_display_btn_heightMap.scale.y+0.5f, infoPanel.sections[0].elements[17].button.pos.z);
    detailed_display_btn_ao.pos = glm::vec3(infoPanel.sections[0].elements[20].button.pos.x + infoPanel.sections[0].elements[20].button.scale.x - detailed_display_btn_ao.scale.x-0.5f, infoPanel.sections[0].elements[20].button.pos.y - infoPanel.sections[0].elements[20].button.scale.y + detailed_display_btn_ao.scale.y+0.5f, infoPanel.sections[0].elements[20].button.pos.z);

    detailed_display_btn_albedo.render(timer, true);
    detailed_display_btn_roughness.render(timer, true);
    detailed_display_btn_metallic.render(timer, true);
    detailed_display_btn_normalMap.render(timer, true);
    detailed_display_btn_heightMap.render(timer, true);
    detailed_display_btn_ao.render(timer, true);

    glClear(GL_DEPTH_BUFFER_BIT);

    if(detailed_display_btn_albedo.hover && *Mouse::LClick()){
        dialog_textureDisplayer.show(timer, this->result.albedo);
    }
    if(detailed_display_btn_roughness.hover && *Mouse::LClick()){
        dialog_textureDisplayer.show(timer, this->result.roughness);
    }
    if(detailed_display_btn_metallic.hover && *Mouse::LClick()){
        dialog_textureDisplayer.show(timer, this->result.metallic);
    }
    if(detailed_display_btn_normalMap.hover && *Mouse::LClick()){
        dialog_textureDisplayer.show(timer, this->result.normalMap);
    }
    if(detailed_display_btn_heightMap.hover && *Mouse::LClick()){
        dialog_textureDisplayer.show(timer, this->result.heightMap);
    }
    if(detailed_display_btn_ao.hover && *Mouse::LClick()){
        dialog_textureDisplayer.show(timer, this->result.ambientOcclusion);
    }

    if(infoPanel.hover)
        __enteredPanelOnce = true;

    if(*Mouse::LPressed())
        __enteredPanelOnce = false;

    if(*Mouse::LClick() && !__enteredPanelOnce && !infoPanel.hover){
        this->infoMode = false;
        __enteredPanelOnce = false;
    }

    if(__enteredPanelOnce && !infoPanel.hover){
        this->infoMode = false;
        __enteredPanelOnce = false;
    }
}

void Layer::render_layer_bg(){
    {
        ShaderSystem::color2d().use();
        ShaderSystem::color2d().setMat4("projection", getContext()->ortho_projection);
        ShaderSystem::color2d().setVec4("color" , glm::vec4(0.f)); 
        //Bottom
        ShaderSystem::color2d().setVec3("pos", glm::vec3(layerButton.resultPos.x, layerButton.resultPos.y + layerButton.resultScale.y + layerButton.resultScale.y * 4.f,   1.f)); 
        ShaderSystem::color2d().setVec2("scale", glm::vec2(layerButton.resultScale * 4.f));
        LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Panel : Barrier bottom");
        //Top
        ShaderSystem::color2d().setVec3("pos", glm::vec3(layerButton.resultPos.x, layerButton.resultPos.y - layerButton.resultScale.y - layerButton.resultScale.y * 4.f,   1.f)); 
        ShaderSystem::color2d().setVec2("scale", glm::vec2(layerButton.resultScale * 4.f));
        LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Panel : Barrier bottom");
    }

    if(this->layerType == "material"){
        Material* material;
        this->get_type_specific_variable(&material, nullptr, nullptr, nullptr, nullptr);
        ShaderSystem::textureRenderingShader().use();
        ShaderSystem::textureRenderingShader().setMat4("projection", getContext()->ortho_projection);
        ShaderSystem::textureRenderingShader().setVec2("scale", glm::vec2(layerButton.resultScale.x/1.4f, layerButton.resultScale.x/ 1.4f));
        ShaderSystem::textureRenderingShader().setVec3("pos", layerButton.resultPos);
        ShaderSystem::textureRenderingShader().setInt("txtr", 0); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, material->displayingTexture.ID);
        ShaderSystem::textureRenderingShader().setFloat("opacity", 1.0f);
        ShaderSystem::textureRenderingShader().setFloat("rotation", 0.f);
        ShaderSystem::textureRenderingShader().setFloat("depthToleranceValue", 0);

        if(material->displayingTexture.ID)
            LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Panel : Pre-rendering mode : Rendering result texture");
        
        ShaderSystem::buttonShader().use();    
    }   

    if(this->layerType == "vector"){
        std::vector<VectorStroke3D>* strokes;
        Brush* brush;
        this->get_type_specific_variable(nullptr, &strokes, &brush, nullptr, nullptr);
        ShaderSystem::textureRenderingShader().use();
        ShaderSystem::textureRenderingShader().setMat4("projection", getContext()->ortho_projection);
        ShaderSystem::textureRenderingShader().setVec2("scale", glm::vec2(layerButton.resultScale.x/1.4f, layerButton.resultScale.x/ 1.4f));
        ShaderSystem::textureRenderingShader().setVec3("pos", layerButton.resultPos);
        ShaderSystem::textureRenderingShader().setInt("txtr", 0); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, brush->displayingTexture.ID);
        ShaderSystem::textureRenderingShader().setFloat("opacity", 1.0f);
        ShaderSystem::textureRenderingShader().setFloat("rotation", 0.f);
        ShaderSystem::textureRenderingShader().setFloat("depthToleranceValue", 0);

        if(brush->displayingTexture.ID)
            LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Panel : Pre-rendering mode : Rendering result texture");
        
        ShaderSystem::buttonShader().use();    
    }   

    if(this->layerType == "painting"){
        Texture* capture_txtr;
        this->get_type_specific_variable(nullptr, nullptr, nullptr, nullptr, &capture_txtr);
        ShaderSystem::textureRenderingShader().use();
        ShaderSystem::textureRenderingShader().setMat4("projection", getContext()->ortho_projection);
        ShaderSystem::textureRenderingShader().setVec2("scale", glm::vec2(layerButton.resultScale.x/1.4f, layerButton.resultScale.x/ 1.4f));
        ShaderSystem::textureRenderingShader().setVec3("pos", layerButton.resultPos);
        ShaderSystem::textureRenderingShader().setInt("txtr", 0); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, capture_txtr->ID);
        ShaderSystem::textureRenderingShader().setFloat("opacity", 1.0f);
        ShaderSystem::textureRenderingShader().setFloat("rotation", 0.f);
        ShaderSystem::textureRenderingShader().setFloat("depthToleranceValue", 0);

        if(capture_txtr->ID)
            LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Panel : Pre-rendering mode : Rendering result texture");
        
        ShaderSystem::buttonShader().use();    
    }   

    if(this->layerType == "texture"){
        MaterialChannels* materialChannels;
        this->get_type_specific_variable(nullptr, nullptr, nullptr, &materialChannels, nullptr);
        
        for (size_t i = 0; i < 6; i++)
        {
            Texture txtr;

            if(i == 0)
                txtr = materialChannels->albedo;
            if(i == 1)
                txtr = materialChannels->roughness;
            if(i == 2)
                txtr = materialChannels->metallic;
            if(i == 3)
                txtr = materialChannels->normalMap;
            if(i == 4)
                txtr = materialChannels->heightMap;
            if(i == 5)
                txtr = materialChannels->ambientOcclusion;

            glm::vec2 scale = glm::vec2(layerButton.resultScale.x / 1.4f);
            scale.x /= 6;
            scale.y = layerButton.resultScale.y;

            glm::vec3 pos = layerButton.resultPos;
            pos.x = pos.x - (layerButton.resultScale.x / 1.4f) + scale.x + (scale.x * 2.f * i);

            ShaderSystem::textureRenderingShader().use();
            ShaderSystem::textureRenderingShader().setMat4("projection", getContext()->ortho_projection);
            ShaderSystem::textureRenderingShader().setVec2("scale", scale);
            ShaderSystem::textureRenderingShader().setVec3("pos", pos);
            ShaderSystem::textureRenderingShader().setInt("txtr", 0); glActiveTexture(GL_TEXTURE0); glBindTexture(GL_TEXTURE_2D, txtr.ID);
            ShaderSystem::textureRenderingShader().setFloat("opacity", 1.0f);
            ShaderSystem::textureRenderingShader().setFloat("rotation", 0.f);
            ShaderSystem::textureRenderingShader().setFloat("depthToleranceValue", 0);
        
            if(txtr.ID)
                LigidGL::makeDrawCall(GL_TRIANGLES, 0, 6, "Panel : Pre-rendering mode : Rendering result texture");
        }

        ShaderSystem::buttonShader().use();    
    }   

    glClear(GL_DEPTH_BUFFER_BIT);
}