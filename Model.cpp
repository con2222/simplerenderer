#include "Model.h"

#include <algorithm>
#include <filesystem>

#include <fstream>
#include <string>
#include <sstream>
#include "ModelsNames.h"
#include "Color.h"
#include "Geometry.h"

namespace fs = std::filesystem;

std::ostream& operator<<(std::ostream& s, const Vec3f& v) {
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

Model::Model(std::string fileName) {
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

                verts.push_back(Vec3f({xt, yt, zt}));
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

                    face.corners[i].v = v - 1;
                    face.corners[i].t = t - 1;
                    face.corners[i].n = n - 1;
                    
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

                tex_coords.push_back(Vec3f({x, y, z}));
            } else if (line.substr(0, 2) == "vn") {
                int index = line.find_first_of(digits);

                if (index != std::string::npos) {
                    line = line.substr(index);
                }

                std::stringstream coords(line);
                float x, y, z;
                coords >> x >> y >> z;

                normals.push_back(Vec3f({x, y, z}));
            }

        }
    }
    in.close();
}

void Model::draw_model(TGAImage& framebuffer, int width, int height, TGAColor color) const {
    for (auto& face : faces)
    {
        Vec3f v0 = verts[face.corners[0].v];
        Vec3f v1 = verts[face.corners[1].v];
        Vec3f v2 = verts[face.corners[2].v];


        /* переводим из [-1, 1] в [0, 2], и делим ширину с высотой на 2 так как переводим 
         виртульные 2 единицы в реальные width пикселей */

        int ax = (v0.x + 1.0f) * width / 2.0f; 
        int ay = (v0.y + 1.0f) * height / 2.0f;
        int az = (v0.z + 1.0f) * 255/2;
        int bx = (v1.x + 1.0f) * width / 2.0f;
        int by = (v1.y + 1.0f) * height / 2.0f;
        int bz = (v1.z + 1.0f) * 255/2;
        int cx = (v2.x + 1.0f) * width / 2.0f;
        int cy = (v2.y + 1.0f) * height / 2.0f;
        int cz = (v2.z + 1.0f) * 255/2;


        triangle(ax, ay, az, bx, by, bz, cx, cy, cz, framebuffer);
    }


    for (auto& p : verts)
    {
        int px = (p.x + 1.0f) * width / 2.0f;
        int py = (p.y + 1.0f) * height / 2.0f;
        
        draw_fat_point(px, py, framebuffer, white, POINT_SIZE);
    }
}

void Model::painters_algorithm_render(struct TGAImage &framebuffer, int width, int height, TGAColor color) {

    int frame_count = 0;
    std::string out_dir = "frames";
    fs::create_directories(out_dir);
    std::cout << "Current path is: " << std::filesystem::current_path() << std::endl;
    int save_every_n = 50;

    std::vector<Triangle> triangles;
    for (auto& face : faces) {
        Vec3f v0 = verts[face.corners[0].v];
        Vec3f v1 = verts[face.corners[1].v];
        Vec3f v2 = verts[face.corners[2].v];

        triangles.push_back({v0, v1, v2});
    }

    //insertion sort
    /*for (int i = 1; i < triangles.size(); i++) {
        for (int j = i; j > 0 && triangles[j-1].z_mid > triangles[j].z_mid; j--) {
            std::swap(triangles[j], triangles[j-1]);
        }
    }*/

    std::sort(triangles.begin(), triangles.end(), [](const Triangle& a, const Triangle& b)
        {
            return a.z_mid < b.z_mid;
        }
    );

    for (auto& Trin : triangles) {
        int ax = (Trin.a.x + 1.0f) * width / 2.0f; //
        int ay = (Trin.a.y + 1.0f) * height / 2.0f;
        int az = (Trin.a.z + 1.0f) * 255/2;
        int bx = (Trin.b.x + 1.0f) * width / 2.0f;
        int by = (Trin.b.y + 1.0f) * height / 2.0f;
        int bz = (Trin.b.z + 1.0f) * 255/2;
        int cx = (Trin.c.x + 1.0f) * width / 2.0f;
        int cy = (Trin.c.y + 1.0f) * height / 2.0f;
        int cz = (Trin.c.z + 1.0f) * 255/2;



        triangle(ax, ay, az, bx, by, bz, cx, cy, cz, framebuffer);

        if (frame_count % save_every_n == 0) {
            std::string filename = out_dir + "/frame_" + std::to_string(frame_count / save_every_n) + ".tga";
            framebuffer.write_tga_file(filename);
        }
        frame_count++;
    }

    for (auto& p : verts)
    {
        int px = (p.x + 1.0f) * width / 2.0f;
        int py = (p.y + 1.0f) * height / 2.0f;

        draw_fat_point(px, py, framebuffer, white, POINT_SIZE);
    }

}
