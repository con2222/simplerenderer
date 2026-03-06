#include <string>
#include <ctime>
#include <random>

#include "ModelsNames.h"
#include "Shaders.h"

int main(int argc, char** argv) {
    constexpr int width  = SIZE;
    constexpr int height = SIZE;

    std::srand(std::time(0));

    init_viewport(0, 0, width, height);
    init_perspective(norm(eye - center));
    lookat(eye, center, up);
    init_zbuffer(width, height);

    TGAColor background_color = black;
    TGAImage framebuffer(width, height, TGAImage::RGB);

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            framebuffer.set(i, j, background_color);
        }
    }

    geom::vec3 l = normalize(geom::vec3(-1, 1, 1));


    TGAImage zbuffer_image(width, height, TGAImage::GRAYSCALE);

    Model head(HEAD);
    Model eyes(HEAD_EYE);

    NormalMappingShader head_shader(l, head);
    head.draw_model(framebuffer,   head_shader);

    NormalMappingShader eye_shader(l, eyes);
    eyes.draw_model(framebuffer, eye_shader);
    //create_zbuffer_image(zbuffer_image);

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}