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
		if (x < 0 || x >= WIDTH || y < 0 || y >= HEIGHT || /*z > 1 || z < -1 ||*/ z > depth_[y * WIDTH + x])
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
	void DrawLine3(Vector3f v0, Vector3f v1, color_t col = col_white) {
		Vector3f dv = Vector3f(0, 0, -dist);
		Vector2f p0 = Vector2f::pixels(Vector2f::proection(v0 + dv));
		Vector2f p1 = Vector2f::pixels(Vector2f::proection(v1 + dv));
		
		float x0 = p0.x_;
		float y0 = p0.y_;
		float x1 = p1.x_;
		float y1 = p1.y_;
		
		if (abs(x1 - x0) > abs(y1 - y0))
		{
			int add = x0 < x1 ? 1 : -1;
			float b = (y1 - y0) / (x1 - x0);
			float a = y0 - b * x0;
			
			for (int x = x0; x != x1; x += add)
				context_.put(round(a + b * x), x, -2, col);
		}
		else
		{
			int add = y0 < y1 ? 1 : -1;
			float b = (x1 - x0) / (y1 - y0);
			float a = x0 - b * y0;
			
			for (int y = y0; y != y1; y += add)
				context_.put(y, round(a + b * y), -2, col);
		}
	}
	
	void DrawTriangle(Vector3f v0, Vector3f v1, Vector3f v2, color_t col = col_white) {
		/*
		DrawLine3(v0, v1);
		DrawLine3(v0, v2);
		DrawLine3(v1, v2);
		*/
		
		Vector3f dv = Vector3f(0, 0, -dist);
		v0 += dv;
		v1 += dv;
		v2 += dv;
		
		Vector2f f0 = Vector2f::proection(v0);
		Vector2f f1 = Vector2f::proection(v1);
		Vector2f f2 = Vector2f::proection(v2);
		
		float D = Vector2f::determine(f1 - f0, f2 - f0);
		
		if (D == 0) return;
		
		float L, T, R, B;
		{
			Vector2f p0 = Vector2f::pixels(f0);
			Vector2f p1 = Vector2f::pixels(f1);
			Vector2f p2 = Vector2f::pixels(f2);
			
			L = std::min(p0.x_, std::min(p1.x_, p2.x_));
			R = std::max(p0.x_, std::max(p1.x_, p2.x_));
			T = std::max(p0.y_, std::max(p1.y_, p2.y_));
			B = std::min(p0.y_, std::min(p1.y_, p2.y_));
		}

		for(int x = L; x <= R && x >= 0; x++) {
			for(int y = B; y <= T && y >= 0; y++) {
				
				Vector2f f = Vector2f::floats(Vector2f(x, y));
				
				float b = Vector2f::determine(f  - f0, f2 - f0) / D;
				float g = Vector2f::determine(f1 - f0, f  - f0) / D;
				float a = 1 - b - g;
				
				if(b < 0 || g < 0 || a < 0)
					continue;
				
				color_t col_tmp = col_yellow;
				col_tmp.r = a * v0.getColor().r + b * v1.getColor().r + g * v2.getColor().r;
				col_tmp.g = a * v0.getColor().g + b * v1.getColor().g + g * v2.getColor().g;
				col_tmp.b = a * v0.getColor().b + b * v1.getColor().b + g * v2.getColor().b;
				
				Vector3f r = v0 * a + v1 * b + v2 * g;
				
				Vector3f l = Vector3f(1, 1, -dist*2/10) - r;
				l /= l.len();
				
				Vector3f n = (v1 - v0) * (v2 - v0);
				n /= n.len();
				
				Vector3f h = (l - r/r.len() ) / 2;
				h /= h.len();
				
				float d = n.scal(l);
				float s = n.scal(h);
				if (d < 0) d = 0;
				if (s < 0) s = 0;
				s*=s; /*2*/ s*=s; /*4*/ s*=s; /*8*/ s*=s; /*16*/ s*=s; /*32*/ s*=s; /*64*/ //s*=s; /*128*/
				
				float I = 0.2 + 0.3 * d + 0.5 * s;
				
				/*
				float R = 2;
				I = 1 - exp(-I * R);
				*/
				
				col_tmp.r = I * col_tmp.r;
				col_tmp.g = I * col_tmp.g;
				col_tmp.b = I * col_tmp.b;
				
				//изменения в кубе - попробуй запустить
				
				float z = 2.0 / (dist - (dist * 4)) * (r.getZ() + dist) - 1;
				
				if(x >= 0 && y >= 0 &&
				   x < WIDTH && y < HEIGHT)
					context_.put(y, x, z, col_tmp);
			}
		}
	}
	
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
		
		static Vector2f &floats(const Vector2f &v)
		{
			Vector2f *res = new Vector2f(0, 0);
			res->x_ = -1 + ((float) 2 * v.x_ + 1) / _WIDTH;
			res->y_ = -1 + ((float) 2 * v.y_ + 1) / _HEIGHT;
			res->col_ = v.col_;
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
		
		
		Vector2f &operator-(const Vector2f &v) const {
			return *(new Vector2f(x_ - v.x_, y_ - v.y_));
		}
	};
};

#endif // CANVAS_H_INCLUDED
