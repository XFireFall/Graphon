#ifndef CANVAS_H_INCLUDED
#define CANVAS_H_INCLUDED

#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>

#include <cstdio>
#include <cstring>
#include <cmath>
#include <algorithm>

#include "matrix.hpp"
#include "vector.hpp"


//template <unsigned long HEIGHT, unsigned long WIDTH>
#define _HEIGHT (1080)
#define _WIDTH  (1920)
#define zNEAR (1000)
#define zFAR  (4000)

class Context
{
    color_t *screen_;
    float *depth_;
    int WIDTH, HEIGHT;
public:
    Context(size_t width, size_t height)
    : screen_((color_t *) calloc(width * height, sizeof(color_t)))
    , depth_((float *) calloc(width * height, sizeof(float)))
    , WIDTH(width)
    , HEIGHT(height)
    {
        Clear();
    }
    
   ~Context()
    {
        free(screen_);
        free(depth_);
    }
    
    void Clear(color_t col = col_black) {
        for (int y = 0; y < HEIGHT; y++)
            for (int x = 0; x < WIDTH; x++) {
                screen_[y * WIDTH + x] = col;
                depth_[y * WIDTH + x] = 1;
            }
    }
    
    void put(int y, int x, float z, color_t col = col_white) {
        if (x < 0 || x > WIDTH || y < 0 || y > HEIGHT || /*z > 1 || z < -1 ||*/ z > depth_[y * WIDTH + x])
            return;
        depth_[y * WIDTH + x] = z;        
        y = HEIGHT - 1 - y;
        screen_[y * WIDTH + x] = col;
    }
    
    color_t *operator[](int y) {
        y = HEIGHT - 1 - y;
        return screen_ + y * WIDTH;
    }
    
    void Send() {
        int fd = open("/dev/fb0", O_WRONLY);
        write(fd, screen_, WIDTH * HEIGHT * sizeof(color_t));
        close(fd);
    }
};








class Canvas
{
    Context context_;
    Vector3f o0_, oX_, oY_, oZ_;
    float dist = 10;
    const int WIDTH, HEIGHT;
public:
    Canvas(size_t width, size_t height)
    : context_(width, height)
    , WIDTH(width)
    , HEIGHT(height)
    , o0_(0, 0, 0)
    , oX_(1, 0, 0)
    , oY_(0, 1, 0)
    , oZ_(0, 0, 1)
    {
        Clear();
    }
    
    void Clear(color_t col = col_black) {
        context_.Clear();
    }
    
    void Rotate(const Matrix3f &m) { oX_ *= m; oY_ *= m; oZ_ *= m; }
    
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
        v0.Move(0, 0, -dist);v1.Move(0, 0, -dist);
        
        // ===================================
        
