#include "Model.h"

#include <algorithm>
#include <filesystem>
#include <random>
#include <fstream>
#include <string>
#include <sstream>
#include "ModelsNames.h"
#include "color.h"
#include "Geometry.h"
#include "rasterizer.h"
#include "our_gl.h"
#include "Shaders.h"

namespace fs = std::filesystem;

std::ostream& operator<<(std::ostream& s, const geom::vec<3>& v) {
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

                verts.push_back(geom::vec<3>({xt, yt, zt}));
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

                tex_coords.push_back(geom::vec<3>({x, y, z}));
            } else if (line.substr(0, 2) == "vn") {
                int index = line.find_first_of(digits);

                if (index != std::string::npos) {
                    line = line.substr(index);
                }

                std::stringstream coords(line);
                float x, y, z;
                coords >> x >> y >> z;

                normals.push_back(geom::vec<3>({x, y, z}));
            }

        }
    }
    in.close();
}

void Model::draw_model(TGAImage& framebuffer, TGAColor color, IShader& shader) const {

    int frame_count = 0;
    std::string out_dir = "frames";
    fs::create_directories(out_dir);
    std::cout << "Current path is: " << std::filesystem::current_path() << std::endl;
    int save_every_n = 30;

    for (int i = 0; i < faces.size(); i++) {
        geom::vec4 clip_coords[3];

        for (int j = 0; j < 3; j++) {
            clip_coords[j] = shader.vertex(i, j);
        }

        /*RandomShader& rndShader = static_cast<RandomShader&>(shader);
        if (rndShader) {
            rndShader.color = {static_cast<uint8_t>(std::rand()%256),
            static_cast<uint8_t>(std::rand()%256),
            static_cast<uint8_t>(std::rand()%256),
            255};
        }*/


        triangle_barycentric_bounding_box(clip_coords, framebuffer, shader);
    }
}

void Model::draw_points(TGAImage& framebuffer, TGAColor color) const {
    geom::matrix<4, 4> T = Viewport * Perspective * ModelView;

    for (size_t i = 0; i < verts.size(); i++) {
        geom::vec<4> v({verts[i].x, verts[i].y, verts[i].z, 1.0});

        v = T * v;

        v.x /= v.w;
        v.y /= v.w;
        v.z /= v.w;

        draw_fat_point(v.x, v.y, framebuffer, color, POINT_SIZE);
    }
}

geom::vec3 Model::vert(int face, int vert) const {
    return verts[faces[face].corners[vert].v];
}

geom::vec3 Model::normal(int face, int vert) const {
    return normals[faces[face].corners[vert].n];
}
