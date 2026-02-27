#pragma once

#include <iostream>
#include <vector>

struct Vec3f
{
    float x, y, z;
};

struct Triangle {
    Vec3f a, b, c;
    float z_mid;
    Triangle(Vec3f a, Vec3f b, Vec3f c) : a(a), b(b), c(c) {
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
    std::vector<Vec3f> verts;
    std::vector<Vec3f> tex_coords;
    std::vector<Vec3f> normals;
    std::vector<Face> faces;
public:
    Model(std::string fileName);

    void draw_model(struct TGAImage& framebuffer, TGAImage& zbuffer, int width, int height, struct TGAColor color) const;
    void painters_algorithm_render(TGAImage& framebuffer, TGAImage& zbuffer, int width, int height, TGAColor color); // draw triangles from back to front
};