#include "our_gl.h"
#include <vector>

geom::matrix<4, 4> ModelView, Viewport, Perspective;
std::vector<double> zbuffer;

std::vector<geom::vec3> position_buffer;
std::vector<geom::vec3> normal_buffer;

void init_viewport(const int x, const int y, const int width, const int height) {
    Viewport = {{{width/2., 0, 0, x + width/2.}, {0, height/2., 0, y + height/2.}, {0, 0, 1, 0}, {0, 0, 0, 1}}};
}

void init_perspective(const double f) {
    Perspective = {{{1,0,0,0}, {0,1,0,0}, {0,0,1,0}, {0,0, -1/f,1}}};
}

void lookat(geom::vec3 eye, geom::vec3 center, geom::vec3 up) {
    geom::vec<3> n = normalize((eye - center));
    geom::vec<3> l = normalize(cross(up, n));
    geom::vec<3> m = normalize(cross(n, l));
    geom::matrix<4, 4> Rotation = {{{l.x, l.y, l.z, 0}, {m.x, m.y, m.z, 0}, {n.x, n.y, n.z, 0}, {0, 0, 0, 1}}};
    geom::matrix<4, 4> Translation = {{{1, 0, 0, -eye.x}, {0, 1, 0, -eye.y}, {0, 0, 1, -eye.z}, {0, 0, 0, 1}}};

    ModelView = Rotation * Translation;
}

void init_zbuffer(const int width, const int height) {
    zbuffer = std::vector<double>(width*height, -std::numeric_limits<double>::max());
}

void create_zbuffer_image(TGAImage& zbuffer_image) {
    int width = zbuffer_image.width();
    int height = zbuffer_image.height();

    double min_z = std::numeric_limits<double>::max();
    double max_z = -std::numeric_limits<double>::max();

    for (int i = 0; i < width * height; i++) {
        if (zbuffer[i] != -std::numeric_limits<double>::max()) {
            if (zbuffer[i] < min_z) min_z = zbuffer[i];
            if (zbuffer[i] > max_z) max_z = zbuffer[i];
        }
    }

    if (max_z == min_z) max_z = min_z + 1.0f;

    for (int i = 0; i < width * height; i++) {
        if (zbuffer[i] != -std::numeric_limits<double>::max()) {
            double normalized_z = (zbuffer[i] - min_z) / (max_z - min_z);

            int color = normalized_z * 255.f;
            color = std::max(0, std::min(255, color));

            zbuffer_image.set(i % width, i / width, {static_cast<unsigned char>(color)});
        }
    }

    zbuffer_image.write_tga_file("zbuffer.tga");
}

void save_depth_buffer(const std::vector<double>& buffer, int width, int height, const std::string& filename) {
    TGAImage img(width, height, TGAImage::GRAYSCALE);

    double min_z = std::numeric_limits<double>::max();
    double max_z = -std::numeric_limits<double>::max();

    for (double z : buffer) {
        if (z != -std::numeric_limits<double>::max()) {
            if (z < min_z) min_z = z;
            if (z > max_z) max_z = z;
        }
    }

    if (max_z == min_z) max_z = min_z + 1.0;

    for (int i = 0; i < width * height; i++) {
        if (buffer[i] != -std::numeric_limits<double>::max()) {
            double normalized_z = (buffer[i] - min_z) / (max_z - min_z);
            int color = static_cast<int>(normalized_z * 255.0);
            img.set(i % width, i / width, TGAColor({static_cast<unsigned char>(color)}));
        }
    }

    img.write_tga_file(filename);
}