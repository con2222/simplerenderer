#pragma once

#include <vector>

struct TGAImage;
struct TGAColor;

double signed_triangle_area(int x1, int y1, int x2, int y2, int x3, int y3);
void line(int ax, int ay, int bx, int by, TGAImage& framebuffer, struct TGAColor color);
void draw_fat_point(int x, int y, TGAImage& framebuffer, TGAColor color, int size = 1);
void triangle_scanline(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color);
void triangle_barycentric_bounding_box(int ax, int ay, float az, int bx, int by, float bz, int cx, int cy, float cz,
    TGAImage& framebuffer, const struct IShader& shader);