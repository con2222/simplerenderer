#include <cmath>
#include "tgaimage.h"
#include <ctime>
#include <cstdlib>

struct Vec3f
{
    float x, y, z;
};

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

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

int main(int argc, char** argv) {
    constexpr int width  = 64;
    constexpr int height = 64;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    std::srand(std::time({}));
    
    for (int i = 0; i < (1 << 24); i++)
    {
        int ax = rand() % width, ay = rand() % height;
        int bx = rand() % width, by = rand() % height;
        line(ax, ay, bx, by, framebuffer, { static_cast<std::uint8_t>(rand() % 255), 
        static_cast<std::uint8_t>(rand() % 255), 
        static_cast<std::uint8_t>(rand() % 255), 
        static_cast<std::uint8_t>(rand() % 255)  });
    }

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}

/*git clone https://github.com/ssloy/tinyrenderer.git &&
cd tinyrenderer &&
cmake -Bbuild &&
cmake --build build -j &&
build/tinyrenderer obj/diablo3_pose/diablo3_pose.obj obj/floor.obj*/

//2.24 sec
//1.7 sec
//1.8
//1.8