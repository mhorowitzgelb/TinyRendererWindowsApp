#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts_(), faces_() {
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail()) return;
    std::string line;
    while (!in.eof()) {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) {
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v.raw[i];
            verts_.push_back(v);
        } 
        else if (!line.compare(0, 3, "vt ")) {
            iss >> trash;
            iss >> trash;
            Vec2f texture_pt;
            for (int i = 0; i < 2; ++i) iss >> texture_pt.raw[i];
            texture_points_.push_back(texture_pt);
        }
        else if (!line.compare(0, 3, "vn ")) {
            iss >> trash;
            iss >> trash;
            Vec3f v;
            for (int i = 0; i < 3; i++) iss >> v.raw[i];
            vert_normals_.push_back(v);
        }
        else if (!line.compare(0, 2, "f ")) {
            std::vector<FaceIndexes> f;
            int texture_idx, vert_idx, normal_idx;
            iss >> trash;
            while (iss >> vert_idx >> trash >> texture_idx >> trash >> normal_idx) {
                FaceIndexes face;
                face.texture_idx = texture_idx - 1;
                face.vert_idx = vert_idx - 1;
                face.vert_normal_idx = normal_idx - 1;
                f.push_back(face);
            }
            faces_.push_back(f);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << std::endl;
}

Model::~Model() {
}

int Model::nverts() {
    return (int)verts_.size();
}

int Model::nfaces() {
    return (int)faces_.size();
}

std::vector<FaceIndexes> Model::face(int idx) {
    return faces_[idx];
}

Vec3f Model::vert(int i) {
    return verts_[i];
}

