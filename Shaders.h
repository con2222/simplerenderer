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

struct NormalTangentSpace : public IShader {
    const Model &model;
    geom::vec3 light_dir;
    geom::matrix<4, 4> normalMatrix;
    double exponent = 50.0;

    geom::vec3 varying[3];
    geom::vec2 varying_uv[3];
    geom::vec3 tri[3];

    NormalTangentSpace(const geom::vec3& light, const Model& model) : model(model) {
        normalMatrix = transpose(ModelView.inverse());
        geom::vec4 l = ModelView * geom::vec4{light.x, light.y, light.z, 0};
        light_dir = normalize(l.xyz());
    };

    geom::vec4 vertex(int face, int nthvert) override {
        geom::vec3 v = model.vert(face, nthvert);
        geom::vec3 normal = model.normal(face, nthvert);

        geom::vec4 n = normalMatrix * geom::vec4{normal.x, normal.y, normal.z, 0.};
        varying[nthvert] = normalize(n.xyz());

        varying_uv[nthvert] = model.uv(face, nthvert);

        geom::vec4 gl_Position = ModelView * geom::vec4{v.x, v.y, v.z, 1.};
        tri[nthvert] = gl_Position.xyz();

        return Perspective * gl_Position;
    }

    virtual std::pair<bool, TGAColor> fragment(geom::vec3 bar) const override {
        geom::vec2 uv = varying_uv[0] * bar.x + varying_uv[1] * bar.y + varying_uv[2] * bar.z;

        geom::vec3 e0 = tri[1] - tri[0];
        geom::vec3 e1 = tri[2] - tri[0];
        geom::vec2 u0 = varying_uv[1] - varying_uv[0];
        geom::vec2 u1 = varying_uv[2] - varying_uv[0];

        geom::matrix<3, 2> E = {{{e0.x, e1.x}, {e0.y, e1.y}, {e0.z, e1.z}}};
        geom::matrix<2, 2> U = {{{u0.x, u1.x}, {u0.y, u1.y}}};

        geom::matrix<3, 2> tb =  E * U.inverse();

        geom::vec3 t = normalize(geom::vec3(tb[0][0], tb[1][0], tb[2][0]));
        geom::vec3 b = normalize(geom::vec3(tb[0][1], tb[1][1], tb[2][1]));
        geom::vec3 normal = normalize(varying[0] * bar.x + varying[1] * bar.y + varying[2] * bar.z);

        geom::matrix<3, 3> TBN = {{{t.x, b.x, normal.x},{t.y, b.y, normal.y},{t.z, b.z, normal.z}}};

        geom::vec3 n_tangent = model.normal_from_tangent_map(uv);

        geom::vec3 newNormal = normalize(TBN * n_tangent);

        geom::vec3 frag_pos = tri[0] * bar.x + tri[1] * bar.y + tri[2] * bar.z;
        geom::vec3 v = normalize(-1.0 * frag_pos);

        const double ambient = 0.05;

        double diffuse = std::max(0., dot(newNormal, light_dir));
        geom::vec3 r = normalize(2 * newNormal * dot(newNormal, light_dir) - light_dir);
        double specular = std::pow(std::max(0., dot(r, v)), exponent);

        TGAColor diff_color = model.diffuse_from_map(uv);
        double spec_power = model.specular_from_map(uv);

        diff_color = diff_color * (ambient + diffuse);

        TGAColor whitecolor = white;

        whitecolor = whitecolor * (specular * spec_power);

        return {false, diff_color + whitecolor};
    }
};

struct DepthShader : public IShader {
    const Model &model;

    DepthShader(const Model &model) : model{model} {}

    virtual geom::vec4 vertex(int iface, int nthvert) {
        geom::vec3 v = model.vert(iface, nthvert);
        return Perspective * ModelView * geom::vec4{v.x, v.y, v.z, 1.};
    }

    virtual std::pair<bool, TGAColor> fragment(geom::vec3 bar) const override {
        return {false, white};
    }
};

struct ShadowShader : public IShader {
    const Model &model;
    geom::vec3 light_dir;
    geom::matrix<4, 4> normalMatrix;
    int width;

