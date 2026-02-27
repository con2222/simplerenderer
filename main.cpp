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
    TGAImage zbuffer(width, height, TGAImage::GRAYSCALE);

    int ax = 17, ay =  4, az =  13;
    int bx = 55, by = 39, bz = 128;
    int cx = 23, cy = 59, cz = 255;

    //triangle(ax, ay, az, bx, by, bz, cx, cy, cz, framebuffer);

    Model model(DIABLO);
    model.draw_model(framebuffer, zbuffer, width, height, red);

    framebuffer.write_tga_file("framebuffer.tga");
    zbuffer.write_tga_file("zbuffer.tga");
    return 0;
}

/*
cmake -Bbuild &&
cmake --build build -j &&
build/tinyrenderer obj/diablo3_pose/diablo3_pose.obj obj/floor.obj*/