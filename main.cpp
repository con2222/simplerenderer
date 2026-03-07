#include <string>
#include <ctime>
#include <random>

#include "ModelsNames.h"
#include "Shaders.h"

#include <string>
#include <ctime>
#include <vector>
#include <limits>

#include "ModelsNames.h"
#include "Shaders.h"
#include "our_gl.h"

std::vector<double> compute_AO_bruteforce(int width, int height, Model& model, Model& floor) {
    std::vector<double> ao_buffer(width * height, 0.0);
    TGAImage dummy_fb(width, height, TGAImage::GRAYSCALE);

    std::vector<geom::vec3> light_dirs(1000);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> distribution(-1.0, 1.0);

    for (int i = 0; i < 1000; i++) {
        for (int j = 0; j < 3; j++) {
            auto value = distribution(gen);
            light_dirs[i][j] = value;
        }
        if (norm(light_dirs[i]) > 1.0) {
            i--;
        } else {
            light_dirs[i].y = light_dirs[i].y < 0. ? light_dirs[i].y * -1 : light_dirs[i].y;
            light_dirs[i] = normalize(light_dirs[i]);
        }
    }

    lookat(eye, center, up);
    init_perspective(norm(eye - center));
    init_viewport(0, 0, width, height);
    init_zbuffer(width, height);

    DepthShader depth_shader_diablo(model);
    model.draw_model(dummy_fb, depth_shader_diablo);
    DepthShader depth_shader_floor(floor);
    floor.draw_model(dummy_fb, depth_shader_floor);

    std::vector<double> camera_zbuffer = zbuffer;
    geom::matrix<4, 4> M_camera_full = Viewport * Perspective * ModelView;
    geom::matrix<4, 4> M_camera_inv = M_camera_full.inverse();

    for (int i = 0; i < 1000; i++ ) {
        geom::vec3 light_pos = light_dirs[i] * 3.0;
        lookat(light_pos, center, up);
        init_perspective(norm(light_pos - center));
        init_viewport(0, 0, width, height);
        init_zbuffer(width, height);
        model.draw_model(dummy_fb, depth_shader_diablo);
        floor.draw_model(dummy_fb, depth_shader_floor);
        geom::matrix<4, 4> M_light_full = Viewport * Perspective * ModelView;

        geom::matrix<4, 4> M_screen_to_light = M_light_full * M_camera_inv;

        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                double z = camera_zbuffer[x + y * width];
                if (z == -std::numeric_limits<double>::max()) continue;
                geom::vec4 frag_screen(x, y, z, 1.0);
                geom::vec4 frag_light = M_screen_to_light * frag_screen;
                frag_light.x = frag_light.x / frag_light.w;
                frag_light.y = frag_light.y / frag_light.w;
                frag_light.z = frag_light.z / frag_light.w;

                if (frag_light.x < 0 || frag_light.x >= width || frag_light.y < 0 || frag_light.y >= height) continue;
                int light_idx = (int)frag_light.x + (int)frag_light.y * width;
                double shadow_z = zbuffer[light_idx];

                if (frag_light.z > shadow_z - 0.01) {
                    ao_buffer[x + y * width] += 1.0;
                }
            }
        }
    }

    for (int i = 0; i < ao_buffer.size(); i++) {
        ao_buffer[i] /= 1000.0;
    }

    TGAImage ao(width, height, TGAImage::GRAYSCALE);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            ao.set(x, y, {static_cast<unsigned char>(ao_buffer[x + y * width] * 255.0)});
        }
    }
    ao.write_tga_file("ao_buffer.tga");

    return ao_buffer;
}

void visualize_ssao_kernel(const std::vector<geom::vec3>& kernel) {
    int size = 512;
    TGAImage viz(size, size, TGAImage::RGB);

    // Рисуем оси для наглядности
    for(int i=0; i<size; i++) {
        viz.set(i, size/2, TGAColor{100, 100, 100, 255}); // Ось X
        viz.set(size/2, i, TGAColor{100, 100, 100, 255}); // Ось Z (высота)
    }

    for (const auto& sample : kernel) {
        // Переводим координаты из [-1, 1] в пиксели [0, 512]
        // Мы смотрим на полусферу "сбоку", поэтому берем X и Z
        int x = (sample.x + 1.0) * (size / 2);
        int z = (sample.z + 0.0) * (size / 1.0); // Z у нас от 0 до 1, поэтому смещаем иначе

        // Рисуем точку (жирную, чтобы было видно)
        for(int dx=-2; dx<=2; dx++) {
            for(int dz=-2; dz<=2; dz++) {
                if (x+dx >= 0 && x+dx < size && (size-z)+dz >= 0 && size-z+dz < size)
                    viz.set(x + dx, (size - z) + dz, TGAColor{255, 255, 255, 255});
            }
        }
    }
    viz.write_tga_file("kernel_viz.tga");
}

