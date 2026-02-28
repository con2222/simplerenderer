#pragma once

#include <cmath>
#include <cassert>
#include <iostream>

template<int n> struct vec {
    double data[n] = {0};
    double& operator[](const int i) {assert(i >= 0 && i < n); return data[i];}
    double  operator[](const int i) const { assert(i>=0 && i<n); return data[i]; }
};

template<> struct vec<2> {
    union {
        struct { double x, y; };
        double data[2];
    };

    vec<2>() : x(0), y(0) {}
    vec<2>(double x, double y) : x(x), y(y) {}

    double& operator[](const int i) {
        assert(i>=0 && i<2); return data[i];
    }
    double operator[](const int i) const {
        assert(i>=0 && i<2); return data[i];
    }
};

template<> struct vec<3> {
    union {
        struct { double x, y, z; };
        double data[3];
    };

    vec<3>() : x(0), y(0), z(0) {}
    vec<3>(double x, double y, double z) : x(x), y(y), z(z) {}

    double& operator[](const int i) {
        assert(i>=0 && i<3); return data[i];
    }
    double operator[](const int i) const {
        assert(i>=0 && i<3); return data[i];
    }
};

template<> struct vec<4> {
    union {
        struct { double x, y , z, w; };
        double data[4];
    };

    vec<4>() : x(0), y(0), z(0), w(0) {}
    vec<4>(double x, double y, double z, double w) : x(x), y(y), z(z), w(w) {}

    double& operator[](const int i) {
        assert(i>=0 && i<4);
        return data[i];
    }
    double operator[](const int i) const {
        assert(i>=0 && i<4);
        return data[i];
    }
};

typedef vec<2> vec2;
typedef vec<3> vec3;
typedef vec<4> vec4;

template<int n> vec<n> operator+(const vec<n>& v1, const vec<n>& v2) {
    vec<n> result;
    for (int i = 0; i < n; i++) {
        result[i] = v1[i] + v2[i];
    }
    return result;
}

template<int n> vec<n> operator-(const vec<n>& v1, const vec<n>& v2) {
    vec<n> result;
    for (int i = 0; i < n; i++) {
        result[i] = v1[i] - v2[i];
    }
    return result;
}

template<int n> vec<n> operator*(const vec<n>& v1, const double scalar) {
    vec<n> result;
    for (int i = 0; i < n; i++) {
        result[i] = v1[i] * scalar;
    }
    return result;
}

template<int n> vec<n> operator*(const double scalar, const vec<n>& v1) {
    return v1 * scalar;
}

template<int n> std::ostream& operator<<(std::ostream& out, const vec<n>& v) {
    for (int i = 0; i < n; i++) {
        out << v[i] << " ";
    }
    return out;
}

template<int n>
double norm(const vec<n>& v) {
    double s = 0;
    for (int i = 0; i < n; i++) {
        s += v[i] * v[i];
    }
    return std::sqrt(s);
}

template<int n>
vec<n> normalize(const vec<n>& v) {
    double nrm = norm(v);
    vec<n> res;
    if (nrm > 1e-5) {
        for (int i = 0; i < n; i++) {
            res[i] = v[i] / nrm;
        }
    }
    return res;
}

template<int n>
double dot(const vec<n>& v1, const vec<n>& v2) {
    double res = 0;
    for (int i = 0; i < n; i++) {
        res += v1[i] * v2[i];
    }
    return res;
}

inline vec3 cross(const vec3& v1, const vec3& v2) {
    vec3 result;
    result[0] = v1.y * v2.z - v1.z * v2.y;
    result[1] = v1.z * v2.x - v1.x * v2.z;
    result[2] = v1.x * v2.y - v1.y * v2.x;
    return result;
}

double signed_triangle_area(int x1, int y1, int x2, int y2, int x3, int y3);