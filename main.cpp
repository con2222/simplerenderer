#include <cmath>
#include <ctime>
#include <cstdlib>
#include <fstream>
#include <string>
#include <sstream>

#include "ModelsNames.h"
#include "Model.h"
#include "Geometry.h"
#include "Color.h"


int main(int argc, char** argv) {
    constexpr int width  = SIZE;
    constexpr int height = SIZE;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    triangle(  7, 45, 35, 100, 45,  60, framebuffer, red);
    triangle_scanline(7, 45, 35, 100, 45, 60, framebuffer, red);
    triangle(120, 35, 90,   5, 45, 110, framebuffer, white);
    triangle_scanline(120, 35, 90, 5, 45, 110, framebuffer, white);
    triangle(115, 83, 80,  90, 85, 120, framebuffer, green);
    triangle_scanline(115, 83, 80, 90, 85, 120, framebuffer, green);

    framebuffer.write_tga_file("framebuffer.tga");
    
    return 0;
}

/*
cmake -Bbuild &&
cmake --build build -j &&
build/tinyrenderer obj/diablo3_pose/diablo3_pose.obj obj/floor.obj*/