std::vector<double> compute_SSAO(int width, int height, Model& model, Model& floor) {

    std::vector<geom::vec3> ssao_kernel;
    ssao_kernel.reserve(128);
    std::random_device rd;
    std::mt19937 gen(rd());
    std::uniform_real_distribution<double> distribution(0, 1.0);
    auto lerp = [](double a, double b, double f) { return a + f * (b - a); };

    for (int i = 0; i < 128; i++) {
        geom::vec3 sample(distribution(gen) * 2.0 - 1.0, distribution(gen) * 2.0 - 1.0, distribution(gen));
        sample = normalize(sample);
        sample = sample * distribution(gen);
        double scale = double(i) / 128.0;
        scale = lerp(0.1, 1.0, scale * scale);
        sample = sample * scale;
        ssao_kernel.push_back(sample);
    }

    visualize_ssao_kernel(ssao_kernel);

    std::vector<geom::vec3> ssao_noise;
    ssao_noise.reserve(16);
    for (int i = 0; i < 16; i++) {
        geom::vec3 noise(distribution(gen) * 2.0 - 1.0, distribution(gen) * 2.0 - 1.0, 0.0);
        ssao_noise.push_back(noise);
    }

    lookat(eye, center, up);
    init_perspective(norm(eye - center));
    init_viewport(0, 0, width, height);
    init_zbuffer(width, height);

    TGAImage normal_fb(width, height, TGAImage::RGB);

    GeometryShader geom_shader_floor(floor);
    floor.draw_model(normal_fb, geom_shader_floor);

    GeometryShader geom_shader_diablo(model);
    model.draw_model(normal_fb, geom_shader_diablo);

    std::vector<double> ao_buffer(width * height, 0.0);
    double radius = 1.5;
    geom::matrix<4, 4> M_inv = (Viewport * Perspective).inverse(); // from 2D to 3D

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int idx = x + y * width;
            double z_screen = zbuffer[idx];

            if (z_screen == -std::numeric_limits<double>::max()) {
                ao_buffer[idx] = 1.0;
                continue;
            }

            geom::vec4 screen_coord(x, y, z_screen, 1.0);
            geom::vec4 view_coord = M_inv * screen_coord; // frag_pos to 3D
            geom::vec3 frag_pos(
                view_coord.x / view_coord.w,
                view_coord.y / view_coord.w,
                view_coord.z / view_coord.w
            );

            TGAColor c = normal_fb.get(x, y);

            geom::vec3 normal(
                c.bgra[2] / 127.5 - 1.0,
                c.bgra[1] / 127.5 - 1.0,
                c.bgra[0] / 127.5 - 1.0
            );

            normal = geom::normalize(normal);

            if (norm(normal) < 0.1) {
                ao_buffer[idx] = 1.0;
                continue;
            }

            int noise_x = x % 4;
            int noise_y = y % 4;
            geom::vec3 random_vec = ssao_noise[noise_x + noise_y * 4];

            geom::vec3 t = geom::normalize(random_vec - normal * dot(random_vec, normal));
            geom::vec3 b = cross(normal, t);
            geom::matrix<3, 3> TBN = {{{t.x, b.x, normal.x}, {t.y, b.y, normal.y}, {t.z, b.z, normal.z}}};

            double occlusion = 0.0;

            for (int i = 0; i < 128; i++) {
                geom::vec3 ssao_vec = TBN * ssao_kernel[i];
                geom::vec3 sample_pos = frag_pos + ssao_vec * radius;

                geom::vec4 clip = Perspective * geom::vec4{sample_pos.x, sample_pos.y, sample_pos.z, 1.0};\
                geom::vec4 ndc = geom::vec4(clip.x / clip.w, clip.y / clip.w, clip.z / clip.w, 1.0);


                geom::vec4 screen = Viewport * ndc;
                int sx = (int)screen.x;
                int sy = (int)screen.y;

                if (sx < 0 || sx >= width || sy < 0 || sy >= height) continue;

                double shadow_z_screen = zbuffer[sx + sy * width];
                if (shadow_z_screen == -std::numeric_limits<double>::max()) continue;

                geom::vec4 shadow_screen_coord(sx, sy, shadow_z_screen, 1.0);
                geom::vec4 shadow_view_coord = M_inv * shadow_screen_coord;
                double z_pos = shadow_view_coord.z / shadow_view_coord.w;

                if (sample_pos.z < z_pos - 0.01 && std::abs(frag_pos.z - z_pos) < radius) {
                    occlusion += 1.0;
                }
            }

            occlusion = 1.0 - (occlusion / 128.0);
            ao_buffer[idx] = occlusion;
        }
    }

    TGAImage ao_image(width, height, TGAImage::GRAYSCALE);
    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            int color_val = std::max(0, std::min(255, (int)(ao_buffer[x + y * width] * 255.0)));
            ao_image.set(x, y, TGAColor({static_cast<unsigned char>(color_val)}));
        }
    }
    ao_image.write_tga_file("ssao_raw.tga");

    return ao_buffer;
}

int main(int argc, char** argv) {
    constexpr int width  = SIZE;

    constexpr int height = SIZE;

    TGAImage framebuffer(width, height, TGAImage::RGB);
    Model diablo(DIABLO);
    Model floor(FLOOR);

    std::vector<double> ao_buffer = compute_SSAO(width, height, diablo, floor);

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            framebuffer.set(i, j, black);
        }
    }
    init_zbuffer(width, height);
    lookat(eye, center, up);
    init_perspective(norm(eye - center));
    init_viewport(0, 0, width, height);

    geom::vec3 light_dir_world = normalize(geom::vec3(1.5, 0.5, 1.5));

    NormalTangentSpace floor_shader(light_dir_world, floor, ao_buffer, width);
    floor.draw_model(framebuffer, floor_shader);

    NormalTangentSpace diablo_shader(light_dir_world, diablo, ao_buffer, width);
    diablo.draw_model(framebuffer, diablo_shader);

    for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            TGAColor color = framebuffer.get(x, y);

            color = color * (0.4 + 0.6 * ao_buffer[x + y * width]);
            framebuffer.set(x, y, color);
        }
    }

    TGAImage zbuffer_image(width, height, TGAImage::GRAYSCALE);
    create_zbuffer_image(zbuffer_image);

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}