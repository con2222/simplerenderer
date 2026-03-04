#include <string>
#include <ctime>
#include <random>

#include "ModelsNames.h"
#include "Shaders.h"

int main(int argc, char** argv) {
    constexpr int width  = SIZE;
    constexpr int height = SIZE;
    constexpr geom::vec3 eye(1, 1, 3); // camera position
    constexpr geom::vec3 center(0, 0, 0);// camera direction
    constexpr geom::vec3 up(0, 1, 0);  // camera up vector

    std::srand(std::time(0));

    init_viewport(0, 0, width, height);
    init_perspective(norm(eye - center));
    lookat(eye, center, up);
    init_zbuffer(width, height);

    TGAColor background_color = {255, 255, 150, 255};
    TGAImage framebuffer(width, height, TGAImage::RGB);

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            framebuffer.set(i, j, background_color);
        }
    }


    TGAImage zbuffer_image(width, height, TGAImage::GRAYSCALE);

    Model model(DIABLO);


    GouraudShader shader(model, geom::vec3(1, 1, 1));
    MySimpleShader shader1(red, model);
    RandomShader a(model);

    model.draw_model(framebuffer, red, a);
    create_zbuffer_image(zbuffer_image);

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}
