#ifndef MATRIX_H_INCLUDED
#define MATRIX_H_INCLUDED

#include <cstdio>
#include <cstring>
#include <cmath>
#include <algorithm>

typedef float vec3_t[3];

class Matrix3f
{
	float arr_[3][3];
public:
	Matrix3f(float const arr[9])
	{
		memcpy(arr_, arr, 9 * sizeof(float));
	}
	
	Matrix3f(float const arr[3][3])
	{
		memcpy(arr_, arr, 9 * sizeof(float));
	}
	
	Matrix3f(float F, float T)
	{
		// unused
		
		float sF = sin(F),
			cF = cos(F);
		float sT = sin(T),
			cT = cos(T);
		
		float arr[3][3] = {
			{ cF     , 0 , -sF     },
			{-sT * sF, cT, -sT * cF},
			{ cT * sF, sT,  cT * cF}};
		
		memcpy(arr_, arr, 9 * sizeof(float));
		/*
		
		float arr2[3][3] = {
			{ 1,  0 , 0 },
			{ 0,  cT, sT},
			{ 0, -sT, cT}};
		Matrix3f tmp = Matrix3f(arr2);
		up_ *= tmp;*/
		//Transpose() ;
	}
	
	void Transpose() {
		for (int i = 0; i < 3; i++)
			for (int j = 0; j < 3; j++)
				std::swap(arr_[i][j], arr_[j][i]);
	}
	
	float const (&operator[](int i) const)[3] {
		return arr_[i];
	}
	
	Matrix3f &operator+(const Matrix3f &other) const {
		Matrix3f *res = new Matrix3f(*this);
		for(int i = 0; i < 3; i++)
			for(int j = 0; j < 3; j++)
				res->arr_[i][j] += other.arr_[i][j];
		return *res;
	}
	
	void Dump() const {
		for(int i = 0; i < 3; i++)
			printf("|% .3f % .3f % .3f|\n", arr_[i][0], arr_[i][1], arr_[i][2]);
	}
};

#endif // MATRIX_H_INCLUDED
