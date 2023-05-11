#pragma once

class vec3 {
public:
	double x, y, z;

public:
	vec3();
	vec3(double x, double y, double z);

	vec3& set(double x, double y, double z);

	vec3& zero(void);

	vec3			operator- (void) const;
	vec3			operator+ (void) const;

	vec3			operator+ (const vec3& v) const;
	vec3			operator- (const vec3& v) const;
	vec3			operator* (double scalar) const;
	vec3			operator/ (double scalar) const;

	vec3& operator= (const vec3& v);
	vec3& operator+=(const vec3& v);
	vec3& operator-=(const vec3& v);
	vec3& operator*=(double scalar);
	vec3& operator/=(double scalar);

	double& operator[](int index);
	const double& operator[](int index) const;

	double			dot(const vec3& v) const;
	vec3			cross(const vec3& v) const;

	double			magnitude(void) const;
	vec3& normalize(void);

	double* ptr(void);
	const double* ptr(void) const;

public:
    // Used when scalar * vec3
    // Need friend to access private fields of vec3
	friend vec3		operator*(double scalar, const vec3& v);
};

