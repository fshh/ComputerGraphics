// Sphere source code based on http://www.songho.ca/opengl/gl_sphere.html#sphere
// I was having some issues with the provided implementation so I used this one instead

#include "Sphere.h"

// based on https://www.gamasutra.com/view/feature/129939/messing_with_tangent_space.php
// and https://learnopengl.com/Advanced-Lighting/Normal-Mapping
QVector<Vec3> getFaceTangents(const QVector<Face>& faces, const QVector<Vertex>& vertices) {
	QVector<Vec3> tangents(faces.size());
	for (int ii = 0; ii < faces.size(); ++ii) {
		Face face = faces[ii];

		Vertex v1 = vertices[face.a];
		Vertex v2 = vertices[face.b];
		Vertex v3 = vertices[face.c];

		Vec3 edge1 = v2.position - v1.position;
		Vec3 edge2 = v3.position - v1.position;

		Vec2 deltaUV1 = v2.texCoord - v1.texCoord;
		Vec2 deltaUV2 = v3.texCoord - v1.texCoord;

		float f = 1.0f / (deltaUV1.u * deltaUV2.v - deltaUV2.u * deltaUV1.v);

		Vec3 tangent;
		tangent.x = f * (deltaUV2.v * edge1.x - deltaUV1.v * edge2.x);
		tangent.y = f * (deltaUV2.v * edge1.y - deltaUV1.v * edge2.y);
		tangent.z = f * (deltaUV2.v * edge1.z - deltaUV1.v * edge2.z);
		tangent.normalize();

		tangents[ii] = tangent;
	}

	return tangents;
}

// based on https://www.gamasutra.com/view/feature/129939/messing_with_tangent_space.php
// and https://learnopengl.com/Advanced-Lighting/Normal-Mapping
void setVertexTangents(const QVector<Vec3>& faceTangents, const QVector<Face>& faces, QVector<Vertex>& vertices) {
	// Calculate cumulative tangents for each vertex
	// Don't need to average because we normalize it anyway
	for (int ii = 0; ii < faces.size(); ++ii) {
		Face face = faces[ii];
		for (int jj = 0; jj < 3; ++jj) {
			vertices[face[jj]].tangent += faceTangents[ii];
		}
	}

	// Normalize each vertex's tangent
	for (Vertex& vert : vertices) {
		vert.tangent.normalize();
	}
}


// Calls the initalization routine
Sphere::Sphere() : radius_(1.0), sectorCount_(30), stackCount_(30) {
	init();
}

///////////////////////////////////////////////////////////////////////////////
// build vertices of sphere with smooth shading using parametric equation
// x = r * cos(u) * cos(v)
// y = r * sin(u)
// z = r * cos(u) * sin(v)
// where u: stack(latitude) angle (-90 <= u <= 90)
//       v: sector(longitude) angle (0 <= v <= 360)
///////////////////////////////////////////////////////////////////////////////
void Sphere::init() {
	double x, y, z, xz;															// vertex position
	double nx, ny, nz, lengthInv = 1.0 / radius_;		// normal
	double u, v;																			// texCoord

	double sectorStep = 2 * M_PI / double(sectorCount_);
	double stackStep = M_PI / double(stackCount_);
	double sectorAngle, stackAngle;

	for (int i = 0; i <= stackCount_; ++i)
	{
		stackAngle = M_PI_2 - i * stackStep;					// starting from pi/2 to -pi/2
		xz = radius_ * cos(stackAngle);								// r * cos(u)
		y = radius_ * sin(stackAngle);								// r * sin(u)

		// add (sectorCount+1) vertices per stack
		// the first and last vertices have same position and normal, but different tex coords
		for (int j = 0; j <= sectorCount_; ++j)
		{
			sectorAngle = j * sectorStep;								// starting from 0 to 2pi

			// vertex position
			x = xz * cos(sectorAngle);									// r * cos(u) * cos(v)
			z = xz * sin(sectorAngle);									// r * cos(u) * sin(v)
			Vec3 pos(x, y, z);

			// normalized vertex normal
			nx = x * lengthInv;
			ny = y * lengthInv;
			nz = z * lengthInv;
			Vec3 norm(nx, ny, nz);

			// vertex tex coord between [0, 1]
			u = -(double(j) / double(sectorCount_)) + 1;
			v = double(i) / double(stackCount_);
			Vec2 tex(u, v);

			Vertex vert;
			vert.position = pos;
			vert.texCoord = tex;
			vert.normal = norm;
			vertices_ << vert;
		}
	}

	// indices
	//  k1--k1+1
	//  |  / |
	//  | /  |
	//  k2--k2+1
	unsigned int k1, k2;
	for (int i = 0; i < stackCount_; ++i)
	{
		k1 = i * (sectorCount_ + 1);									// beginning of current stack
		k2 = k1 + stackCount_ + 1;										// beginning of next stack

		for (int j = 0; j < sectorCount_; ++j, ++k1, ++k2)
		{
			// 2 triangles per sector excluding 1st and last stacks
			if (i != 0)
			{
				Face face(k1, k2, k1 + 1);								// k1---k2---k1+1
				faces_ << face;
			}

			if (i != (stackCount_ - 1))
			{
				Face face(k1 + 1, k2, k2 + 1);						// k1+1---k2---k2+1
				faces_ << face;
			}
		}
	}

	// calculate tangents (per face) after reading all file data
	QVector<Vec3> faceTangents = getFaceTangents(faces_, vertices_);
	// set vertex tangents based on all faces a vertex is part of
	setVertexTangents(faceTangents, faces_, vertices_);
}