        float fx0 = -v0.getX() * zNEAR / (v0.getZ() * WIDTH);
        float fy0 = -v0.getY() * zNEAR / (v0.getZ() * HEIGHT);
        float fx1 = -v1.getX() * zNEAR / (v1.getZ() * WIDTH);
        float fy1 = -v1.getY() * zNEAR / (v1.getZ() * HEIGHT);
        
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
                    context_.put(y, x, -2, col);
                    //(*this)[y][x] = col;
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
                    context_.put(y, x, -2, col);
                    //(*this)[y][x] = col;
            }
        }
    }
    
    void DrawTriangle(Vector3f v0, Vector3f v1, Vector3f v2, color_t col = col_white) {
        /*DrawLine3(v0, v1);
        DrawLine3(v0, v2);
        DrawLine3(v1, v2);*/
        //return;
        
        /*Vector3f orientation = (v1 - v0) * (v2 - v0);
        if (orientation.getZ() > 0)
            return;*/
        
        v0.Move(0, 0, -dist);v1.Move(0, 0, -dist);v2.Move(0, 0, -dist);
        
        float fx0 = -v0.getX() * zNEAR / (v0.getZ() * WIDTH);
        float fy0 = -v0.getY() * zNEAR / (v0.getZ() * HEIGHT);
        float fx1 = -v1.getX() * zNEAR / (v1.getZ() * WIDTH);
        float fy1 = -v1.getY() * zNEAR / (v1.getZ() * HEIGHT);
        float fx2 = -v2.getX() * zNEAR / (v2.getZ() * WIDTH);
        float fy2 = -v2.getY() * zNEAR / (v2.getZ() * HEIGHT);
        
        Vector2f f0 = Vector2f::proection(v0);
        Vector2f f1 = Vector2f::proection(v1);
        Vector2f f2 = Vector2f::proection(v2);

        float x0 = round((fx0 + 1) * WIDTH  / 2 - 1);
        float y0 = round((fy0 + 1) * HEIGHT / 2 - 1);
        float x1 = round((fx1 + 1) * WIDTH  / 2 - 1);
        float y1 = round((fy1 + 1) * HEIGHT / 2 - 1);
        float x2 = round((fx2 + 1) * WIDTH  / 2 - 1);
        float y2 = round((fy2 + 1) * HEIGHT / 2 - 1);
        
        Vector2f p0 = Vector2f::pixels(f0);
        Vector2f p1 = Vector2f::pixels(f1);
        Vector2f p2 = Vector2f::pixels(f2);
        
        //float D = (fx1 - fx0)*(fy2 - fy0) - (fx2 - fx0)*(fy1 - fy0);
        float D = Vector2f::determine(f1 - f0, f2 - f0);
        
        //if(D < 0)return;
        
        //printf("%f %f\n", fx0, fy0);
        //printf("%f %f\n", x0, y0);
        
        float L = std::min(x0, std::min(x1, x2));
        float R = std::max(x0, std::max(x1, x2));
        
        float T = std::max(y0, std::max(y1, y2));
        float B = std::min(y0, std::min(y1, y2));
        //printf("%d %d -> %d %d\n", L, B, R, T);

        for(int x = L; x <= R && x >= 0; x++) {
            for(int y = B; y <= T && y >= 0; y++) {
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
                
                color_t col_tmp = col_yellow;
                col_tmp.r = a * v0.getColor().r + b * v1.getColor().r + g * v2.getColor().r;
                col_tmp.g = a * v0.getColor().g + b * v1.getColor().g + g * v2.getColor().g;
                col_tmp.b = a * v0.getColor().b + b * v1.getColor().b + g * v2.getColor().b;
                
                fx = v0.getX() * a + v1.getX() * b + v2.getX() * g;
                fy = v0.getY() * a + v1.getY() * b + v2.getY() * g;
                float fz = v0.getZ() * a + v1.getZ() * b + v2.getZ() * g; // < 0
                float k = (float) 2 / (dist - ((float)4 * dist));
                float d = k * (fz + dist) - 1;
                
                /*puts("");
                printf("%10f\t|\t%10f\t|\t%10f\n", v0.getZ(), v1.getZ(), v2.getZ());
                printf("%10f\t|\t%10f\t|\t%10f\n", a, b, g);
                printf("%10f\t|\t%10f\t\n", z, d);*/
                //unsigned char b_val = 128 + (unsigned char)((d + 1) * 255);
                //col_tmp = {255, 0, 0};
                //col_tmp = {};
                //col_tmp.b = (unsigned char)((-d + 2)*255);
                /*if(col_tmp.b < 128)
                {
                    col_tmp.b = 0;
                    col_tmp.r = 255;
                }*/
                
                Vector3f r = Vector3f(fx, fy, fz);
                Vector3f vb = v1 - v0;
                Vector3f vc = v2 - v0;
                
                Vector3f l = Vector3f(1, 1, -dist/2) - r;
                Vector3f c = Vector3f(-fx, -fy, -fz);
                Vector3f n = vb * vc;
                
                //DrawLine3(l + r + Vector3f(0, 0, +dist), r + Vector3f(0, 0, +dist));
                //DrawLine3(l + r + Vector3f(0, 0, +dist), Vector3f(.1, .1, .1) + Vector3f(0, 0, +dist));
                
                l /= l.len();
                c /= c.len();
                n /= n.len();
                
                Vector3f h = (l + c) / 2;
                //h /= h.len();
                
                //DrawLine3(r + n + Vector3f(0, 0, +dist), r + Vector3f(0, 0, +dist));
                
                float s = 1; //n.scal(h);
                //s*=s*s;
                /*s*=s;//4
                s*=s;//8
                s*=s;//16
                s*=s;//32*/
                //float I = 0.2 + 0.3 * n.scal(l) + 0.5 * s;
                float I = 0.2 + 0.8 * n.scal(l);
                //I = abs(I);
                if (I > 1) I = 1;
                if (I < 0) I = 0;
                col_tmp.r = I * col_tmp.r;
                col_tmp.g = I * col_tmp.g;
                col_tmp.b = I * col_tmp.b;
                                
                if(x >= 0 && y >= 0 &&
                   x < WIDTH && y < HEIGHT)
                    context_.put(y, x, d, col_tmp);
                    //(*this)[y][x] = col_tmp;
            }
        }
    }
    
    /*color_t *operator[](int y) {
        return context_[HEIGHT - y - 1];
    }*/
    
    void Display() {
        context_.Send();
    }
    
    class Vector2f
    {
    public:
        float x_, y_;
        color_t col_;
    //public:
        Vector2f()
        : x_(0)
        , y_(0)
        , col_(col_white)
        { }
        
        Vector2f(float x, float y, color_t col = col_white)
        : x_(x)
        , y_(y)
        , col_(col)
        { }
        
        static Vector2f &proection(const Vector3f &v)
        {
            Vector2f *res = new Vector2f(0, 0);
            res->x_ = -v.getX() * zNEAR / (v.getZ() * _WIDTH);
            res->y_ = -v.getY() * zNEAR / (v.getZ() * _HEIGHT);
            res->col_ = v.getColor();
            return *res;
        }
        
        static Vector2f &pixels(const Vector2f &v)
        {
            Vector2f *res = new Vector2f(0, 0);
            res->x_ = round((v.x_ + 1) * _WIDTH  / 2 - 1);
            res->y_ = round((v.y_ + 1) * _HEIGHT / 2 - 1);
            res->col_ = v.col_;
            return *res;
        }
        
        static float determine(const Vector2f &v1, const Vector2f &v2)
        {
            return v1.x_ * v2.y_ - v2.x_ * v1.y_;
        }
        
        
        Vector2f &operator-(const Vector2f &other) const {
            return *(new Vector2f(x_ - other.x_, y_ - other.y_));
        }
    };
};

#endif // CANVAS_H_INCLUDED
