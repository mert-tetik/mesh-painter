/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    Updating texture fragment shader

*/

//OpenGL version used in the app (Major 4 , Minor 0, Profile core)
#version 400 core

//Painting
#pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Painting.frag)

in vec2 TexCoords;
in vec4 ProjectedPos;

//The texture that will be updated
uniform sampler2D txtr; 

//Contains the brush strokes
uniform sampler2D paintingTexture;

//0 = paint
//1 = soften
//2 = smear
uniform int brushModeState; 

//Selected opacity for painting
uniform float paintingOpacity;

//Selected color for painting
uniform vec3 paintingColor;

uniform sampler2D paintingOverTexture;
uniform int usePaintingOver;
uniform int paintingOverGrayScale;
uniform int paintingOverWraping;

//3D Model rendered with depth shader (to compare depth)
uniform sampler2D depthTexture;

uniform float smearTransformStrength;
uniform float smearBlurStrength;

uniform int multiChannelsPaintingMod;
uniform int channelI;
uniform float channelStrength;

//Do depth testing (painting) if set to 1
uniform int doDepthTest;

//Fragment shader output
out vec4 fragColor;

void main(){
    vec3 screenPos = 0.5 * (vec3(1,1,1) + ProjectedPos.xyz / ProjectedPos.w);

    vec4 brushTxtr = texture(paintingTexture, TexCoords);

    if(!(brushModeState == 2 || brushModeState == 3)){
        brushTxtr.a = brushTxtr.r;
    }

    float txtrAlpha = texture(txtr, TexCoords).a; 

    fragColor = vec4(getBrushedTexture(txtr, brushTxtr, paintingTexture, TexCoords, screenPos.xy, brushModeState, smearTransformStrength, smearBlurStrength, multiChannelsPaintingMod == 1, channelI, channelStrength, usePaintingOver == 1), txtrAlpha);
}