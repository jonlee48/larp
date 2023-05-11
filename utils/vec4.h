#pragma once
#include "vec3.h"
#include "mat4.h"

class vec4 {
public:
	float x, y, z, w;

public:
	vec4();
	vec4(const vec3 &vec, float w);
	vec4(float x, float y, float z, float w);

	vec4& set(float x, float y, float z, float w);

	vec4& zero(void);

	vec4			operator- (void) const;
	vec4			operator+ (void) const;

	vec4			operator+ (const vec4& v) const;
	vec4			operator- (const vec4& v) const;
	vec4			operator* (float scalar) const;
	vec4			operator/ (float scalar) const;

	vec4& operator= (const vec4& v);
	vec4& operator+=(const vec4& v);
	vec4& operator-=(const vec4& v);
	vec4& operator*=(float scalar);
	vec4& operator/=(float scalar);

	float& operator[](int index);
	const float& operator[](int index) const;

	float			dot(const vec4& v) const;

	float			magnitude(void) const;
	vec4& normalize(void);

	float* ptr(void);
	const float* ptr(void) const;

public:
    // Used when order is `scalar * vec4`
	// Or `mat4 * vec4`
    // Need friend to access private fields of vec4
	friend vec4		operator*(float scalar, const vec4& v);
	friend vec4		operator*(const mat4& mat, const vec4& v);
};

