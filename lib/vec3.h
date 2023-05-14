#pragma once

class vec3 {
public:
	float x, y, z;

public:
	vec3();
	vec3(float x, float y, float z);

	vec3& set(float x, float y, float z);

	vec3& zero(void);

	vec3			operator- (void) const;
	vec3			operator+ (void) const;

	vec3			operator+ (const vec3& v) const;
	vec3			operator- (const vec3& v) const;
	vec3			operator* (float scalar) const;
	vec3			operator/ (float scalar) const;

	vec3& operator= (const vec3& v);
	vec3& operator+=(const vec3& v);
	vec3& operator-=(const vec3& v);
	vec3& operator*=(float scalar);
	vec3& operator/=(float scalar);

	float& operator[](int index);
	const float& operator[](int index) const;

	float			dot(const vec3& v) const;
	vec3			cross(const vec3& v) const;

	float			magnitude(void) const;
	vec3& normalize(void);

	float* ptr(void);
	const float* ptr(void) const;

public:
    // Used when scalar * vec3
    // Need friend to access private fields of vec3
	friend vec3		operator*(float scalar, const vec3& v);
};

