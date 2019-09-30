#ifndef CUBE_H_INCLUDED
#define CUBE_H_INCLUDED

#include <time.h>

#include "vector.hpp"
#include "canvas.hpp"

class Cube
{
    Vector3f points_[8];
    Vector3f center_;
public:
    Cube(float arr[8][3])
    : points_()
    , center_(0, 0, 0)
    {
        srandom((unsigned long) time(NULL));
        for (int i = 0; i < 8; i++) {
            points_[i].Init(arr[i]);
            //points_[i].setColor({random() % 255, random() % 255, random() % 255});
            points_[i].setColor(col_red);
        }
    }
    
    void Rotate(Matrix3f &m) {
        for (int i = 0; i < 8; i++)
            points_[i] -= center_;
        
        for (int i = 0; i < 8; i++)
            points_[i] *= m;
        
        for (int i = 0; i < 8; i++)
            points_[i] += center_;
    }
    
    void setCenter(const Vector3f &v) {
        for (int i = 0; i < 8; i++)
            points_[i] += v - center_;
        center_ = Vector3f(v);
    }
    
    void Draw(Canvas &canvas) {        
        canvas.DrawTriangle(points_[0], points_[1], points_[2]);
        canvas.DrawTriangle(points_[2], points_[3], points_[0]);
        
        canvas.DrawTriangle(points_[4], points_[7], points_[6]);
        canvas.DrawTriangle(points_[6], points_[5], points_[4]);
        
        canvas.DrawTriangle(points_[0], points_[4], points_[5]);
        canvas.DrawTriangle(points_[5], points_[1], points_[0]);
        
        canvas.DrawTriangle(points_[3], points_[2], points_[6]);
        canvas.DrawTriangle(points_[6], points_[7], points_[3]);
        
        canvas.DrawTriangle(points_[2], points_[1], points_[5]);
        canvas.DrawTriangle(points_[5], points_[6], points_[2]);
        
        canvas.DrawTriangle(points_[3], points_[7], points_[4]);
        canvas.DrawTriangle(points_[4], points_[0], points_[3]);
        
        return;
        
        {
            canvas.DrawLine3(points_[0], points_[1]);
            canvas.DrawLine3(points_[0], points_[2]);
            canvas.DrawLine3(points_[0], points_[3]);
            
            canvas.DrawLine3(points_[7], points_[4]);
            canvas.DrawLine3(points_[7], points_[5]);
            canvas.DrawLine3(points_[7], points_[6]);
            
            canvas.DrawLine3(points_[4], points_[2]);
            canvas.DrawLine3(points_[4], points_[1]);
            
            canvas.DrawLine3(points_[5], points_[3]);
            canvas.DrawLine3(points_[5], points_[1]);
            
            canvas.DrawLine3(points_[6], points_[2]);
            canvas.DrawLine3(points_[6], points_[3]);
        }
    }
    
    void Dump() {
        puts("Cube dump:");
        for(int i = 0; i < 8; i++) {
            printf("\t");
            points_[i].Dump();
        }
    }
};

#endif // CUBE_H_INCLUDED
