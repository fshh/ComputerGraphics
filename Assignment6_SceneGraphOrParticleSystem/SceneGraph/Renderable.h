#pragma once

#include <QtCore>
#include <QtGui>
#include <QtOpenGL>
#include "Structs.h"

enum class DrawMode {
	DEFAULT = 0,
	WIREFRAME = 1,
	TEX_DEBUG = 2,
	NORM_DEBUG = 3
};

class Renderable: protected QOpenGLFunctions
{
protected:
	// Each renderable has its own model matrix
	QMatrix4x4 modelMatrix_;
	// For now, we have only one shader per object
	QOpenGLShaderProgram shader_;
	// Diffuse map for the object
	QOpenGLTexture diffuseMap_;
	// Normal map for the object
	QOpenGLTexture normalMap_;
	// For now, we have a single unified buffer per object
	QOpenGLBuffer vbo_;
	// Make sure we have an index buffer.
	QOpenGLBuffer ibo_;
	// We have a single draw call, so a single vao
	QOpenGLVertexArrayObject vao_;
	// Keep track of our lights
	QVector<PointLight> lights_;
	// Keep track of how many triangles we actually have to draw in our ibo
	unsigned int numTris_;
	int vertexSize_;

	// Define our axis of rotation for animation
	QVector3D rotationAxis_;
	float rotationSpeed_;
	float rotationAngle_;

	// Create our shader and fix it up
	void createShaders();

public:
	Renderable();
	virtual ~Renderable();

	virtual void init(const QVector<Vertex>& vertices, const QVector<Face>& faces, const QString& diffuseMap, const QString& normalMap);
	virtual void update(const qint64 msSinceLastFrame);
	virtual void draw(const QMatrix4x4& world, const QMatrix4x4& view, const QMatrix4x4& projection, const DrawMode drawMode);

	void setModelMatrix(const QMatrix4x4& transform);
	void setRotationAxis(const QVector3D& axis);
	void setRotationSpeed(float speed);

private:

};
