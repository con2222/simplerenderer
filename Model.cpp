#include "Model.h"

#include <algorithm>
#include <filesystem>

#include <fstream>
#include <string>
#include <sstream>
#include "ModelsNames.h"
#include "Color.h"
#include "Geometry.h"
#include "rasterizer.h"
#include "our_gl.h"

namespace fs = std::filesystem;

std::ostream& operator<<(std::ostream& s, const vec<3>& v) {
    s << "(" << v.x << ", " << v.y << ", " << v.z << ")" << std::endl;
    return s;
}

std::ostream& operator<<(std::ostream& s, const Face& face) {

    for (int i = 0; i < 3; i++) {
        s << face.corners[i].v << "/" << face.corners[i].t << "/" << face.corners[i].n;
        if (i != 2) {
            s << " ";
        }
    }
    return s;
}

Model::Model(const std::string& fileName) {
    std::string line;
    std::string digits = "-0123456789";

    std::ifstream in(fileName);

    if (in.is_open())
    {
        while (std::getline(in, line)) {
            if (line[0] == '#') {
                continue;
            }
            if (line[0] == 'v' && line[1] == ' ') {
                size_t index = line.find_first_of(digits);

                if (index != std::string::npos) {
                    line = line.substr(index);
                }
                std::istringstream coords(line);

                float xt, yt, zt;
                coords >> xt >> yt >> zt;

                verts.push_back(vec<3>({xt, yt, zt}));
            } else if (line[0] == 'f' && line[1] == ' ') {
                size_t index = line.find_first_of(digits);

                if (index != std::string::npos) {
                    line = line.substr(index);
                }

                std::istringstream iss(line);
                std::string str;

                Face face = Face();
                int i = 0;
                while (iss >> str && i < 3) {

                    int v, t, n;

                    int result = sscanf(str.c_str(), "%d/%d/%d", &v, &t, &n);

                    if (result == 3) {
                        face.corners[i].v = v - 1;
                        face.corners[i].t = t - 1;
                        face.corners[i].n = n - 1;
                    }
                    
                    i++;
                }

                faces.push_back(face);
                
            } else if (line.substr(0, 2) == "vt") {

                int index = line.find_first_of(digits);

                if (index != std::string::npos) {
                    line = line.substr(index);
                }

                std::stringstream coords(line);
                float x, y, z;
                coords >> x >> y >> z;

                tex_coords.push_back(vec<3>({x, y, z}));
            } else if (line.substr(0, 2) == "vn") {
                int index = line.find_first_of(digits);

                if (index != std::string::npos) {
                    line = line.substr(index);
                }

                std::stringstream coords(line);
                float x, y, z;
                coords >> x >> y >> z;

                normals.push_back(vec<3>({x, y, z}));
            }

        }
    }
    in.close();
}

void Model::draw_model(TGAImage& framebuffer, TGAColor color, const IShader& shader) const {
    int width = framebuffer.width();
    int height = framebuffer.height();

    int frame_count = 0;
    std::string out_dir = "frames";
    fs::create_directories(out_dir);
    std::cout << "Current path is: " << std::filesystem::current_path() << std::endl;
    int save_every_n = 30;

    matrix<4, 4> T = Viewport * Perspective * ModelView; // from right to left

    for (auto& face : faces)
    {

        vec<4> v0({verts[face.corners[0].v].x, verts[face.corners[0].v].y, verts[face.corners[0].v].z, 1});
        vec<4> v1({verts[face.corners[1].v].x, verts[face.corners[1].v].y, verts[face.corners[1].v].z, 1});
        vec<4> v2({verts[face.corners[2].v].x, verts[face.corners[2].v].y, verts[face.corners[2].v].z, 1});

        v0 = T * v0;
        v0.x /= v0.w;
        v0.y /= v0.w;
        v0.z /= v0.w;


        v1 = T * v1;
        v1.x /= v1.w;
        v1.y /= v1.w;
        v1.z /= v1.w;

        v2 = T * v2;
        v2.x /= v2.w;
        v2.y /= v2.w;
        v2.z /= v2.w;

        triangle_barycentric_bounding_box(v0.x, v0.y, v0.z, v1.x, v1.y, v1.z, v2.x, v2.y, v2.z, framebuffer, shader);

        /*if (frame_count % save_every_n == 0) {
            std::string filename = out_dir + "/frame_" + std::to_string(frame_count / save_every_n) + ".tga";
            framebuffer.write_tga_file(filename);
        }*/
        frame_count++;
    }
}

void Model::draw_points(TGAImage& framebuffer, TGAColor color) const {
    matrix<4, 4> T = Viewport * Perspective * ModelView;

    for (size_t i = 0; i < verts.size(); i++) {
        vec<4> v({verts[i].x, verts[i].y, verts[i].z, 1.0});

        v = T * v;

        v.x /= v.w;
        v.y /= v.w;
        v.z /= v.w;

        draw_fat_point(v.x, v.y, framebuffer, color, POINT_SIZE);
    }
}