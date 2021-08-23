#include "darboux_shader.h"
/*
namespace simple_graphics {
    vec<3> DarbouxShader::vertex(int iface, int nthvert)
    {
        FaceIndexes face = model_->face(iface)[nthvert];
        vec<2> uv = model_->texture(face.texture_idx);
        varying_uv_.set_col(nthvert, uv);
        varying_normal_.set_col(nthvert,model_->vert_normal(face.vert_normal_idx));
        vertex_[nthvert] = model_->vert(face.vert_idx);
        return from_homogenous(vertex_matrix_ * embed<4>(vertex_[nthvert]));
    }

   

    bool DarbouxShader::fragment(const vec<3>& bar, SimpleColor& color)
    {
        vec2 uv = varying_uv_ * bar;
        int u = std::round(uv[0]* texture_->get_width());
        int v = std::round(uv[1] *texture_->get_height());
      
        vec3 normal = varying_normal_* bar;
        normal.normalize();
        
        vec3& u_vec = varying_uv_[0];
        vec3& v_vec = varying_uv_[1];  
      
            vertex_[0] = Vec3f(0, 0, 0);
        vertex_[1] = Vec3f(0, 1, 0);
        vertex_[2] = Vec3f(1, 0, 0);
        Vec3f u_vec = Vec3f(0, 0, 1);
        Vec3f v_vec = Vec3f(0, 1, 0);

        //Vec3f normal(0, 0, 1);

        mat<3,3> A = { vertex_[1] - vertex_[0], vertex_[2] - vertex_[0], normal };
        mat<3, 3> A_I = A.invert();
        
        //Vec3f tangent_basis = Vec3f(1,0,0);
        //vec3 tangent_basis =vec<3>(0,0,1);
       TGAColor tga_tangent = darboux_map_->get(u, v);
       vec3 tangent_basis(tga_tangent[2], tga_tangent[1], tga_tangent[0]);
       tangent_basis = tangent_basis / (255 / 2) -  vec3(1,1,1);
       //tangent_basis = vec3(0, 0, 1);

        i = A_I * vec3(u_vec[1] - u_vec[0], u_vec[2] - u_vec[0], 0);
        j = A_I * vec3(v_vec[1] - v_vec[0], v_vec[2] - v_vec[0], 0);
        n = normal;
        p = vertex_.transpose() * bar;
        mat<3,3> basis_change;
        basis_change.set_col(0, i.normalize());
        basis_change.set_col(1, j.normalize());
        basis_change.set_col(2, normal);

        float dot1 = i * j;
        float dot2 = j * n;
        float dot3 = i * n;

        vec3 new_normal = basis_change * tangent_basis;
        new_normal.normalize();



        //float intensity = 0.6* (std::max)(0.f, Vec3f(normal_map_matrix_ * normal.AsHomogenous()).normalize() * light_direction_);
        double intensity = (std::max)(0.0 , new_normal*light_direction_);
        //float intensity = 0.3;
        color = SimpleColor(texture_->get(u, v)) * intensity;
        return false;
    }
*/