#pragma once

#include "Geometry.h"
#include "tgaimage.h"
#include "our_gl.h"
#include "color.h"
#include "Model.h"

struct MySimpleShader : public IShader {
    TGAColor color;
    const Model &model;

    virtual geom::vec4 vertex(const int face, const int vert) {
        geom::vec3 v = model.vert(face, vert);                          // current vertex in object coordinates
        geom::vec4 gl_Position = ModelView * geom::vec4{v.x, v.y, v.z, 1.};// in eye coordinates
        return Perspective * gl_Position;                         // in clip coordinates
    }

    MySimpleShader(TGAColor c, const Model &model) : color(c), model(model) {}
    virtual std::pair<bool, TGAColor> fragment(const geom::vec<3> bar) const override {
        return {false, color};
    }
};

struct WireframeShader : public IShader {
    const Model &model;

    virtual geom::vec4 vertex(const int face, const int vert) {
        geom::vec3 v = model.vert(face, vert);                          // current vertex in object coordinates
        geom::vec4 gl_Position = ModelView * geom::vec4{v.x, v.y, v.z, 1.};// in eye coordinates
        return Perspective * gl_Position;                         // in clip coordinates
    }

    virtual std::pair<bool, TGAColor> fragment(const geom::vec<3> bar) const override {
        if (bar.x < 0.1f || bar.y < 0.1f || bar.z < 0.1f) {
            return {false, TGAColor({255, 255, 255, 255})};
        }
        return {true, TGAColor({0, 0, 0, 0})};
    }
};

struct SolidShader : public IShader {
    const Model &model;

    virtual geom::vec4 vertex(const int face, const int vert) {
        geom::vec3 v = model.vert(face, vert);                          // current vertex in object coordinates
        geom::vec4 gl_Position = ModelView * geom::vec4{v.x, v.y, v.z, 1.};// in eye coordinates
        return Perspective * gl_Position;                         // in clip coordinates
    }

    virtual std::pair<bool, TGAColor> fragment(const geom::vec<3> bar) const override {
        return {false, white};
    }
};

struct RandomShader : IShader {
    const Model &model;
    TGAColor color = {};
    geom::vec3 tri[3];  // triangle in eye coordinates

    RandomShader(const Model &m) : model(m) {
    }

    virtual geom::vec4 vertex(const int face, const int vert) {
        geom::vec3 v = model.vert(face, vert);                          // current vertex in object coordinates
        geom::vec4 gl_Position = ModelView * geom::vec4{v.x, v.y, v.z, 1.};
        tri[vert] = gl_Position.xyz();                            // in eye coordinates
        return Perspective * gl_Position;                         // in clip coordinates
    }

    virtual std::pair<bool,TGAColor> fragment(const geom::vec3 bar) const {
        return {false, color};                                    // do not discard the pixel
    }
};

struct GouraudShader : public IShader {
    const Model &model;
    geom::vec3 light_dir;

    double varying_intensity[3];

    GouraudShader(const Model &m, geom::vec3 light) : model(m), light_dir(geom::normalize(light)) {}

    virtual geom::vec4 vertex(int iface, int nthvert) override {

        geom::vec3 v = model.vert(iface, nthvert);

        geom::vec3 normal = model.normal(iface, nthvert);

        varying_intensity[nthvert] = std::max(0.0, dot(normal, light_dir));

        geom::vec4 gl_Position = Perspective * ModelView * geom::vec4(v.x, v.y, v.z, 1.0);

        return gl_Position;
    }

    virtual std::pair<bool, TGAColor> fragment(geom::vec3 bar) const override {

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