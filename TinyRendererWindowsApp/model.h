#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"

struct FaceIndexes {
	int vert_idx;
	int vert_normal_idx;
	int texture_idx;
};

class Model {
private:
	std::vector<Vec3f> verts_;
	std::vector<Vec2f> texture_points_;
	std::vector<Vec3f> vert_normals_;
	std::vector<std::vector<FaceIndexes>> faces_;
public:
	Model(const char *filename);
	~Model();
	int nvert_normals() { return vert_normals_.size(); }
	int ntexture_points() { return texture_points_.size(); }
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	std::vector<FaceIndexes> face(int idx);
	Vec3f vert_normal(int i) { return vert_normals_[i]; }
	Vec2f texture(int i) { return texture_points_[i]; }
	
};

#endif //__MODEL_H__
