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

// v позиция, vt текстура, vn нормаль
struct VertIndices {
    int v, t, n; // Один "угол" треугольника
};


struct Face {
    VertIndices corners[3]; // Три угла
};

class Model
{
    std::vector<vec<3>> verts;
    std::vector<vec<3>> tex_coords;
    std::vector<vec<3>> normals;
    std::vector<Face> faces;
public:
    Model(const std::string& fileName);

    void draw_model(struct TGAImage& framebuffer, float* zbuffer, int width, int height, struct TGAColor color) const;
    void painters_algorithm_render(TGAImage& framebuffer, float* zbuffer, int width, int height, TGAColor color); // draw triangles from back to front
};