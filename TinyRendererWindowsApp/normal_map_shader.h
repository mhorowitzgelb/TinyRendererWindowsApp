#pragma once
#include "SimpleGraphics.h"
#include "model.h"
#include "tgaimage.h"
#include "geometry.h"
namespace simple_graphics {
    class NormalMapShader :
        public IShader
    {
    public:
        NormalMapShader(Model* model, TGAImage* texture, TGAImage* normal_map, TGAImage* spec_map, const Vec3f& light_direction) : 
            model_(model), texture_(texture), normal_map_(normal_map), spec_map_(spec_map){
            vertex_matrix_ = Viewport * Projection * ModelView;
            normal_map_matrix_ = (Projection).transpose().inverse();
            light_direction_ = Vec3f(Projection*ModelView* light_direction.AsHomogenous()).normalize();
        }
        virtual Vec3f vertex(int iface, int nthvert) override;
        virtual bool fragment(Vec3f bar, SimpleColor& color) override;
    protected:
        TGAImage* normal_map_;
        TGAImage* texture_;
        TGAImage* spec_map_;
        Model* model_;
        Vec3f varying_texture_[2];
        Vec3f varying_normal_map_[2];
        Matrix vertex_matrix_;
        Matrix normal_map_matrix_;
        Vec3f light_direction_;
    };
}

