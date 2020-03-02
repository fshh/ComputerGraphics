#pragma once

#include <QtCore>
#include <QtGui>
#include <QtOpenGL>

enum class DrawMode {
	DEFAULT = 0,
	WIREFRAME = 1,
	TEX_DEBUG = 2,
	NORM_DEBUG = 3
};

class Renderable
{
protected:
	// Each renderable has its own model matrix
	QMatrix4x4 modelMatrix_;
	// For now, we have only one shader per object
	QOpenGLShaderProgram shader_;
	// For now, we have only one texture per object
	QOpenGLTexture texture_;
	// For now, we have a single unified buffer per object
	QOpenGLBuffer vbo_;
	// Make sure we have an index buffer.
	QOpenGLBuffer ibo_;
	// We have a single draw call, so a single vao
	QOpenGLVertexArrayObject vao_;
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

	virtual void init(const QVector<QVector3D>& positions, const QVector<QVector3D>& normals, const QVector<QVector2D>& texCoords, const QVector<QVector<unsigned int>>& faces, const QString& textureFile);
	virtual void update(const qint64 msSinceLastFrame);
	virtual void draw(const QMatrix4x4& view, const QMatrix4x4& projection, const DrawMode drawMode);

	void setModelMatrix(const QMatrix4x4& transform);
	void setRotationAxis(const QVector3D& axis);
	void setRotationSpeed(float speed);

private:

};
