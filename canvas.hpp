#ifndef CANVAS_H_INCLUDED
#define CANVAS_H_INCLUDED

#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cmath>
#include <algorithm>

#include "matrix.hpp"
#include "vector.hpp"


//template <unsigned long HEIGHT, unsigned long WIDTH>
#define HEIGHT (1080)
#define WIDTH  (1920)
#define DEPTH  (1000)
#define FARTH  (4000)
class Canvas
{
    color_t screen_[HEIGHT][WIDTH];
    color_t (&mem)[HEIGHT][WIDTH] = screen_;
    Vector3f o0_, oX_, oY_, oZ_;
     
    /*void put(int y, int x, float d, color_t col = col_white) {
        if(x < 0 || x > WIDTH || y < 0 || y > HEIGHT || d < depth_[y][x])
            return;
        depth_[y][x] = d;
        screen_[HEIGHT - 1 - y][x] = col;
    }*/
     
public:
    Canvas()
    : o0_(.0, .0, .0)
    , oX_(.1, .0, .0)
    , oY_(.0, .1, .0)
    , oZ_(.0, .0, .1)
    {
        Clear();
    }
    
    void Clear(color_t col = col_black) {
        for (int y = 0; y < HEIGHT; y++)
            for (int x = 0; x < WIDTH; x++) {
                screen_[y][x] = col;
                //depth_[y][x] = 1;
            }
    }
    
    void Rotate(const Matrix3f &m) {
        oX_ *= m;
        oY_ *= m;
        oZ_ *= m;
    }
    
    void DrawZoX() {
        Vector3f v = Vector3f(oX_);
        Vector3f z = Vector3f(o0_);
        for (int i = 0; i < 10; i++) {
            z += oZ_ / 10;
            DrawLine3(z, z + v);
        }
        
        v = Vector3f(oZ_);
        z = Vector3f(o0_);
        for (int i = 0; i < 10; i++) {
            z += oX_ / 10;
            DrawLine3(z, z + v);
        }
    }
    
    void DrawChords() {
        DrawLine3(o0_, oX_, col_red);
        DrawLine3(o0_, oY_, col_green);
        DrawLine3(o0_, oZ_, col_blue);
    }
    
    //void DrawLine3(const Vector3f &v0, const Vector3f &v1) { 
    void DrawLine3(Vector3f &v0, Vector3f &v1, color_t col = col_white) { 
        
        float dist = 0.4;
        v0.Move(0, 0, -dist);v1.Move(0, 0, -dist);
        
        // ===================================
        
        float fx0 = -v0.getX() * DEPTH / (v0.getZ() * WIDTH);
        float fy0 = -v0.getY() * DEPTH / (v0.getZ() * HEIGHT);
        float fx1 = -v1.getX() * DEPTH / (v1.getZ() * WIDTH);
        float fy1 = -v1.getY() * DEPTH / (v1.getZ() * HEIGHT);
        
        // ===================================
        
        v0.Move(0, 0, dist);v1.Move(0, 0, dist);
        
        // ===================================
        
        float x0 = round((fx0 + 1) * WIDTH  / 2 - 1);
        float y0 = round((fy0 + 1) * HEIGHT / 2 - 1);
        float x1 = round((fx1 + 1) * WIDTH  / 2 - 1);
        float y1 = round((fy1 + 1) * HEIGHT / 2 - 1);
        
        if (abs(x1 - x0) > abs(y1 - y0))
        {
            int add = x0 < x1 ? 1 : -1;
            float b = (y1 - y0) / (x1 - x0);
            float a = y0 - b * x0;
            
            for (int x = x0; x != x1 && x >= 0 && x < WIDTH; x += add) {
                int y = round(a + b * x);
                
                if(x >= 0 && y >= 0 &&
                   x < WIDTH && y < HEIGHT)
                    //put(y, x, d, col);
                    (*this)[y][x] = col;
            }
        }
        else //if (abs(x1 - x0) < abs(y1 - y0))
        {
            int add = y0 < y1 ? 1 : -1;
            float b = (x1 - x0) / (y1 - y0);
            float a = x0 - b * y0;
            
            for (int y = y0; y != y1 && y >= 0 && y < HEIGHT; y += add) {
                int x = round(a + b * y);
                
                if(x >= 0 && y >= 0 &&
                   x < WIDTH && y < HEIGHT)
                    (*this)[y][x] = col;
            }
        }
    }
    
