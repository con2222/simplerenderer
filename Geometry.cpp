#include "Geometry.h"
#include "color.h"
#include <cmath>
#include <algorithm>


void line(int ax, int ay, int bx, int by, TGAImage& framebuffer, TGAColor color)
{
    bool steep = std::abs(ax - bx) < std::abs(ay - by);
    if (steep)
    {
        std::swap(ax, ay);
        std::swap(bx, by);
    }
    if (ax > bx)
    {
        std::swap(ax, bx);
        std::swap(ay, by);
    }
    int y = ay;
    int ierror = 0;
    int slope = std::abs(by - ay) * 2;
    for (int x = ax; x <= bx; x++)
    {
        if (steep)
        {
            framebuffer.set(y, x, color);
        } else
        {
            framebuffer.set(x, y, color);
        }
        /* чтобы убрать деление умножаем угловой коэфф на dx, а для 0.5f в условии домножаем на 2 */
        ierror += slope;
        if (ierror > bx - ax)
        {
            y += by > ay ? 1 : -1;
            ierror -= 2 * (bx - ax);
        }
        
        /*y += (by > ay ? 1 : -1) * (ierror > bx - ax);
        ierror -= 2 * (bx-ax)   * (ierror > bx - ax);*/
    }
}

void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color) {
    line(ax, ay, bx, by, framebuffer, color);
    line(bx, by, cx, cy, framebuffer, color);
    line(cx, cy, ax, ay, framebuffer, color);
}

void draw_fat_point(int x, int y, TGAImage& framebuffer, TGAColor color, int size)
{
    if (size == 0) return;
    for (int i = 0; i <= size; i++)
    {
        for (int j = -size; j <= size; j++)
        {
            int px = x + i;
            int py = y + j;
            
            if (px >= 0 && px < framebuffer.width() && py >= 0 && py < framebuffer.height())
            {
                framebuffer.set(px, py, color);
            }
            
        }
    }
}