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

double signed_triangle_area(int x1, int y1, int x2, int y2, int x3, int y3) {
    return 0.5 * (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));
}

void triangle_barycentric_bounding_box(int ax, int ay, int bx, int by, int cx, int cy, TGAImage& framebuffer, TGAColor color) {
    int bbminx = std::min(std::min(ax, bx), cx);
    int bbminy = std::min(std::min(ay, by), cy);
    int bbmaxx = std::max(std::max(ax, bx), cx);
    int bbmaxy = std::max(std::max(ay, by), cy);
    double total_area = signed_triangle_area(ax, ay, bx, by, cx, cy);
    if (total_area < 1) return;

    #pragma omp parallel for

    for (int x = bbminx; x <= bbmaxx; x++) {
        for (int y = bbminy; y <= bbmaxy; y++) {
            double alpha = signed_triangle_area(x, y, bx, by, cx, cy) / total_area;
            double beta = signed_triangle_area(x, y, cx, cy, ax, ay) / total_area;   
            double gamma = signed_triangle_area(x, y, ax, ay, bx, by) / total_area;
            if (alpha < 0 || beta < 0 || gamma < 0) continue;
            framebuffer.set(x, y, color);
        }
    }
}

void triangle(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color) {
    line(ax, ay, bx, by, framebuffer, color);
    line(bx, by, cx, cy, framebuffer, color);
    line(cx, cy, ax, ay, framebuffer, color);

    triangle_barycentric_bounding_box(ax, ay, bx, by, cx, cy, framebuffer, color);
}

void triangle_scanline(int ax, int ay, int bx, int by, int cx, int cy, TGAImage &framebuffer, TGAColor color) {
    int low_y = ay, mid_y = by, high_y = cy;
    int low_x = ax, mid_x = bx, high_x = cx;

    if (low_y > mid_y) { std::swap(low_y, mid_y); std::swap(low_x, mid_x); }
    if (low_y > high_y) { std::swap(low_y, high_y); std::swap(low_x, high_x); }
    if (mid_y > high_y) { std::swap(mid_y, high_y); std::swap(mid_x, high_x); }

    for (int y = low_y; y <= mid_y; y++) {
        if (low_y == mid_y) continue;
        
        int x1 = low_x + ((mid_x - low_x) * (y - low_y)) / (mid_y - low_y);
        int x2 = low_x + ((high_x - low_x) * (y - low_y)) / (high_y - low_y);
        

        if (x1 > x2) std::swap(x1, x2);

        for (int x = std::min(x1, x2); x < std::max(x2, x1); x++) {
            framebuffer.set(x, y, color);
        }
    }

    for (int y = mid_y + 1; y <= high_y; y++) {
        if (mid_y == high_y) continue;
            
        int x1 = mid_x + ((high_x - mid_x) *(y - mid_y)) / (high_y - mid_y);
        int x2 = low_x + ((high_x - low_x) * (y - low_y)) / (high_y - low_y);

        if (x1 > x2) std::swap(x1, x2);

        for (int x = std::min(x1, x2); x < std::max(x2, x1); x++) {
            framebuffer.set(x, y, color);
        }
    }
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