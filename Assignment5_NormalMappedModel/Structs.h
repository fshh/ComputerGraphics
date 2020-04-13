#pragma once

#include <QtOpenGL>

struct Vec3 {
	float x, y, z;

	Vec3();
	Vec3(float x, float y, float z);
	Vec3(const QVector3D vec);

	float length() const;
	void normalize();
	Vec3 normalized() const;

	bool operator==(Vec3 other) const;
	Vec3& operator+=(const Vec3& other);
	Vec3 operator-(const Vec3& other) const;
	Vec3 operator-() const;
	float& operator[](int i);
	const float& operator[](int i) const;
};

struct Vec2 {
	float u, v;

	Vec2();
	Vec2(float u, float v);
	Vec2(const QVector2D vec);

	float length() const;
	void normalize();
	Vec2 normalized() const;

	bool operator==(Vec2 other) const;
	Vec2 operator-(const Vec2& other) const;
	float& operator[](int i);
	const float& operator[](int i) const;
};

struct Vertex {
	Vec3 position;
	Vec2 texCoord;
	Vec3 normal;
	Vec3 tangent;

	Vertex();
	Vertex(Vec3 pos, Vec2 tex, Vec3 norm, Vec3 tan);

	bool operator==(Vertex other) const;
};

struct Face {
	unsigned int a, b, c;

	Face();
	Face(unsigned int v1, unsigned int v2, unsigned int v3);

	bool operator==(Face other) const;
	unsigned int& operator[](int i);
	const unsigned int& operator[](int i) const;
};

struct PointLight {
	QVector3D position;
	QVector3D color;
	float ambientIntensity;
	float specularIntensity;
	float constant;
	float linear;
	float quadratic;

	PointLight();
	PointLight(QVector3D position, QVector3D color, float ambientIntensity = 0.5f, float specularIntensity = 0.5f, float constant = 1.0f, float linear = 0.09f, float quadratic = 0.032f);
};
