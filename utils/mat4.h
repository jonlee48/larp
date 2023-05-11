#pragma once

class mat4 
{
public:
	double mat[16];

public:
	mat4(double diagonal);
	mat4(double e0, double e1, double e2, double e3, 
         double e4, double e5, double e6, double e7, 
         double e8, double e9, double e10, double e11, 
         double e12, double e13, double e14, double e15);

    // Operator overloading:
    // https://www.programiz.com/cpp-programming/operator-overloading
    // `const` at the end means none of the data members will be modified

    /*
     * Overload * operator to compute matrix multiplication
     */
	mat4 operator *(const mat4& m) const;
	double operator [](int index) const;

    // & is a reference to mat index for assignment
    // i.e. mat[2] = 12.3;
    // https://isocpp.org/wiki/faq/references#returning-refs
	double& operator [](int index);
};

