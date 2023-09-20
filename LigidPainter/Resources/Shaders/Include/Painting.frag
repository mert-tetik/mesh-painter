/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

Copyright (c) 2022-2023, Mert Tetik

All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------

    *This file is included to other shader code using painting

    *Include that file using : #pragma LIGID_INCLUDE(./LigidPainter/Resources/Shaders/Include/Painting.frag)

    And don't forget to disable the squiggles for the GLSL 
    (squiggles becomes annoying since the vscode compiler doesn't recognise including other shader files) 
*/


//----------------------- DEPTH TESTING -----------------------

// Constants
const float far = 1.0;
const float near = 0.1;

/**
 * Linearize the depth value from a depth buffer.
 * 
 * @return The linearized depth value.
 */
float linearizeDepth(
                        float depth //The depth value to linearize.
                    ) 
{

    // Compute the linearized depth value
    float linearizedDepth = (2.0 * near * far) / (far + near - (depth * 2.0 - 1.0) * (far - near));

    return linearizedDepth;

}


//----------------------- SOFTEN BRUSH ----------------------- 

const int LOD = 1,         // gaussian done on MIPmap at scale LOD
          sLOD = 1 << LOD; // tile size = 2^LOD

/**
* Util function for the getSoftenedTexture function
* Calculate the value of a Gaussian function at the given position.
* 
* returns The computed value of the Gaussian function.
*/
float gaussian(
                vec2 i, //The input position in 2D space 
                int range //The range parameter for the Gaussian function
            ) 
{
    // Compute the standard deviation based on the range parameter.
    float sigma = float(range) * 0.25;

    // Calculate the exponent term of the Gaussian function.
    float exponent = -0.5 * dot(i /= sigma, i);

    // Compute the final value of the Gaussian function.
    float value = exp(exponent) / (6.28 * sigma * sigma);

    return value;
}

/**
*    Apply soften brush to the given texture
*
*    Example : 
*
*    vec3 albedo;
*    if(paintTheAlbedoTexture)
*        albedo = getSoftenedTexture(txtr, brushTxtr, TexCoords);
*    else
*        albedo = texture2D(txtr, TexCoords).rgb;
*           
*
*    returns the painted color data with given texture coordinate
**/
vec3 getSoftenedTexture(
                            sampler2D txtr, //The texture that will be painted
                            vec4 brushTxtr, //Brush value (painted texture value)
                            vec2 TexCoords //The texture coordinates
                        )
{ //Intensity from the painted texture
    const float width = 1024;
    int range = 20;
    
    vec4 O = vec4(0); 
    int s = range/sLOD;
    
    // Loop through the subdivisions and accumulate the blurred color
    for (int i = 0; i < s * s; i++) {
        // Compute the offset from the center of the range
        vec2 d = vec2(i % s, i / s) * float(sLOD) - float(range) / 2.0;

        // Compute the contribution using the Gaussian function and textureLod
        O += gaussian(d, range) * textureLod(txtr, TexCoords + vec2(1.0 / width) * d, float(LOD));
    }
    
    //Mix the blurred color data with original texture data with the brush texture's alpha value
    return mix(
                texture(txtr,TexCoords).rgb, //Original value
                O.rgb/O.a, //Blurred value
                brushTxtr.a //The brush value
            );
}


//----------------------- SMEAR BRUSH -----------------------


/**
*    Apply smear brush to the given texture
*
*    Example : 
*
*    vec3 albedo;
*    if(paintTheAlbedoTexture)
*        albedo = getSmearedTexture(txtr, brushTxtr, TexCoords);
*    else
*        albedo = texture2D(txtr, TexCoords).rgb;
*           
*
*    returns the painted color data with given texture coordinate
**/
vec3 getSmearedTexture(
                        sampler2D txtr, //The texture that will be painted  
                        vec4 brushTxtr, //Brush value (painted texture value)
                        vec2 TexCoords  //The texture coordinates
                    )
{
    
    
    float intensity = brushTxtr.a/2.;
    
    //Get the direction value from the painted texture (which direction the painting made in the current texture coordinates)
    vec2 Direction = brushTxtr.rg * 2.;

    //Samples (multiple of 2)
    int Samples = 64;
     
    //Get the blurred color (directional)
    vec4 blurredColor = vec4(0.0);  
    
    for(int i = -Samples/2; i <= Samples/2; i++)
    {
        blurredColor += texture(txtr, TexCoords - vec2(Direction.x * (float(i) / float(Samples)), Direction.y * (float(i) / float(Samples)))) / float(Samples + 1);
    }
    
    //TODO : Write smt better to that returning value

    return blurredColor.rgb;
}


//----------------------- NORMAL BRUSH -----------------------

/**
*    Apply default brush to the given texture
*
*    Example : 
*
*    vec3 albedo;
*    if(paintTheAlbedoTexture)
*        albedo = getPaintedTexture(txtr, brushTxtr, TexCoords, vec3(1,0,0)); //Paint to red
*    else
*        albedo = texture2D(txtr, TexCoords).rgb;
*           
*
*    returns the painted color data with given texture coordinate
**/
vec3 getPaintedTexture  ( 
                            sampler2D txtr, //The texture that will be painted  
                            vec4 brushTxtr, //Brush value (painted texture value)
                            vec2 TexCoords,  //The texture coordinates
                            vec2 modelCoord,  //The texture coordinates
                            vec3 paintingColor, //Painting color
                            sampler2D paintingOverTexture, //Painting color
                            int usePaintingOver,
                            int paintingOverGrayScale,
                            int paintingOverWraping
                        )
{
    vec4 paintingOverTxtrVal;
    if(paintingOverWraping == 0)
        paintingOverTxtrVal = texture(paintingOverTexture, modelCoord);
    else 
        paintingOverTxtrVal = texture(paintingOverTexture, TexCoords);

    float intensity = brushTxtr.a;

    //Mix the original color data with painting color using the intensity of the painted texture
    vec3 destColor = vec3(0);
    if(usePaintingOver == 1){
        if(paintingOverGrayScale == 1){
            destColor = paintingColor;
            intensity *= paintingOverTxtrVal.r;
        }
        else{
            destColor = paintingOverTxtrVal.rgb;
            intensity *= paintingOverTxtrVal.a;
        }
    }
    else
        destColor = paintingColor;

    return mix  (
                    texture(txtr,TexCoords).rgb,
                    destColor, //paintingColor
                    intensity
                );
}

vec3 getPaintedTextureTxtrClr( 
                            sampler2D txtr, //The texture that will be painted  
                            vec4 brushTxtr, //Brush value (painted texture value)
                            vec2 TexCoords  //The texture coordinates
                        )
{

    float intensity = brushTxtr.a;

    //Mix the original color data with painting color using the intensity of the painted texture
    vec3 destColor = brushTxtr.rgb;

    return mix  (
                    texture(txtr,TexCoords).rgb,
                    destColor, //paintingColor
                    intensity
                );
}

vec3 getPaintedTextureFilterDisplayingMode( 
                                            sampler2D txtr, //The texture that will be painted  
                                            vec4 brushTxtr, //Brush value (painted texture value)
                                            vec2 TexCoords  //The texture coordinates
                                        )
{

    float intensity = brushTxtr.a;

    //Mix the original color data with painting color using the intensity of the painted texture
    vec3 destColor = vec3(1.) - texture(txtr,TexCoords).rgb;

    return mix  (
                    texture(txtr,TexCoords).rgb,
                    destColor, //paintingColor
                    intensity
                );
}


//----------------------- COMBINATION -----------------------  

/**
*    Apply a brush to the given texture
*
*    Use that function as the given examples of the other painting functions 
*
*    brushModeState == 0 : paint with color
*    
*    brushModeState == 1 : paint with softening
*
*    brushModeState == 2 : paint with smearing
*
*    returns the painted color data with given texture coordinate
**/
vec3 getBrushedTexture (
                            sampler2D txtr, //The texture that will be painted  
                            vec4 brushTxtr, //Brush value (painted texture value)
                            vec2 TexCoords,  //The texture coordinates
                            vec2 modelCoord,  //The texture coordinates
                            vec3 paintingColor, //Painting color (used for painting with color (if brushModeState == 0))
                            sampler2D paintingOverTexture, //Painting color (used for painting with color (if brushModeState == 0))
                            int brushModeState, //Decide which painting mode will be used
                            int usePaintingOver,
                            int paintingOverGrayScale,
                            int paintingOverWraping
                        )
{
    //Apply painting with color
    if(brushModeState == 0)
        return getPaintedTexture(txtr,brushTxtr,TexCoords, modelCoord, paintingColor, paintingOverTexture, usePaintingOver, paintingOverGrayScale, paintingOverWraping);

    //Apply painting with softening
    if(brushModeState == 1)
        return getSoftenedTexture(txtr,brushTxtr,TexCoords);

    //Apply painting with smearing
    if(brushModeState == 2)
        return getSmearedTexture(txtr,brushTxtr,TexCoords);
    
    //Apply painting with painting texture color data
    if(brushModeState == 3)
        return getPaintedTextureTxtrClr(txtr, brushTxtr, TexCoords);
    
    //Apply painting with inverted texture color
    if(brushModeState == 4)
        return getPaintedTextureFilterDisplayingMode(txtr, brushTxtr, TexCoords);
    
    if(brushModeState == 5)
        return texture(txtr, TexCoords).rgb;
    
    //If the brushModeState value is not valid
    return vec3(0);
}
