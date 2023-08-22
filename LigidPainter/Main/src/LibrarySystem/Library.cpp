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

#include "UTIL/Util.hpp"
#include "3D/ThreeD.hpp"
#include "GUI/GUI.hpp"
#include "LibrarySystem/Library.hpp"

#include <vector>
#include <string>

std::vector<Texture> __textures;
std::vector<Material> __materials;
std::vector<Brush> __brushes;
std::vector<Model> __TDModels;
std::vector<Filter> __filters;

int __selectedElementIndex = 0;

bool __changed = true;

void Library::uniqueNameControl(){
    for (int i = __textures.size()-1; i >= 0; i--)
    {
        std::vector<std::string> texturesStr;
        for (size_t istr = 0; istr < __textures.size(); istr++)
        {
            if(i != istr)
                texturesStr.push_back(__textures[istr].title);
        }

        if(UTIL::uniqueName(__textures[i].title,texturesStr)){
            __changed = true;
        }    
    }

    for (int i = __materials.size()-1; i >= 0; i--)
    {
        std::vector<std::string> materialsStr;
        for (size_t istr = 0; istr < __materials.size(); istr++)
        {
            if(i != istr)
                materialsStr.push_back(__materials[istr].title);
        }

        if(UTIL::uniqueName(__materials[i].title,materialsStr)){
            __changed = true;
        }    
    }

    for (int i = __brushes.size()-1; i >= 0; i--)
    {
        std::vector<std::string> brushesStr;
        for (size_t istr = 0; istr < __brushes.size(); istr++)
        {
            if(i != istr)
                brushesStr.push_back(__brushes[istr].title);
        }

        if(UTIL::uniqueName(__brushes[i].title,brushesStr)){
            __changed = true;
        }    
    }
    
    for (int i = __TDModels.size()-1; i >= 0; i--)
    {
        std::vector<std::string> TDModelsStr;
        for (size_t istr = 0; istr < __TDModels.size(); istr++)
        {
            if(i != istr)
                TDModelsStr.push_back(__TDModels[istr].title);
        }

        if(UTIL::uniqueName(__TDModels[i].title,TDModelsStr)){
            __changed = true;
        }    
    }
    
    for (int i = __filters.size() - 1; i >= 0; i--)
    {
        std::vector<std::string> filtersStr;
        for (size_t istr = 0; istr < __filters.size(); istr++)
        {
            if(i != istr)
                filtersStr.push_back(__filters[istr].title);
        }

        if(UTIL::uniqueName(__filters[i].title, filtersStr)){
            __changed = true;
        }    
    }
}

void Library::addTexture(Texture texture){
    __changed = true;
    
    registerTextureAdditionAction("Texture added", Texture(), texture, __textures.size());

    texture.uniqueId = 0; 

    __textures.push_back(texture);
    Library::textureGiveUniqueId(__textures.size() - 1);
}

void Library::addMaterial(Material material){
    __changed = true;
    
    material.uniqueID = 0; 

    //Give unique ID to the material

    __materials.push_back(material);
    Library::materialGiveUniqueId(__materials.size() - 1);
}

void Library::addBrush(Brush brush){
    __changed = true;
    
    __brushes.push_back(brush);
}

void Library::addModel(Model model){
    __changed = true;
    
    __TDModels.push_back(model);
}

void Library::addFilter(Filter filter){
    __changed = true;
    
    __filters.push_back(filter);
}

void Library::eraseTexture   (int index){
    
    if(index >= __textures.size()){
        LGDLOG::start<< "ERROR! : Couldn't erase the texture : Requested texture index is out of boundaries." << LGDLOG::end;
        return;
    }

    registerTextureDeletionAction("Texture Deletion", Texture(), __textures[index], index);

    __changed = true;
    
    glDeleteTextures(1, &__textures[index].ID);

    __textures.erase(__textures.begin() + index);
}

void Library::eraseMaterial  (int index){
    
    if(index >= __materials.size()){
        LGDLOG::start<< "ERROR! : Couldn't erase the material : Requested material index is out of boundaries." << LGDLOG::end;
        return;
    }
    
    __changed = true;

    glDeleteFramebuffers(1, &__materials[index].displayingFBO);
    glDeleteTextures(1, &__materials[index].displayingTexture);
    
    __materials.erase(__materials.begin() + index);
}

void Library::eraseBrush     (int index){

    if(index >= __brushes.size()){
        LGDLOG::start<< "ERROR! : Couldn't erase the brush : Requested brush index is out of boundaries." << LGDLOG::end;
        return;
    }
    __changed = true;
    
    __brushes.erase(__brushes.begin() + index);
}

void Library::eraseModel     (int index){
    
    if(index >= __TDModels.size()){
        LGDLOG::start<< "ERROR! : Couldn't erase the model : Requested model index is out of boundaries." << LGDLOG::end;
        return;
    }
        
    
    __changed = true;
    
    for (size_t mshI = 0; mshI < __TDModels[index].meshes.size(); mshI++)
    {
        glDeleteVertexArrays(1, &__TDModels[index].meshes[mshI].VAO);
        glDeleteBuffers(1, &__TDModels[index].meshes[mshI].VBO);
        
        glDeleteTextures(1, &__TDModels[index].meshes[mshI].albedo.ID);
        glDeleteTextures(1, &__TDModels[index].meshes[mshI].roughness.ID);
        glDeleteTextures(1, &__TDModels[index].meshes[mshI].metallic.ID);
        glDeleteTextures(1, &__TDModels[index].meshes[mshI].normalMap.ID);
        glDeleteTextures(1, &__TDModels[index].meshes[mshI].heightMap.ID);
        glDeleteTextures(1, &__TDModels[index].meshes[mshI].ambientOcclusion.ID);
    }

    __TDModels.erase(__TDModels.begin() + index);
}

