#pragma once
#define _USE_MATH_DEFINES

#include <array>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <vector>
#include <string>
#include <windows.h>
#include <chrono>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"


int fps;
int frameCount = 0;
float previousTime = glfwGetTime();

static void getFPS(int& fps) {
	float currentTime = glfwGetTime();
	frameCount++;
	if (currentTime - previousTime >= 1.0) {
		fps = frameCount;
		frameCount = 0;
		previousTime = currentTime;
	}
}

void clamp(float& x, float min_value, float max_value) {
	x = min(max_value, max(x, min_value));
}

float radians(float degrees) {
	return degrees * M_PI / 180.0;
}

float random(float min, float max) {
	static std::random_device rd;
	static std::mt19937 rng(rd());
	static std::uniform_real_distribution<float> dist(min, max);
	return dist(rng);
}

template <typename T>
T mix(const T& start, const T& end, float alpha) {
	return start + alpha * (end - start);
}

static float getTime() {
	auto now = std::chrono::high_resolution_clock::now();
	auto duration = now.time_since_epoch();
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count();
	return static_cast<float>(milliseconds);
}

struct vec2 {
	float x, y;

	vec2(float x0 = 0, float y0 = 0) { x = x0; y = y0; }

	vec2 operator*(float a) const { return vec2(x * a, y * a); }
	vec2 operator/(float a) const { return vec2(x / a, y / a); }
	vec2 operator+(const vec2& v) const { return vec2(x + v.x, y + v.y); }
	vec2 operator-(const vec2& v) const { return vec2(x - v.x, y - v.y); }
	vec2 operator*(const vec2& v) const { return vec2(x * v.x, y * v.y); }
	vec2 operator-() const { return vec2(-x, -y); }
	void operator+=(const vec2 right) { x += right.x; y += right.y; }
	void operator-=(const vec2 right) { x -= right.x; y -= right.y; }
};

inline float dot(const vec2& v1, const vec2& v2) { return (v1.x * v2.x + v1.y * v2.y); }
inline float length(const vec2& v) { return sqrtf(dot(v, v)); }
inline vec2 normalize(const vec2& v) { return v * (1 / length(v)); }
inline vec2 abs(const vec2& v) { return vec2(abs(v.x), abs(v.y)); }
inline vec2 max2(const vec2& a, const vec2& b) { return vec2(max(a.x, b.x), max(a.y, b.y)); }
inline vec2 operator*(float a, const vec2& v) { return vec2(v.x * a, v.y * a); }

struct vec3 {
	float x, y, z;

	vec3(float x0 = 0, float y0 = 0, float z0 = 0) { x = x0; y = y0; z = z0; }
	vec3(vec2 v) { x = v.x; y = v.y; z = 0; }

	vec3 operator*(float a) const { return vec3(x * a, y * a, z * a); }
	vec3 operator/(float a) const { return vec3(x / a, y / a, z / a); }
	vec3 operator+(const vec3& v) const { return vec3(x + v.x, y + v.y, z + v.z); }
	vec3 operator-(const vec3& v) const { return vec3(x - v.x, y - v.y, z - v.z); }
	vec3 operator*(const vec3& v) const { return vec3(x * v.x, y * v.y, z * v.z); }
	vec3 operator-()  const { return vec3(-x, -y, -z); }
	void operator+=(const vec3 right) { x += right.x; y += right.y; z += right.z; }
	void operator-=(const vec3 right) { x -= right.x; y -= right.y; z -= right.z; }
	void operator*=(float a) { x *= a; y *= a; z *= a; }
	void operator/=(float a) { x /= a; y /= a; z /= a; }
	bool operator==(const vec3& right) { return x == right.x && y == right.y && z == right.z; }
	
};

inline float dot(const vec3& v1, const vec3& v2) { return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z); }
inline float length(const vec3& v) { return sqrtf(dot(v, v)); }
inline vec3 abs(const vec3& v) { return vec3(abs(v.x), abs(v.y), abs(v.z)); }
inline vec3 max3(const vec3& a, const vec3& b) { return vec3(max(a.x, b.x), max(a.y, b.y), max(a.z, b.z)); }
inline vec3 normalize(const vec3& v) { return v * (1 / length(v)); }
inline vec3 cross(const vec3& v1, const vec3& v2) {	return vec3(v1.y * v2.z - v1.z * v2.y, v1.z * v2.x - v1.x * v2.z, v1.x * v2.y - v1.y * v2.x); }
inline vec3 operator*(float a, const vec3& v) { return vec3(v.x * a, v.y * a, v.z * a); }

