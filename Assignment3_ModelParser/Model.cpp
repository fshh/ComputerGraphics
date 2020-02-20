#include "Model.h"

Model::Model(QVector<GLfloat>& verts, QVector<GLfloat>& norms, QVector<QVector<GLuint>>& faces, QOpenGLShaderProgram* shader) :
	vbo_(QOpenGLBuffer::VertexBuffer), ibo_(QOpenGLBuffer::IndexBuffer), shaderProgram_{ shader } {
	QVector<GLfloat> vbuffer;
	QVector<GLuint> ibuffer;

	GLuint index = 0;
	for (QVector<GLuint>& face : faces) {
		bool pos = true;
		for (GLuint& idx : face) {
			if (pos) {
				vbuffer[index] = verts[idx * 3];
				vbuffer[index + 1] = verts[idx * 3 + 1];
				vbuffer[index + 2] = verts[idx * 3 + 2];
				ibuffer.append(index);
				index += 3;
			}
			else {
				vbuffer[index] = norms[idx * 3];
				vbuffer[index + 1] = norms[idx * 3 + 1];
				vbuffer[index + 2] = norms[idx * 3 + 2];
				index += 3;
			}
			pos = !pos;
		}
	}

	// Temporary bind of our shader.
	shader->bind();

	// Create and prepare a vbo
	vbo_.create();
	vbo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// Bind our vbo inside our vao
	vbo_.bind();
	vbo_.allocate(&(vbuffer[0]), vbuffer.size() * sizeof(GL_FLOAT));

	// Create and prepare an ibo
	ibo_.create();
	ibo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
	// Bind our ibo inside our vao
	ibo_.bind();
	ibo_.allocate(&(ibuffer[0]), ibuffer.size() * sizeof(GL_UNSIGNED_INT));

	// Create a VAO to keep track of things for us.
	vao_.create();
	vao_.bind();
	vbo_.bind();
	shader->enableAttributeArray(0);
	shader->setAttributeBuffer(0, GL_FLOAT, 0, 3, 3 * sizeof(GL_FLOAT));
	shader->enableAttributeArray(1);
	shader->setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GL_FLOAT), 3, 3 * sizeof(GL_FLOAT));
	ibo_.bind();
	// Release the vao THEN the vbo
	vao_.release();
	shader->release();

	vertsToDraw = ibuffer.size();
}

Model::~Model() {
	vbo_.release();
	vbo_.destroy();
	ibo_.release();
	ibo_.destroy();
	vao_.release();
	vao_.destroy();
}