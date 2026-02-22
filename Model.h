#pragma once

#include <iostream>
#include <vector>

struct Vec3f
{
    float x, y, z;
};

// позиция, vt текстура, vn нормаль
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

    void draw_model(struct TGAImage& framebuffer, int width, int height) const;
};