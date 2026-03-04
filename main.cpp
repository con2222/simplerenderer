#include <cmath>
#include <string>

#include "ModelsNames.h"
#include "Model.h"
#include "Color.h"
#include "our_gl.h"

struct MySimpleShader : public IShader {
    TGAColor color;

    MySimpleShader(TGAColor c) : color(c) {}
    virtual std::pair<bool, TGAColor> fragment(const vec<3> bar) const override {
        return {false, color};
    }
};

struct WireframeShader : public IShader {
    virtual std::pair<bool, TGAColor> fragment(const vec<3> bar) const override {
        if (bar.x < 0.1f || bar.y < 0.1f || bar.z < 0.1f) {
            return {false, TGAColor({255, 255, 255, 255})};
        }

        return {true, TGAColor({0, 0, 0, 0})};
    }
};

struct SolidShader : public IShader {
    virtual std::pair<bool, TGAColor> fragment(const vec<3> bar) const override {
        return {false, white};
    }
};

int main(int argc, char** argv) {
    constexpr int width  = SIZE;
    constexpr int height = SIZE;
    constexpr vec3 eye(1, 1, 3); // camera position
    constexpr vec3 center(0, 0, 0);// camera direction
    constexpr vec3 up(0, 1, 0);  // camera up vector

    init_viewport(0, 0, width, height);
    init_perspective(norm(eye - center));
    lookat(eye, center, up);
    init_zbuffer(width, height);
    TGAImage framebuffer(width, height, TGAImage::RGB);
    TGAImage zbuffer_image(width, height, TGAImage::GRAYSCALE);

    MySimpleShader my_simple_shader(red);

    Model model(DIABLO);
    model.draw_model(framebuffer, red, my_simple_shader);
    create_zbuffer_image(zbuffer_image);

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}