struct Vec3Hash {
	size_t operator()(const vec3& v) const {
		return std::hash<float>()(v.x) ^ std::hash<float>()(v.y) ^ std::hash<float>()(v.z);
	}
};

struct Vec3Equal {
	bool operator()(const vec3& lhs, const vec3& rhs) const {
		return lhs.x == rhs.x && lhs.y == rhs.y && lhs.z == rhs.z;
	}
};

struct vec4 {
	float x, y, z, w;

	vec4(float x0 = 0, float y0 = 0, float z0 = 0, float w0 = 0) { x = x0; y = y0; z = z0; w = w0; }

	float& operator[](int j) { return *(&x + j); }
	float operator[](int j) const { return *(&x + j); }
	vec4 operator*(float a) const { return vec4(x * a, y * a, z * a, w * a); }
	vec4 operator/(float d) const { return vec4(x / d, y / d, z / d, w / d); }
	vec4 operator+(const vec4& v) const { return vec4(x + v.x, y + v.y, z + v.z, w + v.w); }
	vec4 operator-(const vec4& v)  const { return vec4(x - v.x, y - v.y, z - v.z, w - v.w); }
	vec4 operator*(const vec4& v) const { return vec4(x * v.x, y * v.y, z * v.z, w * v.w); }
	void operator+=(const vec4 right) { x += right.x; y += right.y; z += right.z; w += right.w; }
	void operator-=(const vec4 right) { x -= right.x; y -= right.y; z -= right.z; w -= right.w; }
};

inline float dot(const vec4& v1, const vec4& v2) {
	return (v1.x * v2.x + v1.y * v2.y + v1.z * v2.z + v1.w * v2.w);
}

inline vec4 operator*(float a, const vec4& v) {
	return vec4(v.x * a, v.y * a, v.z * a, v.w * a);
}

struct mat4 { // row-major matrix 4x4
	vec4 rows[4];
public:
	mat4() {}
	mat4(float m00, float m01, float m02, float m03,
		float m10, float m11, float m12, float m13,
		float m20, float m21, float m22, float m23,
		float m30, float m31, float m32, float m33) {
		rows[0][0] = m00; rows[0][1] = m01; rows[0][2] = m02; rows[0][3] = m03;
		rows[1][0] = m10; rows[1][1] = m11; rows[1][2] = m12; rows[1][3] = m13;
		rows[2][0] = m20; rows[2][1] = m21; rows[2][2] = m22; rows[2][3] = m23;
		rows[3][0] = m30; rows[3][1] = m31; rows[3][2] = m32; rows[3][3] = m33;
	}
	mat4(vec4 it, vec4 jt, vec4 kt, vec4 ot) {
		rows[0] = it; rows[1] = jt; rows[2] = kt; rows[3] = ot;
	}

	vec4& operator[](int i) { return rows[i]; }
	vec4 operator[](int i) const { return rows[i]; }
	operator float*() const { return (float*)this; }
};

inline vec4 operator*(const vec4& v, const mat4& mat) {
	return v[0] * mat[0] + v[1] * mat[1] + v[2] * mat[2] + v[3] * mat[3];
}

inline mat4 operator*(const mat4& left, const mat4& right) {
	mat4 result;
	for (int i = 0; i < 4; i++) result.rows[i] = left.rows[i] * right;
	return result;
}

inline mat4 TranslateMatrix(vec3 t) {
	return mat4(vec4(1,   0,   0,   0),
			    vec4(0,   1,   0,   0),
				vec4(0,   0,   1,   0),
				vec4(t.x, t.y, t.z, 1));
}

inline mat4 ScaleMatrix(vec3 s) {
	return mat4(vec4(s.x, 0,   0,   0),
			    vec4(0,   s.y, 0,   0),
				vec4(0,   0,   s.z, 0),
				vec4(0,   0,   0,   1));
}

inline mat4 RotationMatrix(float angle, vec3 w) {
	float c = cosf(angle), s = sinf(angle);
	w = normalize(w);
	return mat4(vec4(c * (1 - w.x*w.x) + w.x*w.x, w.x*w.y*(1 - c) + w.z*s, w.x*w.z*(1 - c) - w.y*s, 0),
			    vec4(w.x*w.y*(1 - c) - w.z*s, c * (1 - w.y*w.y) + w.y*w.y, w.y*w.z*(1 - c) + w.x*s, 0),
			    vec4(w.x*w.z*(1 - c) + w.y*s, w.y*w.z*(1 - c) - w.x*s, c * (1 - w.z*w.z) + w.z*w.z, 0),
			    vec4(0, 0, 0, 1));
}

