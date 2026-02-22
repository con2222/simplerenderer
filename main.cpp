#include <cmath>
#include "tgaimage.h"
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>
#include <iostream>

#define SIZE 2048
#define POINT_SIZE 1

constexpr TGAColor white   = {255, 255, 255, 255}; // attention, BGRA order
constexpr TGAColor green   = {  0, 255,   0, 255};
constexpr TGAColor red     = {  0,   0, 255, 255};
constexpr TGAColor blue    = {255, 128,  64, 255};
constexpr TGAColor yellow  = {  0, 200, 255, 255};

constexpr TGAColor red1     = {  12,  12, 249, 255};

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

std::ostream& operator<<(std::ostream& s, const Vec3f& v) {
    s << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
    return s;
}

std::ostream& operator<<(std::ostream& s, const Face& face) {

    for (int i = 0; i < 3; i++) {
        s << face.corners[i].v << "/" << face.corners[i].t << "/" << face.corners[i].n;
        if (i != 2) {
            s << " ";
        }
    }
    return s;
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
        
        /*y += (by > ay ? 1 : -1) * (ierror > bx - ax);
        ierror -= 2 * (bx-ax)   * (ierror > bx - ax);*/
    }
}

void draw_fat_point(int x, int y, TGAImage& framebuffer, TGAColor color, int size = 1)
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

int main(int argc, char** argv) {
    constexpr int width  = SIZE;
    constexpr int height = SIZE;
    TGAImage framebuffer(width, height, TGAImage::RGB);
    
    
    std::vector<Vec3f> points;
    std::vector<Face> faces;
    
    std::string line1;
    std::string digits = "-0123456789";

    float xt, yt, zt;
    int x, y, z;
    int v, t, n;
    
    
    std::ifstream in("obj/diablo3_pose/diablo3_pose.obj");
    if (in.is_open())
    {
        while (std::getline(in, line1)) {
            if (line1[0] == 'v' && line1[1] == ' ') {
                size_t index = line1.find_first_of(digits);

                if (index != std::string::npos) {
                    line1 = line1.substr(index);
                }
                std::istringstream coords(line1);
                coords >> xt >> yt >> zt;

                points.push_back(Vec3f({xt, yt, zt}));
            } else if (line1[0] == 'f' && line1[1] == ' ') {
                size_t index = line1.find_first_of(digits);

                if (index != std::string::npos) {
                    line1 = line1.substr(index);
                }
                

                std::istringstream iss(line1);
                std::string str;

                Face face = Face();
                int i = 0;
                while (iss >> str && i < 3) {

                    int v, t, n;

                    int result = sscanf(str.c_str(), "%d/%d/%d", &v, &t, &n);

                    face.corners[i].v = v - 1;
                    face.corners[i].t = t - 1;
                    face.corners[i].n = n - 1;
                    
                    i++;
                }

                faces.push_back(face);
                
            }
        }
    }
    in.close();
    

    std::srand(std::time({}));
    
    /*for (int i = 0; i < (1 << 24); i++)
    {
        int ax = rand() % width, ay = rand() % height;
        int bx = rand() % width, by = rand() % height;
        line(ax, ay, bx, by, framebuffer, { static_cast<std::uint8_t>(rand() % 255),
        static_cast<std::uint8_t>(rand() % 255), 
        static_cast<std::uint8_t>(rand() % 255), 
        static_cast<std::uint8_t>(rand() % 255)});
    }*/
    
    for (auto& face : faces)
    {
        int a = face.corners[0].v;
        int b = face.corners[1].v;
        
        float x = points[a].x;
        float y = points[a].y;
        
        int ax = (x + 1.0f) * width / 2.0f;
        int ay = (y + 1.0f) * height / 2.0f;
        
        x = points[b].x;
        y = points[b].y;
        
        int bx = (x + 1.0f) * width / 2.0f;
        int by = (y + 1.0f) * height / 2.0f;
        
        line(ax, ay, bx, by, framebuffer, red1);
    }
    
    for (auto& p : points)
    {
        int px = (p.x + 1.0f) * width / 2.0f;
        int py = (p.y + 1.0f) * height / 2.0f;
        
        draw_fat_point(px, py, framebuffer, white, POINT_SIZE);
    }

    framebuffer.write_tga_file("framebuffer.tga");
    
    /*int x0 = (v0.x + 1.0) * width / 2.0;
    int y0 = (v0.y + 1.0) * height / 2.0;
    int x1 = (v1.x + 1.0) * width / 2.0;
    int y1 = (v1.y + 1.0) * height / 2.0;*/
    
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