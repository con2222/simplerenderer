#pragma once

struct TGAImage;
struct TGAColor;

void line(int ax, int ay, int bx, int by, TGAImage& framebuffer, struct TGAColor color);
void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color);
void draw_fat_point(int x, int y, TGAImage& framebuffer, TGAColor color, int size = 1);
void triangle_scanline(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color);