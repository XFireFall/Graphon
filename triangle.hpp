#ifndef TRIANGLE_H_INCLUDED
#define TRIANGLE_H_INCLUDED

#include <cstdio>
#include <cstring>
#include <cmath>
#include <algorithm>

#include "canvas.hpp"
#include "vector.hpp"
#include "matrix.hpp"

class Triangle
{
    Vector3f a_, b_, c_;
public:
    Triangle(const Vector3f &a, const Vector3f &b, const Vector3f &c)
    : a_(a)
    , b_(b)
    , c_(c)
    { }
    
    void Rotate(Matrix3f &m) {
        a_ *= m;
        b_ *= m;
        c_ *= m;
    }
    
    void Draw(Canvas &canvas) {
        canvas.DrawTriangle(a_, b_, c_);
        return;
        
        canvas.DrawLine3(a_, b_);
        canvas.DrawLine3(b_, c_);
        canvas.DrawLine3(c_, a_);
    }
};

#endif // TRIANGLE_H_INCLUDED
