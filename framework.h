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
inline float cross(const vec2& a, const vec2& b) { return a.x * b.y - a.y * b.x; };
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
inline vec3 minVec3(const vec3& v1, const vec3& v2) { return (vec3(min(v1.x, v2.x), min(v1.y, v2.y), min(v1.z, v2.z))); }
inline vec3 maxVec3(const vec3& v1, const vec3& v2) { return (vec3(max(v1.x, v2.x), max(v1.y, v2.y), max(v1.z, v2.z))); }

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

vec3 lerp(const vec3& start, const vec3& end, float t) {
	return start + (end - start) * t;
}

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

inline vec4 HexRGB(uint32_t rgb) {
	float r = float((rgb >> 16) & 0xFF) / 255.0f;
	float g = float((rgb >> 8) & 0xFF) / 255.0f;
	float b = float((rgb >> 0) & 0xFF) / 255.0f;
	return vec4(r, g, b, 1.0f);
}

inline vec4 HexRGBA(uint32_t rgba) {
	float r = float((rgba >> 24) & 0xFF) / 255.0f;
	float g = float((rgba >> 16) & 0xFF) / 255.0f;
	float b = float((rgba >> 8) & 0xFF) / 255.0f;
	float a = float((rgba >> 0) & 0xFF) / 255.0f;
	return vec4(r, g, b, a);
}

struct mat4 { // column-major storage: cols[0..3]
	vec4 cols[4];
	mat4() { cols[0] = vec4(1, 0, 0, 0); cols[1] = vec4(0, 1, 0, 0); cols[2] = vec4(0, 0, 1, 0); cols[3] = vec4(0, 0, 0, 1); }
	mat4(const vec4& c0, const vec4& c1, const vec4& c2, const vec4& c3) { cols[0] = c0; cols[1] = c1; cols[2] = c2; cols[3] = c3; }

	vec4& operator[](int i) { return cols[i]; }          // column access
	vec4  operator[](int i) const { return cols[i]; }
	operator const float* () const { return (const float*)this; } // contiguous, column-major
};

// mat * vec (column vector)
inline vec4 operator*(const mat4& M, const vec4& v) {
	return v.x * M[0] + v.y * M[1] + v.z * M[2] + v.w * M[3];
}

// mat * mat
inline mat4 operator*(const mat4& A, const mat4& B) {
	// Columns of result are A * B's columns
	return mat4(A * B[0], A * B[1], A * B[2], A * B[3]);
}

// Builders
inline mat4 TranslateMatrix(vec3 t) {
	return mat4(
		vec4(1, 0, 0, 0),
		vec4(0, 1, 0, 0),
		vec4(0, 0, 1, 0),
		vec4(t.x, t.y, t.z, 1)
	);
}

inline mat4 ScaleMatrix(vec3 s) {
	return mat4(
		vec4(s.x, 0, 0, 0),
		vec4(0, s.y, 0, 0),
		vec4(0, 0, s.z, 0),
		vec4(0, 0, 0, 1)
	);
}

inline mat4 RotationMatrix(float angle, vec3 w) {
	float c = cosf(angle), s = sinf(angle);
	w = normalize(w);
	float x = w.x, y = w.y, z = w.z;
	// Columns (not rows): standard Rodrigues form for column-major
	return mat4(
		vec4(c + (1 - c) * x * x, (1 - c) * x * y + s * z, (1 - c) * x * z - s * y, 0),
		vec4((1 - c) * y * x - s * z, c + (1 - c) * y * y, (1 - c) * y * z + s * x, 0),
		vec4((1 - c) * z * x + s * y, (1 - c) * z * y - s * x, c + (1 - c) * z * z, 0),
		vec4(0, 0, 0, 1)
	);
}

inline mat4 TransposeMatrix(const mat4& m) {
	return mat4(
		vec4(m[0].x, m[1].x, m[2].x, m[3].x),
		vec4(m[0].y, m[1].y, m[2].y, m[3].y),
		vec4(m[0].z, m[1].z, m[2].z, m[3].z),
		vec4(m[0].w, m[1].w, m[2].w, m[3].w)
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

	vec3 right() {
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

	mat4 toRotationMatrix() const {
		float xx = x * x, yy = y * y, zz = z * z;
		float xy = x * y, xz = x * z, yz = y * z;
		float wx = w * x, wy = w * y, wz = w * z;

		return mat4(
			vec4(1 - 2 * (yy + zz), 2 * (xy - wz), 2 * (xz + wy), 0), // col 0: right
			vec4(2 * (xy + wz), 1 - 2 * (xx + zz), 2 * (yz - wx), 0), // col 1: up
			vec4(2 * (xz - wy), 2 * (yz + wx), 1 - 2 * (xx + yy), 0), // col 2: fwd
			vec4(0, 0, 0, 1)
		);
	}

	static Quaternion fromRotationMatrix(const mat4& m) {
		// read rotation from columns m[0..2]
		float m00 = m[0].x, m01 = m[1].x, m02 = m[2].x;
		float m10 = m[0].y, m11 = m[1].y, m12 = m[2].y;
		float m20 = m[0].z, m21 = m[1].z, m22 = m[2].z;

		Quaternion q;
		float trace = m00 + m11 + m22;
		if (trace > 0.0f) {
			float s = sqrtf(trace + 1.0f) * 2.0f;
			q.w = 0.25f * s;
			q.x = (m21 - m12) / s;
			q.y = (m02 - m20) / s;
			q.z = (m10 - m01) / s;
		}
		else if (m00 > m11 && m00 > m22) {
			float s = sqrtf(1.0f + m00 - m11 - m22) * 2.0f;
			q.w = (m21 - m12) / s;
			q.x = 0.25f * s;
			q.y = (m01 + m10) / s;
			q.z = (m02 + m20) / s;
		}
		else if (m11 > m22) {
			float s = sqrtf(1.0f + m11 - m00 - m22) * 2.0f;
			q.w = (m02 - m20) / s;
			q.x = (m01 + m10) / s;
			q.y = 0.25f * s;
			q.z = (m12 + m21) / s;
		}
		else {
			float s = sqrtf(1.0f + m22 - m00 - m11) * 2.0f;
			q.w = (m10 - m01) / s;
			q.x = (m02 + m20) / s;
			q.y = (m12 + m21) / s;
			q.z = 0.25f * s;
		}
		q.normalize();
		return q;
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

struct AABB { vec3 min, max; };

inline bool pointInAABB(const vec3& p, const AABB& b) {
	return (p.x >= b.min.x && p.x <= b.max.x) &&
		(p.y >= b.min.y && p.y <= b.max.y) &&
		(p.z >= b.min.z && p.z <= b.max.z);
}

