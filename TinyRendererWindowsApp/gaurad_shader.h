#pragma once
#include "SimpleGraphics.h"
#include "model.h"
namespace simple_graphics {
    class GauraudShader :
        public IShader
    {
    public:
        GauraudShader() = delete;
        ~GauraudShader() = default;
        explicit GauraudShader(Model* model, TGAImage* texture, const Vec3f& light_direction) :
            model_(model), texture_(texture), light_direction_(light_direction) {}

        // Inherited via IShader
        virtual Vec3f vertex(int iface, int nthvert) override;
        virtual bool fragment(Vec3f bar, SimpleColor& color) override;
    protected :
        Model* model_;
        TGAImage* texture_;
        Vec3f light_direction_;
        Vec3f varying_intensity_;
        Vec3f varying_texture_[2];
    };
}

