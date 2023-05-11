#include "vec3.h"
#include <assert.h>
#include <stdio.h>
#include <cmath>

vec3::vec3() 
{
	x = y = z = 0;
}

vec3::vec3(double x, double y, double z) 
{
	this->x = x;
	this->y = y;
	this->z = z;
}

vec3& vec3::set(double x, double y, double z) {
	this->x = x;
	this->y = y;
	this->z = z;

	return *this;
}

vec3& vec3::zero(void) {
	x = y = z = 0;

	return *this;
}

vec3 vec3::operator-(void) const {
	return vec3(-x, -y, -z);
}

vec3 vec3::operator+(void) const {
	return vec3(x, y, z);
}

vec3 vec3::operator+(const vec3& v) const {
	return vec3(x + v.x, y + v.y, z + v.z);
}

vec3 vec3::operator-(const vec3& v) const {
	return vec3(x - v.x, y - v.y, z - v.z);
}

vec3 vec3::operator*(double scalar) const {
	return vec3(x * scalar, y * scalar, z * scalar);
}

vec3 vec3::operator/(double scalar) const {
	double inv = 1.0f / scalar;
	return vec3(x * inv, y * inv, z * inv);
}

vec3& vec3::operator=(const vec3& v) {
	x = v.x;
	y = v.y;
	z = v.z;
	return *this;
}

vec3& vec3::operator+=(const vec3& v) {
	x += v.x;
	y += v.y;
	z += v.z;
	return *this;
}

vec3& vec3::operator-=(const vec3& v) {
	x -= v.x;
	y -= v.y;
	z -= v.z;
	return *this;
}

vec3& vec3::operator*=(double scalar) {
	x *= scalar;
	y *= scalar;
	z *= scalar;
	return *this;
}

vec3& vec3::operator/=(double scalar) {
	double inv = 1.0f / scalar;
	x *= inv;
	y *= inv;
	z *= inv;
	return *this;
}

double& vec3::operator[](int index) {
	assert(index >= 0 && index < 3);
	return (&x)[index];
}

const double& vec3::operator[](int index) const {
	assert(index >= 0 && index < 3);
	return (&x)[index];
}

double vec3::dot(const vec3& v) const {
	return x * v.x + y * v.y + z * v.z;
}

vec3 vec3::cross(const vec3& v) const {
	vec3 result(0, 0, 0);
	result[0] = y * v.z - z * v.y;
	result[1] = z * v.x - x * v.z;
	result[2] = x * v.y - y * v.x;
	return result;
}

double vec3::magnitude(void) const {
	return sqrtf(x * x + y * y + z * z);
}

vec3& vec3::normalize(void) {
	double mag = sqrtf(x * x + y * y + z * z);

	if (mag < 1e-6f) {
		x = y = z = 0;
	}
	else {
		double inv = 1.0f / mag;

		x *= inv;
		y *= inv;
		z *= inv;
	}

	return *this;
}

double* vec3::ptr(void) {
	return &x;
}

const double* vec3::ptr(void) const {
	return &x;
}

vec3 operator*(double scalar, const vec3& v) {
    printf("x: %f y: %f z: %f\n", v.x * scalar, v.y * scalar, v.z * scalar);
	return vec3(v.x * scalar, v.y * scalar, v.z * scalar);
}
