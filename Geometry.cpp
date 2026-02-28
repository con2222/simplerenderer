#include "Geometry.h"

constexpr double PI = 3.14159265358979323846;

double signed_triangle_area(int x1, int y1, int x2, int y2, int x3, int y3) {
    return 0.5 * (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));
}

vec3 rot(vec3 v) {
    constexpr double a = PI/6; // 30 degrees
    const matrix<3, 3> Ry = {{{std::cos(a), 0, std::sin(a)}, {0, 1, 0}, {-std::sin(a), 0, std::cos(a)}}};
    return Ry * v;
}

vec3 persp(vec3 v) {
    constexpr double c = 3.;
    return v / (1 - v.z/c);
}