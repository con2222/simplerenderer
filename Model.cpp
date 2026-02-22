#include "Model.h"

#include <fstream>
#include <string>
#include <sstream>
#include "ModelsNames.h"
#include "Color.h"
#include "Geometry.h"

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

void Model::draw_model(TGAImage& framebuffer, int width, int height) const {
    for (auto& face : faces)
    {
		for (int i = 0; i < 3; i++) {
			Vec3f v0_raw = verts[face.corners[i].v];
        	Vec3f v1_raw = verts[face.corners[(i + 1) % 3].v];

			int ax = (v0_raw.x + 1.0f) * width / 2.0f;
        	int ay = (v0_raw.y + 1.0f) * height / 2.0f;
        	int bx = (v1_raw.x + 1.0f) * width / 2.0f;
       	 	int by = (v1_raw.y + 1.0f) * height / 2.0f;

			//int ay = (1.0f - (v0_raw.y + 1.0f) / 2.0f) * height;

        	line(ax, ay, bx, by, framebuffer, red);
		}
    }


    for (auto& p : verts)
    {
        int px = (p.x + 1.0f) * width / 2.0f;
        int py = (p.y + 1.0f) * height / 2.0f;
        
        draw_fat_point(px, py, framebuffer, white, POINT_SIZE);
    }
}