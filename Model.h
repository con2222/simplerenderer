#pragma once

#include <iostream>
#include <vector>
#include "Geometry.h"

struct Triangle {
    vec<3> a, b, c;
    float z_mid;
    Triangle(vec<3> a, vec<3> b, vec<3> c) : a(a), b(b), c(c) {
        z_mid = (a.z + b.z + c.z) / 3;
    }
};

// v - position, vt - texture coordinate, vn - normal
struct VertIndices {
    int v, t, n; // A single vertex of a triangle
};

struct Face {
    VertIndices corners[3]; // The three vertices that make up a face
};

class Model
{
    std::vector<vec<3>> verts;
    std::vector<vec<3>> tex_coords;
    std::vector<vec<3>> normals;
    std::vector<Face> faces;
public:
    Model(const std::string& fileName);

    void draw_model(struct TGAImage& framebuffer, struct TGAColor color, struct IShader& shader) const;
    void draw_points(struct TGAImage& framebuffer, struct TGAColor color) const;
    vec3 vert(int face, int vert) const;
    vec3 normal(int face, int vert) const;
};