void Library::eraseFilter     (int index){
    
    if(index >= __filters.size()){
        LGDLOG::start<< "ERROR! : Couldn't erase the filter : Requested filter index is out of boundaries." << LGDLOG::end;
        return;
    }

    __changed = true;
    
    //TODO Delete the displaying texture

    __filters.erase(__filters.begin() + index);
}

void Library::clearTextures   (){
    __changed = true;
    
    for (size_t i = 0; i < __textures.size(); i++)
    {
        glDeleteTextures(1, &__textures[i].ID);
    }
    
    __textures.clear();
}

void Library::clearMaterials  (){
    __changed = true;
    
    for (size_t i = 0; i < __materials.size(); i++)
    {
        glDeleteFramebuffers(1, &__materials[i].displayingFBO);
        glDeleteTextures(1, &__materials[i].displayingTexture);
    }

    __materials.clear();
}

void Library::clearBrushes     (){
    __changed = true;
    
    __brushes.clear();
}

void Library::clearModels     (){
    __changed = true;
    
    for (size_t i = 0; i < __TDModels.size(); i++)
    {
        for (size_t mshI = 0; mshI < __TDModels[i].meshes.size(); mshI++)
        {
            glDeleteVertexArrays(1, &__TDModels[i].meshes[mshI].VAO);
            glDeleteBuffers(1, &__TDModels[i].meshes[mshI].VBO);

            glDeleteTextures(1, &__TDModels[i].meshes[mshI].albedo.ID);
            glDeleteTextures(1, &__TDModels[i].meshes[mshI].roughness.ID);
            glDeleteTextures(1, &__TDModels[i].meshes[mshI].metallic.ID);
            glDeleteTextures(1, &__TDModels[i].meshes[mshI].normalMap.ID);
            glDeleteTextures(1, &__TDModels[i].meshes[mshI].heightMap.ID);
            glDeleteTextures(1, &__TDModels[i].meshes[mshI].ambientOcclusion.ID);
        }
    }

    __TDModels.clear();
}

void Library::clearFilters     (){
    __changed = true;
    
    for (size_t i = 0; i < __filters.size(); i++)
    {
        //TODO Delete displaying texture
    }

    __filters.clear();
}

void Library::changeSelectedElementIndex(int newI){
    __changed = true;
    
    __selectedElementIndex = newI;
}

int Library::getSelectedElementIndex(){
    return __selectedElementIndex;
}

bool Library::isChanged(){
    return __changed;
}

void Library::setChanged(bool state){
    __changed = state;
}

Texture* Library::getTexture(int index){
    if(index >= __textures.size()){
        LGDLOG::start<< "ERROR! : Couldn't get the texture : Requested texture index is out of boundaries." << LGDLOG::end;
        Texture a = Texture();
        return &a;
    }
    return &__textures[index];
}
Material* Library::getMaterial(int index){
    if(index >= __materials.size()){
        LGDLOG::start<< "ERROR! : Couldn't get the material : Requested material index is out of boundaries." << LGDLOG::end;
        Material a = Material();
        return &a;
    }
    return &__materials[index];
}
Brush* Library::getBrush(int index){
    if(index >= __brushes.size()){
        LGDLOG::start<< "ERROR! : Couldn't get the brush : Requested brush index is out of boundaries." << LGDLOG::end;
        Brush a = Brush();
        return &a;
    }
    return &__brushes[index];
}
Model* Library::getModel(int index){
    if(index >= __TDModels.size()){
        LGDLOG::start<< "ERROR! : Couldn't get the model : Requested model index is out of boundaries." << LGDLOG::end;
        Model a = Model();
        return &a;
    }
    return &__TDModels[index];
}

Filter* Library::getFilter(int index){
    if(index >= __filters.size()){
        LGDLOG::start<< "ERROR! : Couldn't get the filter : Requested filter index is out of boundaries." << LGDLOG::end;
        Filter a = Filter();
        return &a;
    }
    return &__filters[index];
}

int Library::getTextureArraySize(){
    return __textures.size();
}
int Library::getMaterialArraySize(){
    return __materials.size();
}
int Library::getBrushArraySize(){
    return __brushes.size();
}
int Library::getModelArraySize(){
    return __TDModels.size();
}
int Library::getFilterArraySize(){
    return __filters.size();
}

void Library::textureGiveUniqueId(int index){
	std::vector<int> IDArray;

	for (size_t i = 0; i < __textures.size(); i++)
	{
		IDArray.push_back(__textures[i].uniqueId);
	}
	
	UTIL::giveUniqueId(__textures[index].uniqueId, IDArray);
}

void Library::materialGiveUniqueId(int index){
	std::vector<int> IDArray;

	for (size_t i = 0; i < __materials.size(); i++)
	{
		IDArray.push_back(__materials[i].uniqueID);
	}

	UTIL::giveUniqueId(__materials[index].uniqueID, IDArray);
}

std::vector<Texture>* Library::getTextureVectorPointer(){
    return &__textures;
}