#include "rasterizer.h"
#include "filesystem"
#include <algorithm>
#include <cmath>
#include "ModelsNames.h"
#include "Geometry.h"
#include "color.h"
#include "our_gl.h"

namespace fs = std::filesystem;

double signed_triangle_area(int x1, int y1, int x2, int y2, int x3, int y3) {
    return 0.5 * (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));
}

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
    }
}

void triangle_barycentric_bounding_box(int ax, int ay, float az, int bx, int by, float bz, int cx, int cy, float cz, TGAImage& framebuffer, const IShader& shader) {
    int bbminx = std::max(0, std::min(std::min(ax, bx), cx));
    int bbminy = std::max(0, std::min(std::min(ay, by), cy));
    int bbmaxx = std::min(framebuffer.width() - 1, std::max(std::max(ax, bx), cx));
    int bbmaxy = std::min(framebuffer.height() - 1, std::max(std::max(ay, by), cy));
    double total_area = signed_triangle_area(ax, ay, bx, by, cx, cy);

    if (total_area < 1) return;

    #pragma omp parallel for

    for (int x = bbminx; x <= bbmaxx; x++) {
        for (int y = bbminy; y <= bbmaxy; y++) {
            double alpha = signed_triangle_area(x, y, bx, by, cx, cy) / total_area;
            double beta = signed_triangle_area(x, y, cx, cy, ax, ay) / total_area;
            double gamma = signed_triangle_area(x, y, ax, ay, bx, by) / total_area;

            if (alpha < 0 || beta < 0 || gamma < 0) continue;
            double z = alpha * az + beta * bz + gamma * cz;
            int idx = x + y * framebuffer.width();
            vec<3> bc = {static_cast<float>(alpha), static_cast<float>(beta), static_cast<float>(gamma)};
            auto [discard, color] = shader.fragment(bc);

            if (discard) continue;

            if (zbuffer[idx] < z) {
                zbuffer[idx] = z;
                framebuffer.set(x, y, color);
            }
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