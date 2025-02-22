/*
---------------------------------------------------------------------------
LigidPainter - 3D Model texturing software / Texture generator   
---------------------------------------------------------------------------

(c) 2024 Mert Tetik. All rights reserved.

Official GitHub Link : https://github.com/mert-tetik/LigidPainter
Official Web Page : https://ligidtools.com/ligidpainter

---------------------------------------------------------------------------
*/

#ifndef PAINTER_HPP
#define PAINTER_HPP

//OpenGL & window 
#include <glad/glad.h>
#include "LigidGL/LigidGL.hpp"

//OpenGL Math Library GLM
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <vector> //std::vector
#include <string> //std::string
#include <map> //std::map
#include <atomic>

#include "UTIL/Util.hpp"

#include "3D/Material/Material.hpp"
#include "3D/ThreeDPoint/ThreeDPoint.hpp"

#include "Toolkit/TextureFieldScene/TextureFieldScene.hpp"


struct PaintingBuffers{
    
    /// @brief 3D model linearized depth texture (3d model rendered with a depth shader) 
    Texture depth_texture;

    Texture window_painting_texture;

    Texture projected_painting_texture;
    Texture projected_painting_texture_low;

    PaintingBuffers(){}

    PaintingBuffers(Texture depth_texture, Texture window_painting_texture, Texture projected_painting_texture, Texture projected_painting_texture_low){
        this->depth_texture = depth_texture;
        this->window_painting_texture = window_painting_texture;
        this->projected_painting_texture = projected_painting_texture;
        this->projected_painting_texture_low = projected_painting_texture_low;
    }
};

struct MirrorSide{
    
    bool active = false;

    PaintingBuffers paintingBuffers;

    Camera threeD_cam;

    /*! @brief Whichever axis this mirror side effects becomes -1 and other axises being set to 1. For instance glm::vec3(-1.f, 1.f, 1.f) effects only the x axis*/
    glm::vec3 effectAxis = glm::vec3(0.);

    MirrorSide(){}

    MirrorSide(bool active, PaintingBuffers paintingBuffers, glm::vec3 effectAxis){
        this->active = active;
        this->paintingBuffers = paintingBuffers;
        this->effectAxis = effectAxis;
    }
};

struct PaintSettings{    
    struct ColorBuffer{
        
        Color stroke_albedo_color;
        float stroke_roughness_color = 0.f;
        float stroke_metallic_color = 0.f;
        float stroke_normalMap_color = 0.f;
        float stroke_heightMap_color = 0.f;
        float stroke_ao_color = 0.f;

        Material material;
        bool use_custom_material = false;

        ColorBuffer(){}

        ColorBuffer(Color stroke_albedo_color, float stroke_roughness_color, float stroke_metallic_color, float stroke_normalMap_color, float stroke_heightMap_color, float stroke_ao_color, Material material, bool use_custom_material){
            this->stroke_albedo_color = stroke_albedo_color;
            this->stroke_roughness_color = stroke_roughness_color;
            this->stroke_metallic_color = stroke_metallic_color;
            this->stroke_normalMap_color = stroke_normalMap_color;
            this->stroke_heightMap_color = stroke_heightMap_color;
            this->stroke_ao_color = stroke_ao_color;
            this->material = material;
            this->use_custom_material = use_custom_material;
        }
    };

    struct MirrorSettings{
        bool X = false;
        float X_offset = 0.f;

        bool Y = false;
        float Y_offset = 0.f;

        bool Z = false;
        float Z_offset = 0.f;

        MirrorSettings(){}

        MirrorSettings(bool X, float X_offset, bool Y, float Y_offset, bool Z, float Z_offset){
            this->X = X; 
            this->X_offset = X_offset; 
            this->Y = Y; 
            this->Y_offset = Y_offset; 
            this->Z = Z; 
            this->Z_offset = Z_offset; 
        }
    };
    
    struct DrawMode{
        DrawMode(){}
    };
    struct SoftenMode{
        
