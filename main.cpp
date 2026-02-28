#include <cmath>
#include <string>

#include "ModelsNames.h"
#include "Model.h"
#include "Color.h"

void create_zbuffer_image(TGAImage& zbuffer_image, float* zbuffer) {
    int width = zbuffer_image.width();
    int height = zbuffer_image.height();

    for (int i = 0; i < width * height; i++) {
        if (zbuffer[i] != -std::numeric_limits<float>::max()) {

            int color = (zbuffer[i] + 1.0f) * 255/2.f;

            color = std::max(0, std::min(255, color));

            zbuffer_image.set(i % width, i / width, {static_cast<unsigned char>(color)});
        }
    }

    zbuffer_image.write_tga_file("zbuffer.tga");
}

int main(int argc, char** argv) {
    constexpr int width  = SIZE;
    constexpr int height = SIZE;
    TGAImage framebuffer(width, height, TGAImage::RGB);
    TGAImage zbuffer_image(width, height, TGAImage::GRAYSCALE);
    float *zbuffer = new float[width * height];

    for (int i = 0; i < width * height; i++) {
        zbuffer[i] = -std::numeric_limits<float>::max();
    }

    Model model(DIABLO);
    model.draw_model(framebuffer, zbuffer, width, height, red);
    create_zbuffer_image(zbuffer_image, zbuffer);



    delete[] zbuffer;
    framebuffer.write_tga_file("framebuffer.tga");


    return 0;
}