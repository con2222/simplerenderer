#pragma once

struct TGAImage;
struct TGAColor;

void line(int ax, int ay, int bx, int by, TGAImage& framebuffer, struct TGAColor color);
void triangle(int ax, int ay, float az, int bx, int by, float bz, int cx, int cy, float cz, TGAImage& framebuffer, float* zbuffer);
void draw_fat_point(int x, int y, TGAImage& framebuffer, TGAColor color, int size = 1);
void triangle_scanline(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color);
void triangle_barycentric_bounding_box(int ax, int ay, float az, int bx, int by, float bz, int cx, int cy, float cz,
    TGAImage& framebuffer, float* zbuffer);