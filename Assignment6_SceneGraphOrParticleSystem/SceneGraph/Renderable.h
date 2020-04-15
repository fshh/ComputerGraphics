#pragma once

#include <QtCore>
#include <QtGui>
#include <QtOpenGL>
#include "Structs.h"

enum class DrawMode {
	DEFAULT = 0,
	WIREFRAME = 1,
	TEX_DEBUG = 2,
	NORM_DEBUG = 3,
	LIGHTING_DEBUG = 4
};

class Renderable: protected QOpenGLFunctions
{
protected:
	// For now, we have only one shader per object
	QOpenGLShaderProgram shader_;
	// For now, we have a single unified buffer per object
	QOpenGLBuffer vbo_;
	// Make sure we have an index buffer.
	QOpenGLBuffer ibo_;
	// We have a single draw call, so a single vao
	QOpenGLVertexArrayObject vao_;
	// Keep track of how many triangles we actually have to draw in our ibo
	unsigned int numTris_;
	int vertexSize_;

	// Create our shader and fix it up
	void createShaders();

public:
	Renderable();
	virtual ~Renderable();

	virtual void init(const QVector<Vertex>& vertices, const QVector<Face>& faces);
	virtual void draw(const QMatrix4x4& worldSpaceModelMatrix, const QVector3D& viewPosition, const QMatrix4x4& viewMatrix, const QMatrix4x4& projectionMatrix, 
		const DrawMode drawMode, QOpenGLTexture* diffuseMap, QOpenGLTexture* normalMap, QVector<PointLight>* lights);

private:

};
