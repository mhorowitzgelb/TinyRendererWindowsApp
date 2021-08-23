#ifndef __MODEL_H__
#define __MODEL_H__
#include <vector>
#include <string>
#include "geometry.h"
#include "tgaimage.h"

class Model{
public:
    virtual int nverts() const = 0;
    virtual int nfaces() const = 0;
    virtual vec3 normal(const int iface, const int nthvert) const = 0;  // per triangle corner normal vertex
    virtual vec3 normal(const vec2& uv) const = 0;                      // fetch the normal vector from the normal map texture
    virtual vec3 vert(const int i) const = 0;
    virtual vec3 vert(const int iface, const int nthvert) const = 0;
    virtual vec2 uv(const int iface, const int nthvert) const = 0;
    virtual TGAColor diffuse(const vec2& uv) const = 0;
    virtual double specular(const vec2& uv) const = 0;
};

class FileModel : public Model{
private:
    std::vector<vec3> verts_;     // array of vertices
    std::vector<vec2> uv_;        // array of tex coords
    std::vector<vec3> norms_;     // array of normal vectors
    std::vector<int> facet_vrt_;
    std::vector<int> facet_tex_;  // indices in the above arrays per triangle
    std::vector<int> facet_nrm_;
    TGAImage diffusemap_;         // diffuse color texture
    TGAImage normalmap_;          // normal map texture
    TGAImage specularmap_;        // specular map texture
    void load_texture(const std::string filename, const std::string suffix, TGAImage& img);
public:
    FileModel(const std::string filename);
    virtual int nverts() const;
    virtual int nfaces() const;
    virtual vec3 normal(const int iface, const int nthvert) const;  // per triangle corner normal vertex
    virtual vec3 normal(const vec2& uv) const;                      // fetch the normal vector from the normal map texture
    virtual vec3 vert(const int i) const;
    virtual vec3 vert(const int iface, const int nthvert) const;
    virtual vec2 uv(const int iface, const int nthvert) const;
    virtual TGAColor diffuse(const vec2& uv) const;
    virtual double specular(const vec2& uv) const;
};
#endif //__MODEL_H__
