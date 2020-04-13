#include "Structs.h"
#include <cmath>

// ~~~~~~~~~~ VEC3 ~~~~~~~~~~
Vec3::Vec3() : x(0), y(0), z(0) {}

Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

Vec3::Vec3(const QVector3D vec) : x(vec.x()), y(vec.y()), z(vec.z()) {}

float Vec3::length() const {
	return std::sqrt(x * x + y * y + z * z);
}

void Vec3::normalize() {
	float len = length();
	x /= len;
	y /= len;
	z /= len;
}

Vec3 Vec3::normalized() const {
	float len = length();
	return Vec3(x / len, y / len, z / len);
}

bool Vec3::operator==(Vec3 other) const {
	return x == other.x && y == other.y && z == other.z;
}

Vec3& Vec3::operator+=(const Vec3& other) {
	x += other.x;
	y += other.y;
	z += other.z;
	return (*this);
}

Vec3 Vec3::operator-(const Vec3& other) const {
	return Vec3(x - other.x, y - other.y, z - other.z);
}

Vec3 Vec3::operator-() const {
	return Vec3(-x, -y, -z);
}

float& Vec3::operator[](int i) {
	return ((&x)[i]);
}

const float& Vec3::operator[](int i) const {
	return ((&x)[i]);
}


// ~~~~~~~~~~ VEC2 ~~~~~~~~~~
Vec2::Vec2() : u(0), v(0) {}

Vec2::Vec2(float u, float v) : u(u), v(v) {}

Vec2::Vec2(const QVector2D vec) : u(vec.x()), v(vec.y()) {}

float Vec2::length() const {
	return std::sqrt(u * u + v * v);
}

void Vec2::normalize() {
	float len = length();
	u /= len;
	v /= len;
}

Vec2 Vec2::normalized() const {
	float len = length();
	return Vec2(u / len, v / len);
}

bool Vec2::operator==(Vec2 other) const {
	return u == other.u && v == other.v;
}

Vec2 Vec2::operator-(const Vec2& other) const {
	return Vec2(u - other.u, v - other.v);
}

float& Vec2::operator[](int i) {
	return ((&u)[i]);
}

const float& Vec2::operator[](int i) const {
	return ((&u)[i]);
}


// ~~~~~~~~~~ VERTEX ~~~~~~~~~~
Vertex::Vertex() : position(), texCoord(), normal(), tangent() {}

Vertex::Vertex(Vec3 pos, Vec2 tex, Vec3 norm, Vec3 tan) : position(pos), texCoord(tex), normal(norm), tangent(tan) {}

bool Vertex::operator==(Vertex other) const {
	return
		position == other.position &&
		texCoord == other.texCoord &&
		normal == other.normal &&
		tangent == other.tangent;
}


// ~~~~~~~~~~ FACE ~~~~~~~~~~
Face::Face() : a(0), b(0), c(0) {}

Face::Face(unsigned int v1, unsigned int v2, unsigned int v3) : a(v1), b(v2), c(v3) {}

bool Face::operator==(Face other) const {
	return
		a == other.a &&
		b == other.b &&
		c == other.c;
}

unsigned int& Face::operator[](int i) {
	return ((&a)[i]);
}

const unsigned int& Face::operator[](int i) const {
	return ((&a)[i]);
}


// ~~~~~~~~~~ POINTLIGHT ~~~~~~~~~~
PointLight::PointLight() : position(), color(), ambientIntensity(0), specularIntensity(0), constant(0), linear(0), quadratic(0) {}
PointLight::PointLight(QVector3D position, QVector3D color, float ambientIntensity, float specularIntensity, float constant, float linear, float quadratic) :
	position(position), color(color), ambientIntensity(ambientIntensity), specularIntensity(specularIntensity), constant(constant), linear(linear), quadratic(quadratic) {}
