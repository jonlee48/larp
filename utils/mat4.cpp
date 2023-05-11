#include "mat4.h"
#include <stdio.h>
#include <assert.h>

mat4::mat4(double diagonal) 
{
	for (int i = 0; i < 16; i++) 
	{
		if (i % 5 == 0) 
		{
			mat[i] = diagonal;
		}
		else 
		{
			mat[i] = 0;
		}
	}
}

mat4::mat4(double e0, double e1, double e2, double e3, 
           double e4, double e5, double e6, double e7, 
           double e8, double e9, double e10, double e11, 
           double e12, double e13, double e14, double e15) 
{
	mat[0] = e0;
	mat[1] = e1;
	mat[2] = e2;
	mat[3] = e3;
	mat[4] = e4;
	mat[5] = e5;
	mat[6] = e6;
	mat[7] = e7;
	mat[8] = e8;
	mat[9] = e9;
	mat[10] = e10;
	mat[11] = e11;
	mat[12] = e12;
	mat[13] = e13;
	mat[14] = e14;
	mat[15] = e15;
}

double mat4::operator [](int index) const 
{
	assert(index >= 0 && index < 16);
	return mat[index];
}
double& mat4::operator [](int index) 
{
	assert(index >= 0 && index < 16);
	return mat[index];
}

mat4 mat4::operator *(const mat4& m) const 
{
	mat4 result(0);

	int result_index = 0;
	for (size_t i = 0; i < 4; i++)
	{
		for (size_t j = 0; j < 4; j++)
		{
			double sum = 0;
			for (size_t k = 0; k < 4; k++)
			{
				// For column-major order (OpenGL)
				// int m1_row = j + k * 4;
				// int m2_col = i * 4 + k;
				// Row major order (matches math notations)
				int m1_row = i * 4 + k;
				int m2_col = j + k * 4;
				sum += (mat[m1_row] * m[m2_col]);
				// printf("i: %d, j: %d, m1_row: %d, m2_col: %d\n", (int)i, (int)j, m1_row, m2_col);
			}
			result[result_index] = sum;
			result_index++;
			// printf(" sum is:%f\n", sum);
		}
	}
	return result;
}