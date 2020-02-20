#ifndef MODEL_STRUCT_H
#define MODEL_STRUCT_H

#include <QtOpenGL>
#include <vector>

struct Model {
public:
	Model() = delete;
	Model(QVector<GLfloat>& verts, QVector<GLfloat>& norms, QVector<QVector<GLuint>>& faces, QOpenGLShaderProgram* shader) :
		vbo_(QOpenGLBuffer::VertexBuffer), ibo_(QOpenGLBuffer::IndexBuffer), shaderProgram_{ shader } {}
	~Model();

	size_t vertsToDraw;
	QOpenGLVertexArrayObject vao_;

private:
	QOpenGLBuffer vbo_;
	QOpenGLBuffer ibo_;
	QOpenGLShaderProgram* shaderProgram_;
};

#endif