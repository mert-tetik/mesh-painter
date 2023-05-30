/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, LigidTools 

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef LGDCOLOR_HPP
#define LGDCOLOR_HPP

#include<glad/glad.h>
#include<GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "assimp/Importer.hpp"
#include "assimp/scene.h"
#include "assimp/postprocess.h"

#include "Shader.hpp"
#include "Box.hpp"
#include "Renderer.h"
#include "Util.hpp"
#include "Timer.hpp"
#include "Texture.hpp"

#include "Mouse.hpp"

#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
#include <cstdlib>
#include <filesystem>

#include "tinyfiledialogs.h"

class Color
{
public:
    glm::vec3 RGB; //Red, Green, Blue 
    glm::vec3 HSV; //Hue, Saturation, Value
    std::string hex; //Hex value which starts with char '#'

    Color(){

    }

    void loadRGB(glm::vec3 value){
        RGB = value;
        HSV = rgbToHsv(value);
        hex = rgbToHex(value);
    }
    void loadHSV(glm::vec3 value){
        RGB = hsvToRgb(value);
        HSV = value;
        hex = hsvToHex(value);
    }
    void loadHex(std::string value){
        RGB = hexToRgb(value);
        HSV = hexToHsv(value);
        hex = value;
    }

    Color(glm::vec3 rgbColor){  
        loadRGB(rgbColor);
    }
    Color(std::string hexColor){  
        loadHex(hexColor);
    }



    //! ---TRANSITIONS - Written by ChatGPT

    glm::vec3 rgbToHsv(const glm::vec3& rgb)
    {
        glm::vec3 hsv;

        float maxVal = std::max(rgb.r, std::max(rgb.g, rgb.b));
        float minVal = std::min(rgb.r, std::min(rgb.g, rgb.b));
        float delta = maxVal - minVal;

        // Calculate hue
        if (delta == 0)
            hsv.x = 0.0f;
        else if (maxVal == rgb.r)
            hsv.x = 60.0f * ((int)((rgb.g - rgb.b) / delta) % 6);
        else if (maxVal == rgb.g)
            hsv.x = 60.0f * (((rgb.b - rgb.r) / delta) + 2);
        else if (maxVal == rgb.b)
            hsv.x = 60.0f * (((rgb.r - rgb.g) / delta) + 4);

        // Calculate saturation
        if (maxVal == 0)
            hsv.y = 0.0f;
        else
            hsv.y = delta / maxVal;

        // Calculate value
        hsv.z = maxVal;

        return hsv;
    }

    glm::vec3 hsvToRgb(const glm::vec3& hsv)
    {
        glm::vec3 rgb;

        float hue = hsv.x;
        float saturation = hsv.y;
        float value = hsv.z;

        if (saturation == 0.0f) {
            // If saturation is 0, the color is gray (shades of white)
            rgb = glm::vec3(value);
        }
        else {
            float chroma = value * saturation;
            float huePrime = hue / 60.0f;
            float x = chroma * (1.0f - std::abs(std::fmod(huePrime, 2.0f) - 1.0f));
            float m = value - chroma;

            if (huePrime >= 0.0f && huePrime < 1.0f)
                rgb = glm::vec3(chroma, x, 0.0f);
            else if (huePrime >= 1.0f && huePrime < 2.0f)
                rgb = glm::vec3(x, chroma, 0.0f);
            else if (huePrime >= 2.0f && huePrime < 3.0f)
                rgb = glm::vec3(0.0f, chroma, x);
            else if (huePrime >= 3.0f && huePrime < 4.0f)
                rgb = glm::vec3(0.0f, x, chroma);
            else if (huePrime >= 4.0f && huePrime < 5.0f)
                rgb = glm::vec3(x, 0.0f, chroma);
            else if (huePrime >= 5.0f && huePrime < 6.0f)
                rgb = glm::vec3(chroma, 0.0f, x);

            rgb += glm::vec3(m);
        }

        return rgb;
    }

    glm::vec3 hexToRgb(const std::string& hex)
    {
        // Remove leading '#' if present
        std::string hexValue = hex;
        if (hexValue[0] == '#')
            hexValue.erase(0, 1);

        // Parse the hex string into RGB components
        unsigned int r, g, b;
        std::stringstream ss;
        ss << std::hex << hexValue;
        ss >> r >> g >> b;

        // Convert the RGB components to the range [0, 255]
        glm::vec3 rgb(r / 255.0f, g / 255.0f, b / 255.0f);
        return rgb;
    }

    std::string rgbToHex(const glm::vec3& rgb)
    {
        // Convert RGB values from [0, 1] range to [0, 255]
        glm::ivec3 rgbInt = glm::ivec3(rgb * 255.0f);

        // Convert RGB values to hex string
        std::stringstream ss;
        ss << "#" << std::setfill('0') << std::hex
           << std::setw(2) << rgbInt.r
           << std::setw(2) << rgbInt.g
           << std::setw(2) << rgbInt.b;
        return ss.str();
    }

    glm::vec3 hexToHsv(const std::string& hex)
    {
        // Remove leading '#' if present
        std::string hexValue = hex;
        if (hexValue[0] == '#')
            hexValue.erase(0, 1);

        // Parse the hex string into RGB components
        unsigned int r, g, b;
        std::stringstream ss;
        ss << std::hex << hexValue;
        ss >> r >> g >> b;

        // Convert the RGB components to the range [0, 255]
        glm::vec3 rgb(r / 255.0f, g / 255.0f, b / 255.0f);
        return rgbToHsv(rgb);
    }

    std::string hsvToHex(const glm::vec3& hsv)
    {
        // Convert HSV to RGB
        glm::vec3 rgb = hsvToRgb(hsv);

        // Convert RGB to hex string
        std::stringstream ss;
        ss << "#" << std::hex << std::setw(2) << std::setfill('0') << static_cast<int>(rgb.r * 255)
           << std::setw(2) << std::setfill('0') << static_cast<int>(rgb.g * 255)
           << std::setw(2) << std::setfill('0') << static_cast<int>(rgb.b * 255);

        return ss.str();
    }


};


#endif