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
#include "LigidGL/LigidGL.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "UTIL/Util.hpp"
#include "GUI/Elements/Elements.hpp"
#include "3D/ThreeD.hpp"

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <filesystem>
#include <ctime>

bool Project::loadProject(std::string ligidFilePath,Library &library,Shaders shaders,Model &model,AppTextures appTextures,ColorPalette colorPalette, 
                          int& textureRes, std::vector<Node> &meshNodeScene, glm::vec2 videoScale){

    //Return if the ligidFilePath doesn't exists
    if(!std::filesystem::exists(ligidFilePath)){
        std::cout << "ERROR CAN'T LOCATE THE LIGID FILE : " << ligidFilePath << std::endl;
        return false;
    }

    //TODO Do smt with these variables
    time_t creationDate; 
    time_t lastOpenedDate;
    

    if(!readLigidFile(ligidFilePath, creationDate, lastOpenedDate, meshNodeScene, textureRes, shaders, colorPalette, appTextures, videoScale)){
        std::cout << "ERROR CAN'T READ THE LIGID FILE : " << ligidFilePath << ". The file is might not be a ligid file." << std::endl;

        return false;
    }

    //Remove the file from the path (./myProject/myProject.ligid -> ./myProject/) 
    this->folderPath = UTIL::removeLastWordBySeparatingWithChar(ligidFilePath, UTIL::folderDistinguisher());

    //Make sure folder path doesn't have seperator at the end (./myProject/ -> ./myProject)
    if(this->folderPath[this->folderPath.size()-1] == '/' || this->folderPath[this->folderPath.size()-1] == '\\') 
        this->folderPath.pop_back();


    //writeLigidFile(meshNodeScene);


    //Load the textures
    library.clearTextures();
    for (const auto & entry : std::filesystem::directory_iterator(this->folderPath + UTIL::folderDistinguisher() + "Textures")){
        std::string texturePath = entry.path().string();

        Texture texture;
        texture.load(texturePath.c_str());

        //Check if the texture is a png file
        if(UTIL::getLastWordBySeparatingWithChar(texturePath, '.') != "png"){
            //The texture in the project folder is not an png file

            //Delete the non-png file
            std::filesystem::remove(texturePath);
        
            //Recreate the png file
            texture.exportTexture(UTIL::removeLastWordBySeparatingWithChar(texturePath, UTIL::folderDistinguisher()), "PNG");
        }


        library.addTexture(texture);
    }

    //Load the materials
    library.clearMaterials();
    for (const auto & entry : std::filesystem::directory_iterator(this->folderPath + UTIL::folderDistinguisher() + "Materials")){
        std::string materialPath = entry.path().string();

        Material material(textureRes, "", 0);;
        material.readFile(materialPath,colorPalette,shaders.buttonShader,appTextures, library.materials);

        library.addMaterial(material);
    }
    
    //Load the brushes
    library.clearBrushes();
    for (const auto & entry : std::filesystem::directory_iterator(this->folderPath + UTIL::folderDistinguisher() + "Brushes")){
        std::string brushPath = entry.path().string();

        Brush brush;
        brush.readFile(brushPath);
        brush.updateDisplayTexture(shaders.twoDPainting,shaders.buttonShader);


        library.addBrush(brush);
    }
    
    //Load the tdmodels
    library.clearModels();
    for (const auto & entry : std::filesystem::directory_iterator(this->folderPath + UTIL::folderDistinguisher() + "3DModels")){
        std::string modelPath = entry.path().string();

        Model TDModel;
        TDModel.loadModel(modelPath, true);

        //Check if the model is an obj file
        if(UTIL::getLastWordBySeparatingWithChar(modelPath, '.') != "obj"){
            //The 3D model in the project folder is not an obj file

            //Delete the non-obj file
            std::filesystem::remove(modelPath);
        
            //Recreate the obj file
            FileHandler::writeOBJFile(UTIL::removeExtension(modelPath) + ".obj",TDModel);
        }


        library.addModel(TDModel);
    }


    if(library.TDModels.size())
        model = library.TDModels[0];

    return true;
}