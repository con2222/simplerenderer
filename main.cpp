#include <cmath>
#include <string>

#include "ModelsNames.h"
#include "Model.h"
#include "Color.h"

void create_zbuffer_image(TGAImage& zbuffer_image, float* zbuffer) {
    int width = zbuffer_image.width();
    int height = zbuffer_image.height();

    float min_z = std::numeric_limits<float>::max();
    float max_z = -std::numeric_limits<float>::max();

    for (int i = 0; i < width * height; i++) {
        if (zbuffer[i] != -std::numeric_limits<float>::max()) {
            if (zbuffer[i] < min_z) min_z = zbuffer[i];
            if (zbuffer[i] > max_z) max_z = zbuffer[i];
        }
    }

    if (max_z == min_z) max_z = min_z + 1.0f;

    for (int i = 0; i < width * height; i++) {
        if (zbuffer[i] != -std::numeric_limits<float>::max()) {
            float normalized_z = (zbuffer[i] - min_z) / (max_z - min_z);

            int color = normalized_z * 255.f;
            color = std::max(0, std::min(255, color));

            zbuffer_image.set(i % width, i / width, {static_cast<unsigned char>(color)});
        }
    }

    zbuffer_image.write_tga_file("zbuffer.tga");
}

matrix<4, 4> build_viewport(const int x, const int y, const int width, const int height) {
    matrix<4, 4> viewport;
    viewport = {{{width/2., 0, 0, x + width/2.}, {0, height/2., 0, y + height/2.}, {0, 0, 1, 0}, {0, 0, 0, 1}}};
    return viewport;
}

matrix<4, 4> build_perspective(const double f) {
    matrix<4, 4> perspective = {{{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0, -1/f,1}}};
    return perspective;
}

matrix<4, 4> look_at(vec3 eye, vec3 center, vec3 up) {
    matrix<4, 4> modelView;
    vec<3> n = normalize((eye - center));
    vec<3> l = normalize(cross(up, n));
    vec<3> m = normalize(cross(n, l));
    matrix<4, 4> Rotation = {{{l.x, l.y, l.z, 0}, {m.x, m.y, m.z, 0}, {n.x, n.y, n.z, 0}, {0, 0, 0, 1}}};
    matrix<4, 4> Translation = {{{1, 0, 0, -eye.x}, {0, 1, 0, -eye.y}, {0, 0, 1, -eye.z}, {0, 0, 0, 1}}};

    modelView = Rotation * Translation;
    return modelView;
}

int main(int argc, char** argv) {
    constexpr int width  = SIZE;
    constexpr int height = SIZE;
    constexpr vec3    eye(1, 1, 3); // camera position
    constexpr vec3 center(0, 0, 0);// camera direction
    constexpr vec3     up(0, 1, 0);  // camera up vector


    matrix<4, 4> LookAt, Perspective, Viewport;
    Viewport = build_viewport(0, 0, width, height);
    Perspective = build_perspective(norm(eye - center));
    LookAt = look_at(eye, center, up);


    TransformState transform_state({Viewport, Perspective, LookAt});

    TGAImage framebuffer(width, height, TGAImage::RGB);
    TGAImage zbuffer_image(width, height, TGAImage::GRAYSCALE);
    float *zbuffer = new float[width * height];

    for (int i = 0; i < width * height; i++) {
        zbuffer[i] = -std::numeric_limits<float>::max();
    }


    Model model(DIABLO);
    model.draw_model(framebuffer, zbuffer, width, height, red, transform_state);
    create_zbuffer_image(zbuffer_image, zbuffer);

    delete[] zbuffer;
    framebuffer.write_tga_file("framebuffer.tga");


    return 0;
}