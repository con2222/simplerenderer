#include "Geometry.h"

double signed_triangle_area(int x1, int y1, int x2, int y2, int x3, int y3) {
    return 0.5 * (x1 * (y2 - y3) + x2 * (y3 - y1) + x3 * (y1 - y2));
}