    void DrawTriangle(Vector3f &v0, Vector3f &v1, Vector3f &v2, color_t col = col_white) {
        /*DrawLine3(v0, v1);
        DrawLine3(v0, v2);
        DrawLine3(v1, v2);*/
        //return;
        
        /*Vector3f orientation = (v1 - v0) * (v2 - v0);
        if (orientation.getZ() > 0)
            return;*/
        
        float dist = 0.4;
        v0.Move(0, 0, -dist);v1.Move(0, 0, -dist);v2.Move(0, 0, -dist);
        
        float fx0 = -v0.getX() * DEPTH / (v0.getZ() * WIDTH);
        float fy0 = -v0.getY() * DEPTH / (v0.getZ() * HEIGHT);
        float fx1 = -v1.getX() * DEPTH / (v1.getZ() * WIDTH);
        float fy1 = -v1.getY() * DEPTH / (v1.getZ() * HEIGHT);
        float fx2 = -v2.getX() * DEPTH / (v2.getZ() * WIDTH);
        float fy2 = -v2.getY() * DEPTH / (v2.getZ() * HEIGHT);
        
        v0.Move(0, 0, dist);v1.Move(0, 0, dist);v2.Move(0, 0, dist);
        
        float x0 = round((fx0 + 1) * WIDTH  / 2 - 1);
        float y0 = round((fy0 + 1) * HEIGHT / 2 - 1);
        float x1 = round((fx1 + 1) * WIDTH  / 2 - 1);
        float y1 = round((fy1 + 1) * HEIGHT / 2 - 1);
        float x2 = round((fx2 + 1) * WIDTH  / 2 - 1);
        float y2 = round((fy2 + 1) * HEIGHT / 2 - 1);
        
        float D = (fx1 - fx0)*(fy2 - fy0) - (fx2 - fx0)*(fy1 - fy0);
        
        //printf("%f %f\n", fx0, fy0);
        //printf("%f %f\n", x0, y0);
        
        float L = std::min(x0, std::min(x1, x2));
        float R = std::max(x0, std::max(x1, x2));
        
        float T = std::max(y0, std::max(y1, y2));
        float B = std::min(y0, std::min(y1, y2));
        //printf("%d %d -> %d %d\n", L, B, R, T);

        for(int x = L; x < R && x >= 0; x++) {
            for(int y = B; y < T && y >= 0; y++) {
                //continue;
                float fx = -1 + ((float) 2 * x + 1) / WIDTH;
                float fy = -1 + ((float) 2 * y + 1) / HEIGHT;
                
                float D1 = (fx - fx0)*(fy2 - fy0) - (fx2 - fx0)*(fy - fy0);
                float D2 = (fx1 - fx0)*(fy - fy0) - (fx - fx0)*(fy1 - fy0);
                
                //if(D == 0) continue;
                
                float b = D1 / D;
                float g = D2 / D;
                
                float a = 1 - b - g;
                
                if(b < 0 || g < 0 || a < 0)
                    continue;
                
                color_t col_tmp;
                col_tmp.r = a * v0.getColor().r + b * v1.getColor().r + g * v2.getColor().r;
                col_tmp.g = a * v0.getColor().g + b * v1.getColor().g + g * v2.getColor().g;
                col_tmp.b = a * v0.getColor().b + b * v1.getColor().b + g * v2.getColor().b;
                
                
                if(x >= 0 && y >= 0 &&
                   x < WIDTH && y < HEIGHT)
                    (*this)[y][x] = col_tmp;
            }
        }
    }
    
    color_t const &at(float fy, float fx) const {
        int px = round((fx + 1) * WIDTH  / 2 - 1);
        int py = round((fy + 1) * HEIGHT / 2 - 1);
        return (*this)[py][px];
    }
    
    color_t &at(float fy, float fx) {
        int px = round((fx + 1) * WIDTH  / 2 - 1);
        int py = round((fy + 1) * HEIGHT / 2 - 1);
        return (*this)[py][px];
    }
    
    color_t const (&operator[](int y) const)[WIDTH] {
        return screen_[HEIGHT - y - 1];
    }
    
    color_t (&operator[](int y))[WIDTH] {
        return screen_[HEIGHT - y - 1];
    }
    
    void Display() {
        int fd = open("/dev/fb0", O_WRONLY);
        write(fd, screen_, WIDTH * HEIGHT * sizeof(color_t));
        close(fd);
    }
};


#endif // CANVAS_H_INCLUDED
