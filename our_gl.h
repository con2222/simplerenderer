#pragma once
#include "tgaimage.h"
#include "Geometry.h"

extern geom::matrix<4,4> ModelView, Viewport, Perspective;
extern std::vector<double> zbuffer;

constexpr geom::vec3 eye(1., 1., 3.); // camera position
constexpr geom::vec3 center(0., 0., 0.);// camera direction
constexpr geom::vec3 up(0., 1., 0.);  // camera up vector

void lookat(geom::vec3 eye, geom::vec3 center, geom::vec3 up);
void init_perspective(const double f);
void init_viewport(const int x, const int y, const int w, const int h);
void init_zbuffer(const int width, const int height);
void init_zbuffer(const int width, const int height);

void create_zbuffer_image(TGAImage& zbuffer_image);

struct IShader {
    virtual ~IShader() = default;
    virtual geom::vec4 vertex(int iface, int nthvert) = 0;
    virtual std::pair<bool, TGAColor> fragment(const geom::vec3 bar) const = 0;
};