        /*! @brief This brush will be used for the strokes;*/
        float softening_strength = 0.f;
        
        SoftenMode(){}

        SoftenMode(float softening_strength){
            this->softening_strength = softening_strength;
        }
    };
    struct SmearMode{
        
        float smear_blur_strength = 0.f;
        /*! @brief This brush will be used for the strokes*/
        float smear_transform_strength = 0.f;
        
        SmearMode(){}

        SmearMode(float smear_blur_strength, float smear_transform_strength){
            this->smear_blur_strength = smear_blur_strength;
            this->smear_transform_strength = smear_transform_strength;
        }
    };
    struct NormalMode{
        
        /*! @brief This brush will be used for the strokes;*/
        float normal_strength = 0.f;
        
        NormalMode(){}

        NormalMode(float normal_strength){
            this->normal_strength = normal_strength;
        }
    };
    struct FilterMode{
        
        /*! @brief This brush will be used for the strokes;*/
        Filter filter;
        
        FilterMode(){}

        FilterMode(Filter filter){
            this->filter = filter;
        }
    };
    struct BucketMode{
        BucketMode(){}
    };

    struct PaintVertexBuffer{
        /*! @brief Paint on a 3D model*/
        Mesh* model_mesh;

        /*! @brief Paint on a 2D square*/
        Box* box;

        /*! @brief Paint on model_mesh if set to true, box if else*/
        bool paint_model = false;
        
        PaintVertexBuffer(){}

        PaintVertexBuffer(Mesh* model_mesh, Box* box, bool paint_model){
            this->model_mesh = model_mesh;
            this->box = box;
            this->paint_model = paint_model;
        }
    };

    struct PointData{
        glm::vec2 point_2D;
        ThreeDPoint point_3D; 

        bool use_3D = false;
        
        PointData(){}

        PointData(glm::vec2 point_2D, ThreeDPoint point_3D, bool use_3D){
            this->point_2D = point_2D;   
            this->point_3D = point_3D;   
            this->use_3D = use_3D;  
        }
    };

    struct PaintingOverData{
        bool active = false;
        bool gray_scale = false;
        TextureFieldScene* texture_field_scene;
        
        PaintingOverData(){}

        PaintingOverData(bool active, bool gray_scale, TextureFieldScene* texture_field_scene){
            this->active = active;
            this->gray_scale = gray_scale;
            this->texture_field_scene = texture_field_scene;
        }
    };
    struct PaintedBuffers{
        /*! @brief Paint material_channel textures if set to true. Paint solid_painted_texture if set to false*/
        bool material_painting = false;

        bool material_channel_albedo_active = false;
        Texture material_channel_albedo;
        bool material_channel_roughness_active = false;
        Texture material_channel_roughness;
        bool material_channel_metallic_active = false;
        Texture material_channel_metallic;
        bool material_channel_normalMap_active = false;
        Texture material_channel_normalMap;
        bool material_channel_heightMap_active = false;
        Texture material_channel_heightMap;
        bool material_channel_ao_active = false;
        Texture material_channel_ao;

        /*! @brief Only this texture will be painted if material_painting flag set to false*/
        Texture solid_painted_texture;

        PaintedBuffers(){}
        
        PaintedBuffers( bool material_painting, Texture solid_painted_texture, bool material_channel_albedo_active, Texture material_channel_albedo, 
                        bool material_channel_roughness_active, Texture material_channel_roughness, bool material_channel_metallic_active, 
                        Texture material_channel_metallic, bool material_channel_normalMap_active, Texture material_channel_normalMap, 
                        bool material_channel_heightMap_active, Texture material_channel_heightMap, bool material_channel_ao_active, 
                        Texture material_channel_ao
                    )
        {
            this->material_painting = material_painting;
            this->solid_painted_texture = solid_painted_texture;
            this->material_channel_albedo_active = material_channel_albedo_active;
            this->material_channel_albedo = material_channel_albedo;
            this->material_channel_roughness_active = material_channel_roughness_active;
            this->material_channel_roughness = material_channel_roughness;
            this->material_channel_metallic_active = material_channel_metallic_active;
            this->material_channel_metallic = material_channel_metallic;
            this->material_channel_normalMap_active = material_channel_normalMap_active;
            this->material_channel_normalMap = material_channel_normalMap;
            this->material_channel_heightMap_active = material_channel_heightMap_active;
            this->material_channel_heightMap = material_channel_heightMap;
            this->material_channel_ao_active = material_channel_ao_active;
            this->material_channel_ao = material_channel_ao;
        }
    };

