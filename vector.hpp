#ifndef VECTOR_H_INCLUDED
#define VECTOR_H_INCLUDED

#include <cstdio>
#include <cstring>
#include <cmath>
#include <algorithm>

#include "matrix.hpp"

typedef struct {unsigned char b, g, r, a;} color_t;
const color_t col_red    = {0,   0,   255, 0};
const color_t col_green  = {0,   255, 0,   0};
const color_t col_blue   = {255, 0,   0,   0};

const color_t col_white  = {255, 255, 255, 0};
const color_t col_black  = {0,   0,   0,   0};

const color_t col_yellow = {0,   255, 255, 0};

class Vector3f
{
    float x_, y_, z_;
    color_t col_;
public:
    Vector3f()
    : x_(0)
    , y_(0)
    , z_(0)
    { }
    
    Vector3f(float x, float y, float z, color_t col = col_white)
    : x_(x)
    , y_(y)
    , z_(z)
    , col_(col)
    { }
    
    Vector3f(float arr[3])
    {
        for(int i = 0; i < 3; i++)
            (*this)[i] = arr[i];
    }
    
    float getX() const { return x_; }
    float getY() const { return y_; }
    float getZ() const { return z_; }
    color_t getColor() const { return col_; }
    
    void Init(float arr[3]) {
        for(int i = 0; i < 3; i++)
            (*this)[i] = arr[i];
    }
    
    void Init(float x, float y, float z) {
        x_ = x;
        y_ = y;
        z_ = z;
    }
    
    void setColor(color_t col) {
        col_ = col;
    }
    
    void Move(float x, float y, float z) {
        x_ += x;
        y_ += y;
        z_ += z;
    }
    
    float &operator[](int i) {
        switch (i) {
        case 0: return z_;
        case 1: return x_;
        case 2: return y_;
        
        default:
            std::terminate;
            break;
        }
        std::terminate();
    }
    
    float operator[](int i) const {
        switch (i) {
        case 0: return z_;
        case 1: return x_;
        case 2: return y_;
        
        default:
            std::terminate;
            break;
        }
        std::terminate();
    }
    
    Vector3f &operator+(const Vector3f &other) const {
        return *(new Vector3f(x_ + other.x_, y_ + other.y_, z_ + other.z_));
    }
    
    Vector3f &operator-(const Vector3f &other) const {
        return *(new Vector3f(x_ - other.x_, y_ - other.y_, z_ - other.z_));
    }
    
    Vector3f &operator/(int k) const {
        return *(new Vector3f(x_ / k, y_ / k, z_ / k));
    }
    
    Vector3f &operator*(int k) const {
        return *(new Vector3f(x_ * k, y_ * k, z_ * k));
    }
    
    Vector3f &operator*(const Matrix3f &m) const {
        Vector3f *res = new Vector3f(0, 0, 0);
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                (*res)[i] += m[i][j] * (*this)[j];
        return *res;
    }
    
    
    void operator*=(const Matrix3f &m) {
        float tmp[3] = {(*this)[0], (*this)[1], (*this)[2]};
        this->Init(0, 0, 0);
        for (int i = 0; i < 3; i++)
            for (int j = 0; j < 3; j++)
                (*this)[i] += m[i][j] * tmp[j];
    }
    
    void operator+=(const Vector3f &v) {
        for (int i = 0; i < 3; i++)
            (*this)[i] += v[i];
    }
    
    Vector3f &operator*(const Vector3f &v) {
        Vector3f *res = new Vector3f(0, 0, 0);
        for (int i = 0; i < 3; i++)
            (*res)[i] = (*this)[(i + 1) % 3] * v[(i + 2) % 3] - (*this)[(i + 2) % 3] * v[(i + 1) % 3];
        return *res;
    }
    
    void operator-=(const Vector3f &v) {
        for (int i = 0; i < 3; i++)
            (*this)[i] -= v[i];
    }
    
    void operator/=(int k) {
        x_ /= k;
        y_ /= k;
        z_ /= k;
    }
    
    void Dump() const {
        printf("{% .3f, % .3f, % .3f}\n", x_, y_, z_);
    }
};

#endif // VECTOR_H_INCLUDED
