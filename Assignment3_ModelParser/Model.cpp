#include "Model.h"

QString Model::vertexShaderString() const
{
  QString str =
	"#version 330\n"
	"uniform vec3 lightDir = vec3(0.2, -1, 0.2);\n"
	"layout(location = 0) in vec3 position;\n"
  "layout(location = 1) in vec3 normal;\n"
	"out float intensity;\n"
	"void main()\n"
	"{\n"
	"  gl_Position = vec4(position, 1.0);\n"
	"  intensity = dot(lightDir, normal);\n"
  "}\n";
  return str;
}

QString Model::fragmentShaderString() const
{
  QString str =
	"#version 330\n"
  "in float intensity;\n"
	"out vec4 color;\n"
	"void main()\n"
	"{\n"
	"  vec4 vertColor;\n"
	"	 if (intensity > 0.95)\n"
	"    vertColor = vec4(1.0, 0.5, 0.5, 1.0);\n"
	"  else if (intensity > 0.5)\n"
	"    vertColor = vec4(0.6, 0.3, 0.3, 1.0);\n"
	"  else if (intensity > 0.25)\n"
	"    vertColor = vec4(0.4, 0.2, 0.2, 1.0);\n"
	"  else\n"
	"    vertColor = vec4(0.2, 0.1, 0.1, 1.0);\n"
	"  color = vertColor;\n"
	"}\n";
  return str;
}

void Model::createShader()
{
  QOpenGLShader vert(QOpenGLShader::Vertex);
  vert.compileSourceCode(vertexShaderString());
  QOpenGLShader frag(QOpenGLShader::Fragment);
  frag.compileSourceCode(fragmentShaderString());
  bool ok = shaderProgram_.addShader(&vert);
  if(!ok) {
	qDebug() << shaderProgram_.log();
  }
  ok = shaderProgram_.addShader(&frag);
  if(!ok) {
	qDebug() << shaderProgram_.log();
  }
  ok = shaderProgram_.link();
  if(!ok) {
	qDebug() << shaderProgram_.log();
  }
}

void Model::bindVAO() {
	// Create a VAO to keep track of things for us.
	vao_.create();
	vao_.bind();
	vbo_.bind();
	shaderProgram_.enableAttributeArray(0);
	shaderProgram_.setAttributeBuffer(0, GL_FLOAT, 0, 3, 3 * sizeof(GL_FLOAT));
	shaderProgram_.enableAttributeArray(1);
	shaderProgram_.setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(GL_FLOAT), 3, 3 * sizeof(GL_FLOAT));
	ibo_.bind();
	// Release the vao THEN the vbo
	vao_.release();
	shaderProgram_.release();
}

Model::Model(QVector<GLfloat>& verts, QVector<GLfloat>& norms, QVector<QVector<GLuint>>& faces) :
	vbo_(QOpenGLBuffer::VertexBuffer), ibo_(QOpenGLBuffer::IndexBuffer) {
	createShader();
		
	QVector<GLfloat> vbuffer;
	vbuffer.reserve(verts.size() + norms.size());
	QVector<GLuint> ibuffer;

	GLuint index = 0;
	for (QVector<GLuint>& face : faces) {
		bool pos = true;
		for (GLuint& idx : face) {
			if (pos) {
				vbuffer.append(verts[idx]);
				vbuffer.append(verts[idx + 1]);
				vbuffer.append(verts[idx + 2]);
				ibuffer.append(index);
				index += 3;
			}
			else {
				vbuffer.append(norms[idx]);
				vbuffer.append(norms[idx + 1]);
				vbuffer.append(norms[idx + 2]);
				index += 3;
			}
			pos = !pos;
		}
	}
	
	qDebug() << "vbuffer";
	for (size_t ii = 0; ii < vbuffer.size() / 6; ++ii) {
		size_t index = ii * 6;
		qDebug() << vbuffer[index] << vbuffer[index + 1] << vbuffer[index + 2] << vbuffer[index + 3] << vbuffer[index + 4] << vbuffer[index + 5];
	}
	
	qDebug() << "ibuffer";
	for (size_t ii = 0; ii < ibuffer.size(); ++ii) {
		qDebug() << ibuffer[ii];
	}

	// Temporary bind of our shader.
	shaderProgram_.bind();

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
		
	bindVAO();

	vertsToDraw = ibuffer.size();
}

Model::Model(size_t _vertsToDraw, QOpenGLBuffer _vbo_, QOpenGLBuffer _ibo_) {
	createShader();
	bindVAO();
}

Model::Model(const Model& other) : Model(other.vertsToDraw, other.vbo_, other.ibo_) {}

Model::~Model() {
	vbo_.release();
	vbo_.destroy();
	ibo_.release();
	ibo_.destroy();
	vao_.release();
	vao_.destroy();
}