    PaintingOverData painting_over_data;
    
    /*! @brief This color values will be used for painting (draw mode & bucket mode : directly paint this color, any other modes : Multiply the stroke value with these colors (black : no effect, white : full effect))*/
    ColorBuffer color_buffer;

    /*! @brief Set which textures will be painted*/
    PaintedBuffers painted_buffers;

    MirrorSettings mirror_settings;

    PaintVertexBuffer vertex_buffer;
    
    PointData point;

    /*! @brief 0 : Draw Mode, 1 : Soften Mode, 2 : Smear Mode, 3 : Normal Mode, 4 : Filter Mode, 5 : Vector(empty), 6 : bucket */ 
    int painting_mode;

    Brush stroke_brush;

    DrawMode draw_mode;
    SoftenMode soften_mode;
    SmearMode smear_mode;
    NormalMode normal_mode;
    FilterMode filter_mode;
    BucketMode bucket_mode;

    PaintSettings(){}

    PaintSettings(PaintingOverData painting_over_data, ColorBuffer color_buffer, PaintedBuffers painted_buffers, MirrorSettings mirror_settings,
                    PaintVertexBuffer vertex_buffer, PointData point, int painting_mode, DrawMode draw_mode, SoftenMode soften_mode, 
                    SmearMode smear_mode, NormalMode normal_mode, FilterMode filter_mode, BucketMode bucket_mode, Brush stroke_brush
                )
    {
        this->painting_over_data = painting_over_data;
        this->color_buffer = color_buffer;
        this->painted_buffers = painted_buffers;
        this->mirror_settings = mirror_settings;
        this->vertex_buffer = vertex_buffer;
        this->point = point;
        this->painting_mode = painting_mode;
        this->draw_mode = draw_mode;
        this->soften_mode = soften_mode;
        this->smear_mode = smear_mode;
        this->normal_mode = normal_mode;
        this->filter_mode = filter_mode;
        this->bucket_mode = bucket_mode;
        this->stroke_brush = stroke_brush;
    }

