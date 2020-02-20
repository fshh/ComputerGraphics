#ifndef MODEL_STRUCT_H
#define MODEL_STRUCT_H

#include <QtOpenGL>
#include <vector>

struct Model {
public:
	Model() = delete;
	Model(QVector<GLfloat>& verts, QVector<GLfloat>& norms, QVector<QVector<GLuint>>& faces);
	Model(const Model& other);
	Model& operator=(const Model& other);
	~Model();

	size_t vertsToDraw;
	QOpenGLVertexArrayObject vao_;
	QOpenGLShaderProgram shaderProgram_;

private:
	QString vertexShaderString() const;
	QString fragmentShaderString() const;
	void createShader();
	
	Model(size_t _vertsToDraw, QOpenGLBuffer _vbo_, QOpenGLBuffer _ibo_);
	QOpenGLBuffer vbo_;
	QOpenGLBuffer ibo_;
	
	void bindVAO();
};

#endif
