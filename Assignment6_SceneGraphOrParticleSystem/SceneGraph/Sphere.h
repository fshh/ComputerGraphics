#pragma once

// Sphere source code based on http://www.songho.ca/opengl/gl_sphere.html#sphere
// I was having some issues with the provided implementation so I used this one instead

#define _USE_MATH_DEFINES

#include <cmath>
#include <QtGui>
#include "Structs.h"

class Sphere {
public:
	// Constructor for the Sphere
	Sphere();
	// The intialization routine for this object.
	void init();

	// Getters for our data.
	inline QVector<Vertex> vertices() const {return vertices_;}
	inline QVector<Face> faces() const { return faces_; }

private:
	double radius_;
	int sectorCount_;
	int stackCount_;
	QVector<Vertex> vertices_;
	QVector<Face> faces_;
};