inline mat4 TransposeMatrix(const mat4& m) {
	return mat4(
		vec4(m[0][0], m[1][0], m[2][0], m[3][0]),
		vec4(m[0][1], m[1][1], m[2][1], m[3][1]),
		vec4(m[0][2], m[1][2], m[2][2], m[3][2]),
		vec4(m[0][3], m[1][3], m[2][3], m[3][3])
	);
}

struct Quaternion {
	float w, x, y, z;

	Quaternion(float w0 = 1.0, float x0 = 0.0, float y0 = 0.0, float z0 = 0.0) : w(w0), x(x0), y(y0), z(z0) {}

	// Forward, Up, Right: https://www.gamedev.net/forums/topic/56471-extracting-direction-vectors-from-quaternion/

	vec3 forward() {
		return vec3(
			2 * (x * z - w * y),
			2 * (y * z + w * x),
			1 - 2 * (x * x + y * y)
		);
	}
	
	vec3 up() {
		return vec3(
			2 * (x * y - w * z),
			1 - 2 * (x * x + z * z),
			2 * (y * z + w * x)
		);
	}

	vec3 left() {
		return vec3(
			1 - 2 * (y * y + z * z),
			2 * (x * y - w * z),
			2 * (x * z + w * y)
		);
	}

	void normalize() {
		float norm = sqrt(w * w + x * x + y * y + z * z);
		w /= norm;
		x /= norm;
		y /= norm;
		z /= norm;
	}

	// TESTING
	static Quaternion fromRotationMatrix(const mat4& m) {
		Quaternion q;

		// Extract the rotation components from the matrix
		float m00 = m[0][0], m01 = m[0][1], m02 = m[0][2];
		float m10 = m[1][0], m11 = m[1][1], m12 = m[1][2];
		float m20 = m[2][0], m21 = m[2][1], m22 = m[2][2];

		float trace = m00 + m11 + m22;

		if (trace > 0.0f) {
			printf("A");
			float s = sqrtf(trace + 1.0f) * 2.0f; // s = 4 * q.w
			q.w = 0.25f * s;
			q.x = -(m12 - m21) / s;
			q.y = -(m20 - m02) / s;
			q.z = -(m01 - m10) / s;
		}
		else if ((m00 > m11) && (m00 > m22)) {
			printf("B");
			float s = sqrtf(1.0f + m00 - m11 - m22) * 2.0f; // s = 4 * q.x
			q.w = (m12 - m21) / s;
			q.x = -0.25f * s;
			q.y = -(m01 + m10) / s;
			q.z = -(m02 + m20) / s;
		}
		else if (m11 > m22) {
			printf("C");
			float s = sqrtf(1.0f + m11 - m00 - m22) * 2.0f; // s = 4 * q.y
			q.w = (m20 - m02) / s;
			q.x = -(m01 + m10) / s;
			q.y = -0.25f * s;
			q.z = -(m12 + m21) / s;
		}
		else {
			printf("D");
			float s = sqrtf(1.0f + m22 - m00 - m11) * 2.0f; // s = 4 * q.z
			q.w = (m01 - m10) / s;
			q.x = (m02 + m20) / s;
			q.y = (m12 + m21) / s;
			q.z = 0.25f * s;
		}

		q.normalize();
		return q;
	}




	mat4 toRotationMatrix() const {
		float wx = w * x, wy = w * y, wz = w * z;
		float xx = x * x, xy = x * y, xz = x * z;
		float yy = y * y, yz = y * z, zz = z * z;

		return mat4(
			vec4(1.0 - 2.0 * (yy + zz), 2.0 * (xy - wz), 2.0 * (xz + wy), 0.0),
			vec4(2.0 * (xy + wz), 1.0 - 2.0 * (xx + zz), 2.0 * (yz - wx), 0.0),
			vec4(2.0 * (xz - wy), 2.0 * (yz + wx), 1.0 - 2.0 * (xx + yy), 0.0),
			vec4(0.0, 0.0, 0.0, 1.0)
		);
	}

	static Quaternion fromAxisAngle(const vec3& axis, float angle) {
		float halfAngle = angle * 0.5;
		return Quaternion(cos(halfAngle), axis.x * sin(halfAngle), axis.y * sin(halfAngle), axis.z * sin(halfAngle));
	}


	Quaternion operator*(const Quaternion& q) const {
		return Quaternion(
			w * q.w - x * q.x - y * q.y - z * q.z,
			w * q.x + x * q.w + y * q.z - z * q.y,
			w * q.y - x * q.z + y * q.w + z * q.x,
			w * q.z + x * q.y - y * q.x + z * q.w
		);
	}
};

