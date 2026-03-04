#pragma once
#include "tgaimage.h"
#include "Geometry.h"

extern matrix<4,4> ModelView, Viewport, Perspective;
extern std::vector<double> zbuffer;

void lookat(vec3 eye, vec3 center, vec3 up);
void init_perspective(const double f);
void init_viewport(const int x, const int y, const int w, const int h);
void init_zbuffer(const int width, const int height);
void init_zbuffer(const int width, const int height);

void create_zbuffer_image(TGAImage& zbuffer_image);

struct IShader {
    virtual ~IShader() = default;
    virtual std::pair<bool, TGAColor> fragment(const vec3 bar) const = 0;
};