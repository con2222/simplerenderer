#include <cmath>
#include <string>

#include "ModelsNames.h"
#include "Model.h"
#include "Color.h"
#include "our_gl.h"

struct MySimpleShader : public IShader {
    TGAColor color;
    const Model &model;

    virtual vec4 vertex(const int face, const int vert) {
        vec3 v = model.vert(face, vert);                          // current vertex in object coordinates
        vec4 gl_Position = ModelView * vec4{v.x, v.y, v.z, 1.};// in eye coordinates
        return Perspective * gl_Position;                         // in clip coordinates
    }

    MySimpleShader(TGAColor c, const Model &model) : color(c), model(model) {}
    virtual std::pair<bool, TGAColor> fragment(const vec<3> bar) const override {
        return {false, color};
    }
};

struct WireframeShader : public IShader {
    const Model &model;

    virtual vec4 vertex(const int face, const int vert) {
        vec3 v = model.vert(face, vert);                          // current vertex in object coordinates
        vec4 gl_Position = ModelView * vec4{v.x, v.y, v.z, 1.};// in eye coordinates
        return Perspective * gl_Position;                         // in clip coordinates
    }

    virtual std::pair<bool, TGAColor> fragment(const vec<3> bar) const override {
        if (bar.x < 0.1f || bar.y < 0.1f || bar.z < 0.1f) {
            return {false, TGAColor({255, 255, 255, 255})};
        }
        return {true, TGAColor({0, 0, 0, 0})};
    }
};

struct SolidShader : public IShader {
    const Model &model;

    virtual vec4 vertex(const int face, const int vert) {
        vec3 v = model.vert(face, vert);                          // current vertex in object coordinates
        vec4 gl_Position = ModelView * vec4{v.x, v.y, v.z, 1.};// in eye coordinates
        return Perspective * gl_Position;                         // in clip coordinates
    }

    virtual std::pair<bool, TGAColor> fragment(const vec<3> bar) const override {
        return {false, white};
    }
};

struct RandomShader : IShader {
    const Model &model;
    TGAColor color = {};
    vec3 tri[3];  // triangle in eye coordinates

    RandomShader(const Model &m) : model(m) {
    }

    virtual vec4 vertex(const int face, const int vert) {
        vec3 v = model.vert(face, vert);                          // current vertex in object coordinates
        vec4 gl_Position = ModelView * vec4{v.x, v.y, v.z, 1.};
        tri[vert] = gl_Position.xyz();                            // in eye coordinates
        return Perspective * gl_Position;                         // in clip coordinates
    }

    virtual std::pair<bool,TGAColor> fragment(const vec3 bar) const {
        return {false, color};                                    // do not discard the pixel
    }
};

struct GouraudShader : public IShader {
    const Model &model;
    vec3 light_dir;

    double varying_intensity[3];

    GouraudShader(const Model &m, vec3 light) : model(m), light_dir(normalize(light)) {}

    virtual vec4 vertex(int iface, int nthvert) override {

        vec3 v = model.vert(iface, nthvert);

        vec3 normal = model.normal(iface, nthvert);

        varying_intensity[nthvert] = std::max(0.0, dot(normal, light_dir));

        vec4 gl_Position = Perspective * ModelView * vec4(v.x, v.y, v.z, 1.0);

        return gl_Position;
    }

    virtual std::pair<bool, TGAColor> fragment(vec3 bar) const override {

        double intensity = varying_intensity[0]*bar.x + varying_intensity[1]*bar.y + varying_intensity[2]*bar.z;

        TGAColor color = {
            static_cast<uint8_t>(white.bgra[0] * intensity),
            static_cast<uint8_t>(white.bgra[1] * intensity),
            static_cast<uint8_t>(white.bgra[2] * intensity),
            255
        };

        return {false, color};
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

    Model model(DIABLO);


    GouraudShader shader(model, vec3(1, 1, 1));
    MySimpleShader shader1(red, model);
    RandomShader a(model);

    model.draw_model(framebuffer, red, a);
    create_zbuffer_image(zbuffer_image);

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}
