#pragma once
#include "vec3.h"
#include "mat4.h"

class vec4 {
public:
	double x, y, z, w;

public:
	vec4();
	vec4(const vec3 &vec, double w);
	vec4(double x, double y, double z, double w);

	vec4& set(double x, double y, double z, double w);

	vec4& zero(void);

	vec4			operator- (void) const;
	vec4			operator+ (void) const;

	vec4			operator+ (const vec4& v) const;
	vec4			operator- (const vec4& v) const;
	vec4			operator* (double scalar) const;
	vec4			operator/ (double scalar) const;

	vec4& operator= (const vec4& v);
	vec4& operator+=(const vec4& v);
	vec4& operator-=(const vec4& v);
	vec4& operator*=(double scalar);
	vec4& operator/=(double scalar);

	double& operator[](int index);
	const double& operator[](int index) const;

	double			dot(const vec4& v) const;

	double			magnitude(void) const;
	vec4& normalize(void);

	double* ptr(void);
	const double* ptr(void) const;

public:
    // Used when order is `scalar * vec4`
	// Or `mat4 * vec4`
    // Need friend to access private fields of vec4
	friend vec4		operator*(double scalar, const vec4& v);
	friend vec4		operator*(const mat4& mat, const vec4& v);
};

