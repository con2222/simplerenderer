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

    Model model(HEAD);

    model.draw_model(framebuffer, width, height);

    framebuffer.write_tga_file("framebuffer.tga");
    
    return 0;
}

/*git clone https://github.com/ssloy/tinyrenderer.git &&
cd tinyrenderer &&
cmake -Bbuild &&
cmake --build build -j &&
build/tinyrenderer obj/diablo3_pose/diablo3_pose.obj obj/floor.obj*/