    bool operator==(const PaintSettings paintSettings) const{
        if(this->color_buffer.material != paintSettings.color_buffer.material){return false;}
        if(this->color_buffer.stroke_albedo_color.priv_rgb != paintSettings.color_buffer.stroke_albedo_color.priv_rgb){return false;}
        if(this->color_buffer.stroke_roughness_color != paintSettings.color_buffer.stroke_roughness_color){return false;}
        if(this->color_buffer.stroke_metallic_color != paintSettings.color_buffer.stroke_metallic_color){return false;}
        if(this->color_buffer.stroke_normalMap_color != paintSettings.color_buffer.stroke_normalMap_color){return false;}
        if(this->color_buffer.stroke_heightMap_color != paintSettings.color_buffer.stroke_heightMap_color){return false;}
        if(this->color_buffer.stroke_ao_color != paintSettings.color_buffer.stroke_ao_color){return false;}
        if(this->filter_mode.filter.shader.ID != paintSettings.filter_mode.filter.shader.ID){return false;}
        if(this->mirror_settings.X != paintSettings.mirror_settings.X){return false;}
        if(this->mirror_settings.Y != paintSettings.mirror_settings.Y){return false;}
        if(this->mirror_settings.Z != paintSettings.mirror_settings.Z){return false;}
        if(this->mirror_settings.X_offset != paintSettings.mirror_settings.X_offset){return false;}
        if(this->mirror_settings.Y_offset != paintSettings.mirror_settings.Y_offset){return false;}
        if(this->mirror_settings.Z_offset != paintSettings.mirror_settings.Z_offset){return false;}
        if(this->normal_mode.normal_strength != paintSettings.normal_mode.normal_strength){return false;}
        if(this->painted_buffers.material_painting != paintSettings.painted_buffers.material_painting){return false;}
        if(this->painted_buffers.solid_painted_texture != paintSettings.painted_buffers.solid_painted_texture){return false;}
        if(this->painted_buffers.material_channel_albedo_active != paintSettings.painted_buffers.material_channel_albedo_active){return false;}
        if(this->painted_buffers.material_channel_albedo != paintSettings.painted_buffers.material_channel_albedo){return false;}
        if(this->painted_buffers.material_channel_roughness_active != paintSettings.painted_buffers.material_channel_roughness_active){return false;}
        if(this->painted_buffers.material_channel_roughness != paintSettings.painted_buffers.material_channel_roughness){return false;}
        if(this->painted_buffers.material_channel_metallic_active != paintSettings.painted_buffers.material_channel_metallic_active){return false;}
        if(this->painted_buffers.material_channel_metallic != paintSettings.painted_buffers.material_channel_metallic){return false;}
        if(this->painted_buffers.material_channel_normalMap_active != paintSettings.painted_buffers.material_channel_normalMap_active){return false;}
        if(this->painted_buffers.material_channel_normalMap != paintSettings.painted_buffers.material_channel_normalMap){return false;}
        if(this->painted_buffers.material_channel_heightMap_active != paintSettings.painted_buffers.material_channel_heightMap_active){return false;}
        if(this->painted_buffers.material_channel_heightMap != paintSettings.painted_buffers.material_channel_heightMap){return false;}
        if(this->painted_buffers.material_channel_ao_active != paintSettings.painted_buffers.material_channel_ao_active){return false;}
        if(this->painted_buffers.material_channel_ao != paintSettings.painted_buffers.material_channel_ao){return false;}
        if(this->painting_mode != paintSettings.painting_mode){return false;}
        if(this->painting_over_data.active != paintSettings.painting_over_data.active){return false;}
        if(this->painting_over_data.gray_scale != paintSettings.painting_over_data.gray_scale){return false;}
        if(this->painting_over_data.texture_field_scene != paintSettings.painting_over_data.texture_field_scene){return false;}
        if(this->smear_mode.smear_blur_strength != paintSettings.smear_mode.smear_blur_strength){return false;}
        if(this->smear_mode.smear_transform_strength != paintSettings.smear_mode.smear_transform_strength){return false;}
        if(this->soften_mode.softening_strength != paintSettings.soften_mode.softening_strength){return false;}
        if(this->stroke_brush != paintSettings.stroke_brush){return false;}
        if(this->vertex_buffer.box != paintSettings.vertex_buffer.box){return false;}
        if(this->vertex_buffer.model_mesh != paintSettings.vertex_buffer.model_mesh){return false;}
        if(this->vertex_buffer.paint_model != paintSettings.vertex_buffer.paint_model){return false;}

        return true;
    }
    bool operator!=(const PaintSettings paintSettings) const{
        return !(*this == paintSettings);
    }
};

extern Texture painting_projected_painting_txtr;

/*! @brief Returns true if we're painting*/
bool painting_paint_condition();
/*! @brief Returns true if we're good to paint (mouse left press to paint)*/
bool painting_paintable_condition();

void painting_paint_buffers(PaintSettings settings, bool first_frame, bool last_frame);

std::vector<MirrorSide*> painting_get_selected_mirror_sides(bool mirror_X, bool mirror_Y, bool mirror_Z);

void painting_init_buffers();
void painting_update_buffers(const unsigned int resolution);

PaintSettings get_paint_settings_using_GUI_data(bool* success);

#endif