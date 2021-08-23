#include "normal_map_shader.h"

namespace simple_graphics {

    vec3 NormalMapShader::vertex(int iface, int nthvert)
    {
       
        vec2 uv = model_->uv(iface, nthvert);
        varying_uv_[0][nthvert] = uv[0];
        varying_uv_[1][nthvert] = uv[1];
        vec3 vert = model_->vert(iface,nthvert);
        triangle_.set_col(nthvert, vert);
        varying_normal_.set_col(nthvert, model_->normal(iface, nthvert));
        return  from_homogenous(vertex_matrix_ * embed<4>(vert));
    }


    bool NormalMapShader::fragment(const vec3& bar, SimpleColor& color)
    {
        vec2 uv = varying_uv_ * bar;
        vec3 point = triangle_ * bar;
        float shadow = 1;


        vec3 normal = varying_normal_ * bar;
        normal.normalize();

        vec3& u_vec = varying_uv_[0];
        vec3& v_vec = varying_uv_[1];


        mat<3, 3> A = { triangle_.col(1) - triangle_.col(0), triangle_.col(2) - triangle_.col(0), normal };
        mat<3, 3> A_I = A.invert();

        
        vec3 tangent_basis = model_->normal(uv);

        vec3 i = A_I * vec3(u_vec[1] - u_vec[0], u_vec[2] - u_vec[0], 0);
        vec3 j = A_I * vec3(v_vec[1] - v_vec[0], v_vec[2] - v_vec[0], 0);
        mat<3, 3> basis_change;
        basis_change.set_col(0, i.normalize());
        basis_change.set_col(1, j.normalize());
        basis_change.set_col(2, normal);

        vec3 new_normal = basis_change * tangent_basis;
        new_normal.normalize();




        vec3 shadow_px = from_homogenous(shadow_projection_view_mat_ * embed<4>(point));
        if (shadow_px.x >= 0 && shadow_px.y >= 0 && shadow_px.x < ShadowZBuffer.width && shadow_px.y < ShadowZBuffer.height) {
            float shadow_z = ShadowZBuffer(shadow_px.x, shadow_px.y);
            if (shadow_px.z + 0.1 < shadow_z) {
                shadow = 0.3;
            }
        }
        else {
            shadow = 0.3;
        }
        
        double spec = model_->specular(uv);
        //spec = 35;
        new_normal = from_homogenous(normal_map_matrix_ * embed<4>(new_normal)).normalize();
        vec3 reflection = (new_normal * (new_normal * light_direction_ * 2.f) - light_direction_).normalize();
        double specular = std::pow((std::max)(0.0,  reflection.z), spec);
        double diffuse = (std::max)(0.0, new_normal * light_direction_);
       //specular = 0;

        

        color = SimpleColor(model_->diffuse(uv)) *shadow* (0.95 * specular + 1.3 * diffuse) + 5 * shadow;
        //color = SimpleColor(model_->diffuse(uv));
        
        color.a = 255;
        return false;
    }
}