#include "vec4.h"
#include <assert.h>
#include <stdio.h>
#include <cmath>

vec4::vec4() 
{
	x = y = z = w = 0;
}

vec4::vec4(const vec3 &vec, double w) 
{
	this->x = vec.x;
	this->y = vec.y;
	this->z = vec.z;
	this->w = w;
}

vec4::vec4(double x, double y, double z, double w) 
{
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;
}

vec4& vec4::set(double x, double y, double z, double w) {
	this->x = x;
	this->y = y;
	this->z = z;
	this->w = w;

	return *this;
}

vec4& vec4::zero(void) {
	x = y = z = w = 0;

	return *this;
}

vec4 vec4::operator-(void) const {
	return vec4(-x, -y, -z, -w);
}

vec4 vec4::operator+(void) const {
	return vec4(x, y, z, w);
}

vec4 vec4::operator+(const vec4& v) const {
	return vec4(x + v.x, y + v.y, z + v.z, w + v.w);
}

vec4 vec4::operator-(const vec4& v) const {
	return vec4(x - v.x, y - v.y, z - v.z, w - v.w);
}

vec4 vec4::operator*(double scalar) const {
	return vec4(x * scalar, y * scalar, z * scalar, w * scalar);
}

vec4 vec4::operator/(double scalar) const {
	double inv = 1.0f / scalar;
	return vec4(x * inv, y * inv, z * inv, w * inv);
}

vec4& vec4::operator=(const vec4& v) {
	x = v.x;
	y = v.y;
	z = v.z;
	w = v.w;
	return *this;
}

vec4& vec4::operator+=(const vec4& v) {
	x += v.x;
	y += v.y;
	z += v.z;
	w += v.w;
	return *this;
}

vec4& vec4::operator-=(const vec4& v) {
	x -= v.x;
	y -= v.y;
	z -= v.z;
	w -= v.w;
	return *this;
}

vec4& vec4::operator*=(double scalar) {
	x *= scalar;
	y *= scalar;
	z *= scalar;
	w *= scalar;
	return *this;
}

vec4& vec4::operator/=(double scalar) {
	double inv = 1.0f / scalar;
	x *= inv;
	y *= inv;
	z *= inv;
	w *= inv;
	return *this;
}

double& vec4::operator[](int index) {
	assert(index >= 0 && index < 4);
	return (&x)[index];
}

const double& vec4::operator[](int index) const {
	assert(index >= 0 && index < 4);
	return (&x)[index];
}

double vec4::dot(const vec4& v) const {
	return x * v.x + y * v.y + z * v.z + w * v.w;
}

double vec4::magnitude(void) const {
	return sqrtf(x * x + y * y + z * z + w * w);
}

vec4& vec4::normalize(void) {
	double mag = sqrtf(x * x + y * y + z * z + w * w);

	if (mag < 1e-6f) {
		x = y = z = w = 0;
	}
	else {
		double inv = 1.0f / mag;

		x *= inv;
		y *= inv;
		z *= inv;
		w *= inv;
	}

	return *this;
}

double* vec4::ptr(void) {
	return &x;
}

const double* vec4::ptr(void) const {
	return &x;
}

vec4 operator*(double scalar, const vec4& v) {
    printf("x: %f y: %f z: %f w: %f\n", v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar);
	return vec4(v.x * scalar, v.y * scalar, v.z * scalar, v.w * scalar);
}

vec4 operator*(const mat4& mat, const vec4& v) {
	double x = mat[0] * v.x + mat[1] * v.y + mat[2] * v.z + mat[3] * v.w;
	double y = mat[4] * v.x + mat[5] * v.y + mat[6] * v.z + mat[7] * v.w;
	double z = mat[8] * v.x + mat[9] * v.y + mat[10] * v.z + mat[11] * v.w;
	double w = mat[12] * v.x + mat[13] * v.y + mat[14] * v.z + mat[15] * v.w;
	return vec4(x, y, z, w);
}