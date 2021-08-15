#include "normal_map_shader.h"

namespace simple_graphics {
    Vec3f NormalMapShader::vertex(int iface, int nthvert)
    {
        FaceIndexes face = model_->face(iface)[nthvert];
        Vec2f uv = model_->texture(face.texture_idx);
        varying_texture_[0][nthvert] = uv[0];
        varying_texture_[1][nthvert] = uv[1];


        return Vec3f( vertex_matrix_ * model_->vert(face.vert_idx).AsHomogenous());
    }

    bool NormalMapShader::fragment(Vec3f bar, SimpleColor& color)
    {
        int u = std::round(bar * varying_texture_[0] * texture_->get_width());
        int v = std::round(bar * varying_texture_[1] * texture_->get_height());
        Vec3f normal_vec = Vec3f(normal_map_->get(u, v));
        int spec = spec_map_->get(u, v).bgra[0];
        //Vec3f normal_vec(0, 0, 1);
        normal_vec = Vec3f(normal_map_matrix_ * normal_vec.AsHomogenous()).normalize();
        Vec3f reflection = (normal_vec * (normal_vec * light_direction_ * 2.f) - light_direction_).normalize();
        float specular = std::pow((std::max)(0.f,  reflection.z), spec);
        float diffuse = (std::max)(0.f, normal_vec * light_direction_);
        //diffuse = 1;
        //specular = 0;

        color = SimpleColor(texture_->get(u, v));
        int ambient = 0;
        color.r = (std::min)(255, (int)(ambient + color.r *  (2*specular + diffuse)));
        color.g = (std::min)(255, (int)(ambient + color.g * ( 2*specular + diffuse)));;
        color.b = (std::min)(255, (int)(ambient + color.b * ( 2*specular + diffuse)));;
        color.a = 255;
        return false;
    }
}