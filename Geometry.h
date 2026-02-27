#pragma once

struct TGAImage;
struct TGAColor;

void line(int ax, int ay, int bx, int by, TGAImage& framebuffer, struct TGAColor color);
void triangle(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy, int cz, TGAImage& framebuffer, TGAImage& zbuffer);
void draw_fat_point(int x, int y, TGAImage& framebuffer, TGAColor color, int size = 1);
void triangle_scanline(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color);
double signed_triangle_area(int x1, int y1, int x2, int y2, int x3, int y3);
void triangle_barycentric_bounding_box(int ax, int ay, int az, int bx, int by, int bz, int cx, int cy, int cz, 
    TGAImage& framebuffer, TGAImage& zbuffer);