    const std::vector<double>& shadow_map;
    geom::matrix<4, 4> M_shadow; // from camera screen to light screen

    double exponent = 50.0;

    geom::vec3 varying[3];
    geom::vec2 varying_uv[3];
    geom::vec3 tri[3];

    ShadowShader(const geom::vec3& light, const Model& model, const std::vector<double>& sm, geom::matrix<4, 4> ms, int w)
        : model(model), shadow_map(sm), M_shadow(ms), width(w) {
        geom::vec4 l = ModelView * geom::vec4{light.x, light.y, light.z, 0};
        normalMatrix = transpose(ModelView.inverse());
        light_dir = normalize(l.xyz());
    };

    geom::vec4 vertex(int face, int nthvert) override {
        geom::vec3 v = model.vert(face, nthvert);
        geom::vec3 normal = model.normal(face, nthvert);

        geom::vec4 n = normalMatrix * geom::vec4{normal.x, normal.y, normal.z, 0.};
        varying[nthvert] = normalize(n.xyz());

        varying_uv[nthvert] = model.uv(face, nthvert);

        geom::vec4 gl_Position = ModelView * geom::vec4{v.x, v.y, v.z, 1.};
        tri[nthvert] = gl_Position.xyz();

        return Perspective * gl_Position;
    }

    virtual std::pair<bool, TGAColor> fragment(geom::vec3 bar) const override {
        geom::vec2 uv = varying_uv[0] * bar.x + varying_uv[1] * bar.y + varying_uv[2] * bar.z;

        geom::vec3 e0 = tri[1] - tri[0];
        geom::vec3 e1 = tri[2] - tri[0];
        geom::vec2 u0 = varying_uv[1] - varying_uv[0];
        geom::vec2 u1 = varying_uv[2] - varying_uv[0];

        geom::matrix<3, 2> E = {{{e0.x, e1.x}, {e0.y, e1.y}, {e0.z, e1.z}}};
        geom::matrix<2, 2> U = {{{u0.x, u1.x}, {u0.y, u1.y}}};

        geom::matrix<3, 2> tb =  E * U.inverse();

        geom::vec3 t = normalize(geom::vec3(tb[0][0], tb[1][0], tb[2][0]));
        geom::vec3 b = normalize(geom::vec3(tb[0][1], tb[1][1], tb[2][1]));
        geom::vec3 normal = normalize(varying[0] * bar.x + varying[1] * bar.y + varying[2] * bar.z);

        geom::matrix<3, 3> TBN = {{{t.x, b.x, normal.x},{t.y, b.y, normal.y},{t.z, b.z, normal.z}}};

        geom::vec3 n_tangent = model.normal_from_tangent_map(uv);

        geom::vec3 newNormal = normalize(TBN * n_tangent);

        geom::vec3 frag_pos = tri[0] * bar.x + tri[1] * bar.y + tri[2] * bar.z;

        geom::vec4 frag_light_screen = M_shadow * geom::vec4(frag_pos.x, frag_pos.y, frag_pos.z, 1.);

        frag_light_screen = frag_light_screen / frag_light_screen.w;

        geom::vec3 v = normalize(-1.0 * frag_pos);

        const double ambient = 0.2;

        double diffuse = std::max(0., dot(newNormal, light_dir));
        geom::vec3 r = normalize(2 * newNormal * dot(newNormal, light_dir) - light_dir);
        double specular = std::pow(std::max(0., dot(r, v)), exponent);

        TGAColor diff_color = model.diffuse_from_map(uv);
        double spec_power = model.specular_from_map(uv);
        TGAColor whitecolor = white;

        int idx = int(frag_light_screen.x) + int(frag_light_screen.y) * width;
        if (idx < 0 || idx >= shadow_map.size()) return {false, diff_color + whitecolor};
        double shadow = 1.0;

        if (frag_light_screen.z < shadow_map[idx] - 0.01) {
            shadow = 0.3;
        }

        diff_color = diff_color * (ambient + diffuse * shadow);

        whitecolor = whitecolor * (specular * spec_power * shadow);

        return {false, diff_color + whitecolor};
    }
};
