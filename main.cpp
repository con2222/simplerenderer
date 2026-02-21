#include <cmath>
#include "tgaimage.h"

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

void line(int ax, int ay, int bx, int by, TGAImage& framebuffer, TGAColor color)
{
    for (float t = 0.f; t < 1; t+=0.02)
    {
        int x = std::round((1 - t) * ax + t * bx);
        int y = std::round((1 - t) * ay + t * by);
        framebuffer.set(x, y, color);
    }
}


int main(int argc, char** argv) {
    constexpr int width  = 64;
    constexpr int height = 64;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    int ax =  7, ay =  3;
    int bx = 12, by = 37;
    int cx = 62, cy = 53;

    framebuffer.set(ax, ay, white);
    framebuffer.set(bx, by, white);
    framebuffer.set(cx, cy, white);
    
    line(ax, ay, bx, by, framebuffer, blue);
    line(bx, by, cx, cy, framebuffer, red);
    line(cx, cy, ax, ay, framebuffer, yellow);
    line(ax, ay, cx, cy, framebuffer, green);

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}

/*git clone https://github.com/ssloy/tinyrenderer.git &&
cd tinyrenderer &&
cmake -Bbuild &&
cmake --build build -j &&
build/tinyrenderer obj/diablo3_pose/diablo3_pose.obj obj/floor.obj*/