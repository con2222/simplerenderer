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

struct PhongShading : public IShader {
    const Model &model;
    TGAColor color;
    geom::vec3 light_dir;
    geom::vec3 varying[3];
    geom::vec3 tri[3];
    geom::matrix<4, 4> normalMatrix;
    int exponent = 5;

    PhongShading(const geom::vec3& light, const Model& model, const TGAColor& color) : model(model), color(color) {
        normalMatrix = transpose(ModelView.inverse());
        geom::vec4 l = ModelView * geom::vec4{light.x, light.y, light.z, 0};
        light_dir = normalize(l.xyz());

    };

    geom::vec4 vertex(int face, int nthvert) override {
        geom::vec3 v = model.vert(face, nthvert);
        geom::vec3 normal = model.normal(face, nthvert);

        geom::vec4 n = normalMatrix * geom::vec4{normal.x, normal.y, normal.z, 0.};
        varying[nthvert] = normalize(n.xyz());

        geom::vec4 gl_Position = ModelView * geom::vec4{v.x, v.y, v.z, 1.};
        tri[nthvert] = gl_Position.xyz();

        return Perspective * gl_Position;
    }

    virtual std::pair<bool, TGAColor> fragment(geom::vec3 bar) const override {
        geom::vec3 newNormal = normalize(bar.x * varying[0] + bar.y * varying[1] + bar.z * varying[2]);

        geom::vec3 frag_pos = tri[0] * bar.x + tri[1] * bar.y + tri[2] * bar.z; // координата полигона в координатной системе камеры
        geom::vec3 v = normalize(-1 * frag_pos);

        const double ambient = 0.1;

        double diffuse = std::max(0., dot(newNormal, light_dir));

        geom::vec3 r = normalize(2 * newNormal * dot(newNormal, light_dir) - light_dir);
        double specular = std::pow(std::max(0., dot(r, v)), exponent);

        double intensity = ambient + diffuse + specular;

        return {false, color * intensity};
    }
};

struct NormalMappingShader : public IShader {
    const Model &model;
    geom::vec3 light_dir;
    geom::matrix<4, 4> normalMatrix;
    int exponent = 5;

    geom::vec2 varying_uv[3];
    geom::vec3 tri[3];

    NormalMappingShader(const geom::vec3& light, const Model& model) : model(model) {
        normalMatrix = transpose(ModelView.inverse());
        geom::vec4 l = ModelView * geom::vec4{light.x, light.y, light.z, 0};
        light_dir = normalize(l.xyz());
    };

    geom::vec4 vertex(int face, int nthvert) override {
        geom::vec3 v = model.vert(face, nthvert);

        varying_uv[nthvert] = model.uv(face, nthvert);

        geom::vec4 gl_Position = ModelView * geom::vec4{v.x, v.y, v.z, 1.};
        tri[nthvert] = gl_Position.xyz();

        return Perspective * gl_Position;
    }

    virtual std::pair<bool, TGAColor> fragment(geom::vec3 bar) const override {
        geom::vec2 uv = varying_uv[0] * bar.x + varying_uv[1] * bar.y + varying_uv[2] * bar.z;

        geom::vec3 n = model.normal_from_map(uv);

        geom::vec4 n_rotated = normalMatrix * geom::vec4{n.x, n.y, n.z, 0.};

        geom::vec3 newNormal = geom::normalize(n_rotated.xyz());

        geom::vec3 frag_pos = tri[0] * bar.x + tri[1] * bar.y + tri[2] * bar.z;
        geom::vec3 v = normalize(-1.0 * frag_pos);

        const double ambient = 0.25;

        double diffuse = std::max(0., dot(newNormal, light_dir));
        geom::vec3 r = normalize(2 * newNormal * dot(newNormal, light_dir) - light_dir);
        double specular = std::pow(std::max(0., dot(r, v)), exponent);

        TGAColor diff_color = model.diffuse_from_map(uv);
        double spec_power = model.specular_from_map(uv);

        diff_color = diff_color * (ambient + diffuse);

        TGAColor whitecolor = white;

        whitecolor = whitecolor * (specular * spec_power);

        //double intensity = ambient + diffuse + specular;

        return {false, diff_color + whitecolor /*+ model.glow_from_map(uv)*/};
    }
};