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
        NormalMapShader(SimpleGraphics* simple_graphics, const mat<4, 4>& shadow_projection_view_mat, const mat<4,4>& projection_view_mat, Model* model, const vec3& light_direction) :
            simple_graphics_(simple_graphics),shadow_projection_view_mat_(shadow_projection_view_mat),model_(model), projection_view_mat_(projection_view_mat){
            vertex_matrix_ = Viewport * projection_view_mat;
            normal_map_matrix_ = (projection_view_mat).invert_transpose();
            light_direction_ = from_homogenous(projection_view_mat_* embed<4>(light_direction)).normalize();
        }
        virtual vec3 vertex(int iface, int nthvert) override;
        virtual bool fragment(const vec3& bar, SimpleColor& color) override;
    protected:
        mat<4, 4> shadow_projection_view_mat_;
        mat<4, 4> projection_view_mat_;
        SimpleGraphics* simple_graphics_;
        Model* model_;
        mat<2,3> varying_uv_;
        mat<3,3> varying_normal_;
        mat<3, 3> triangle_;
        mat<4,4> vertex_matrix_;
        mat<4,4> normal_map_matrix_;
        vec3 light_direction_;
    };
}

