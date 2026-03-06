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

int main(int argc, char** argv) {
    constexpr int width  = SIZE;
    constexpr int height = SIZE;

    TGAImage framebuffer(width, height, TGAImage::RGB);
    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            framebuffer.set(i, j, black);

        }
    }

    Model diablo(DIABLO);
    Model floor(FLOOR);

    geom::vec3 light_dir_world = normalize(geom::vec3(1.5, 0.5, 1.5));
    geom::vec3 light_pos = light_dir_world * 3.0;

    lookat(light_pos, center, up);
    init_perspective(norm(light_pos - center));
    init_viewport(0, 0, width, height);
    init_zbuffer(width, height);

    geom::matrix<4, 4> M_light = Viewport * Perspective * ModelView;

    DepthShader depth_shader_diablo(diablo);
    diablo.draw_model(framebuffer, depth_shader_diablo);

    DepthShader depth_shader_floor(floor);
    floor.draw_model(framebuffer, depth_shader_floor);

    std::vector<double> shadow_buffer = zbuffer;

    for (int i = 0; i < width; i++) {
        for (int j = 0; j < height; j++) {
            framebuffer.set(i, j, black);
        }
    }
    init_zbuffer(width, height);

    lookat(eye, center, up);
    init_perspective(norm(eye - center));
    init_viewport(0, 0, width, height);

    geom::matrix<4, 4> M_view_camera = ModelView;

    geom::matrix<4, 4> M_shadow_matrix = M_light * M_view_camera.inverse();

    ShadowShader diablo_shader(light_dir_world, diablo, shadow_buffer, M_shadow_matrix, width);
    diablo.draw_model(framebuffer, diablo_shader);

    ShadowShader floor_shader(light_dir_world, floor, shadow_buffer, M_shadow_matrix, width);
    floor.draw_model(framebuffer, floor_shader);

    TGAImage zbuffer_image(width, height, TGAImage::GRAYSCALE);
    create_zbuffer_image(zbuffer_image);

    framebuffer.write_tga_file("framebuffer.tga");
    return 0;
}