#pragma once

class mat4 
{
private:
	float mat[16];

public:
	mat4(float diagonal);
	mat4(float e0, float e1, float e2, float e3, 
         float e4, float e5, float e6, float e7, 
         float e8, float e9, float e10, float e11, 
         float e12, float e13, float e14, float e15);

    // Operator overloading:
    // https://www.programiz.com/cpp-programming/operator-overloading
    // `const` at the end means none of the data members will be modified

    /*
     * Overload * operator to compute matrix multiplication
     */
	mat4 operator *(const mat4& m) const;
	float operator [](int index) const;

    // & is a reference to mat index for assignment
    // i.e. mat[2] = 12.3;
    // https://isocpp.org/wiki/faq/references#returning-refs
	float& operator [](int index);
};

