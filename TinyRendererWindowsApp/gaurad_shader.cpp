#include "gaurad_shader.h"

Vec3f simple_graphics::GauraudShader::vertex(int iface, int nthvert)
{
    FaceIndexes face = model_->face(iface)[nthvert];
    varying_intensity_[nthvert] = (std::max)(0.f, model_->vert_normal(face.vert_idx).normalize() * light_direction_);
    Vec2f uv = model_->texture(face.texture_idx);
    varying_texture_[0][nthvert] = uv[0];
    varying_texture_[1][nthvert] = uv[1];
    return Vec3f(Viewport* Projection* ModelView* model_->vert(face.vert_idx).AsHomogenous());
}

bool simple_graphics::GauraudShader::fragment(Vec3f bar, SimpleColor& color)
{
    float intensity = bar * varying_intensity_;
    int u = std::round(bar * varying_texture_[0] * texture_->get_width());
    int v = std::round(bar * varying_texture_[1] * texture_->get_height());
    color = SimpleColor(texture_->get(u, v));
    color.r *= intensity;
    color.g *= intensity;
    color.b *= intensity;
    color.a = 255;
    return false;
}
