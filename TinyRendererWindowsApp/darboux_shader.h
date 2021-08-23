#pragma once
#include "SimpleGraphics.h"
#include "geometry.h"
#include "model.h"
#include "tgaimage.h"
#include <initializer_list>

namespace simple_graphics {
    class DarbouxShader :
        public IShader
    {
    public:

        DarbouxShader(Model* model, TGAImage* texture, TGAImage* darboux_map, const vec<3>& light_direction) :
        model_(model),
            texture_(texture),
            darboux_map_(darboux_map)
        {
            //light_direction_ = Vec3f(Projection * light_direction.AsHomogenous()).normalize();
            light_direction_ = light_direction;
            vertex_matrix_ = Viewport * Projection * ModelView;
            normal_map_matrix_ = (Projection * ModelView).invert_transpose();
        }


        // Inherited via IShader
        virtual vec<3> vertex(int iface, int nthvert) override;

        virtual bool fragment(const vec<3>& bar, SimpleColor& color) override;
        vec<3> p;
        vec<3> i;
        vec<3> j;
        vec<3> n;
    protected:
        mat<2,3> varying_uv_;
        mat<3,3> varying_normal_;
        mat<3,3> vertex_;
        Model* model_;
        TGAImage* texture_;
        TGAImage* darboux_map_;
        vec<3> light_direction_;
        mat<4,4> normal_map_matrix_;
        mat<4,4> vertex_matrix_;
    };
}

