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

int main(int argc, char** argv) {
    constexpr int width  = SIZE;
    constexpr int height = SIZE;

    TGAImage framebuffer(width, height, TGAImage::RGB);
    Model diablo(DIABLO);
    Model floor(FLOOR);

    std::vector<double> ao_buffer = compute_AO_bruteforce(width, height, diablo, floor);

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
    NormalTangentSpace diablo_shader(light_dir_world, diablo, ao_buffer);
    diablo.draw_model(framebuffer, diablo_shader);

    NormalTangentSpace floor_shader(light_dir_world, floor, ao_buffer);
    floor.draw_model(framebuffer, floor_shader);

    /*for (int y = 0; y < height; y++) {
        for (int x = 0; x < width; x++) {
            TGAColor color = framebuffer.get(x, y);

            // We always set 40% of the base color, and adjust the remaining 60% using AO
            color = color *(0.4 + 0.6 * ao_buffer[x + y * width]);
            framebuffer.set(x, y, color);
        }
    }*/

    TGAImage zbuffer_image(width, height, TGAImage::GRAYSCALE);
    create_zbuffer_image(zbuffer_image);

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}