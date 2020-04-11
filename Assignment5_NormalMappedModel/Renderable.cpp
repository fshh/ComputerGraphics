#include "Renderable.h"

#include <QtCore>
#include <QtGui>
#include <QtOpenGL>
#include <QOpenGLFunctions_3_3_core>

Renderable::Renderable() : vbo_(QOpenGLBuffer::VertexBuffer), ibo_(QOpenGLBuffer::IndexBuffer), diffuseMap_(QOpenGLTexture::Target2D), normalMap_(QOpenGLTexture::Target2D), numTris_(0), vertexSize_(0), rotationAxis_(0.0, 1.0, 0.0), rotationSpeed_(0.2)
{
	rotationAngle_ = 0.0;
}

Renderable::~Renderable()
{
	if (diffuseMap_.isCreated()) {
		diffuseMap_.destroy();
	}
	if (normalMap_.isCreated()) {
		normalMap_.destroy();
	}
	if (vbo_.isCreated()) {
		vbo_.destroy();
	}
	if (ibo_.isCreated()) {
		ibo_.destroy();
	}
	if (vao_.isCreated()) {
		vao_.destroy();
	}
}

void Renderable::createShaders()
{
	QString vertexFilename = "../../vert.glsl";
	bool ok = shader_.addShaderFromSourceFile(QOpenGLShader::Vertex, vertexFilename);
	if (!ok) {
		qDebug() << shader_.log();
	}
	QString fragmentFilename = "../../frag.glsl";
	ok = shader_.addShaderFromSourceFile(QOpenGLShader::Fragment, fragmentFilename);
	if (!ok) {
		qDebug() << shader_.log();
	}
	ok = shader_.link();
	if (!ok) {
		qDebug() << shader_.log();
	}
}

void Renderable::init(const QVector<QVector3D>& positions, const QVector<QVector3D>& normals, const QVector<QVector2D>& texCoords, const QVector<QVector<unsigned int>>& faces, 
	const QString& diffuseMap, const QString& normalMap)
{
	initializeOpenGLFunctions();

	// Set our model matrix to identity
	modelMatrix_.setToIdentity();
	// Load diffuse map
	diffuseMap_.setData(QImage(diffuseMap).mirrored(true, true));

	// Load normal map
	if (!normalMap.isEmpty()) {
		normalMap_.setData(QImage(normalMap).mirrored(true, true));
	}
	
	// create final data buffers
	QVector<float> vbuffer;
	QVector<unsigned int> ibuffer;
	QMap<QVector<float>, unsigned int> vmap;
	int index = 0;
	
	// eliminate duplicate vertices and indexes
	for (const QVector<unsigned int>& face : faces) {
		for (unsigned int ii = 0; ii < face.size() / 3; ++ii) {
			// store index of pos, tex, and norm data
			unsigned int ipos = face[ii * 3];
			unsigned int itex = face[ii * 3 + 1];
			unsigned int inorm = face[ii * 3 + 2];
						
			// construct vertex
			QVector<float> vert;
			
			// append pos data
			QVector3D pos = positions[ipos];
			vert << pos.x() << pos.y() << pos.z();
			
			// append tex data
			QVector2D tex = texCoords[itex];
			vert << tex.x() << tex.y();
			
			// append norm data
			QVector3D norm = normals[inorm];
			vert << norm.x() << norm.y() << norm.z();
			
			// if vertex already exists in map, append its index to ibuffer
			if (vmap.contains(vert)) {
				ibuffer.append(vmap[vert]);
			}
			// if vertex is new: append current index to ibuffer,
			// add vertex to map, and copy data to vbuffer
			else {
				// append current index to ibuffer
				ibuffer.append(index);
				
				// add to map
				vmap.insert(vert, index);
				index++;
				
				// copy data into vbuffer
				vbuffer.append(vert);
			}
		}
	}
	
	/*
	qDebug() << "vbuffer";
	for (size_t ii = 0; ii < vbuffer.size() / 8; ++ii) {
		size_t index = ii * 8;
		qDebug() << vbuffer[index] << vbuffer[index + 1] << vbuffer[index + 2] << vbuffer[index + 3] << vbuffer[index + 4] << vbuffer[index + 5] << vbuffer[index + 6] << vbuffer[index + 7];
	}
	
	qDebug() << "ibuffer";
	for (size_t ii = 0; ii < ibuffer.size() / 3; ++ii) {
		size_t index = ii * 3;
		qDebug() << ibuffer[index] << ibuffer[index + 1] << ibuffer[index + 2];
	}
	 */
	
	// set our vertex size
	// 3 for pos, 2 for tex, 3 for normal
	vertexSize_ = 3 + 2 + 3;
	
	// set our number of trianges.
	numTris_ = ibuffer.size() / 3;

	// Setup our shader.
	createShaders();

	// Now we can set up our buffers.
	// The VBO is created -- now we must create our VAO
	vao_.create();
	vao_.bind();
	vbo_.create();
	vbo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
	vbo_.bind();
	vbo_.allocate(&vbuffer[0], vbuffer.size() * sizeof(float));

	// Create our index buffer
	ibo_.create();
	ibo_.bind();
	ibo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
	ibo_.allocate(&ibuffer[0], ibuffer.size() * sizeof(unsigned int));

	// Make sure we setup our shader inputs properly
	shader_.enableAttributeArray(0);
	shader_.setAttributeBuffer(0, GL_FLOAT, 0, 3, vertexSize_ * sizeof(float));
	shader_.enableAttributeArray(1);
	shader_.setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 2, vertexSize_ * sizeof(float));
	shader_.enableAttributeArray(2);
	shader_.setAttributeBuffer(2, GL_FLOAT, 5 * sizeof(float), 3, vertexSize_ * sizeof(float));

	// Release our vao and THEN release our buffers.
	vao_.release();
	vbo_.release();
	ibo_.release();
}

void Renderable::update(const qint64 msSinceLastFrame)
{
	// For this lab, we want our polygon to rotate. 
	float sec = msSinceLastFrame / 1000.0f;
	float anglePart = sec * rotationSpeed_ * 360.f;
	rotationAngle_ += anglePart;
	while(rotationAngle_ >= 360.0) {
		rotationAngle_ -= 360.0;
	}
}

void Renderable::draw(const QMatrix4x4& view, const QMatrix4x4& projection, const DrawMode drawMode)
{
	// Create our model matrix.
	QMatrix4x4 rotMatrix;
	rotMatrix.setToIdentity();
	rotMatrix.rotate(rotationAngle_, rotationAxis_);

	QMatrix4x4 modelMat = modelMatrix_ * rotMatrix;
	// Make sure our state is what we want
	shader_.bind();
	// Set our matrix uniforms!
	QMatrix4x4 id;
	id.setToIdentity();
	shader_.setUniformValue("modelMatrix", modelMat);
	shader_.setUniformValue("viewMatrix", view);
	shader_.setUniformValue("projectionMatrix", projection);
	shader_.setUniformValue("drawMode", (int)drawMode);

	bool hasNormalMap = normalMap_.isCreated();
	shader_.setUniformValue("hasNormalMap", hasNormalMap);

	vao_.bind();

	glActiveTexture(GL_TEXTURE0);
	diffuseMap_.bind();
	shader_.setUniformValue("diffuseMap", GL_TEXTURE0);

	if (hasNormalMap) {
		glActiveTexture(GL_TEXTURE1);
		normalMap_.bind();
		shader_.setUniformValue("normalMap", GL_TEXTURE1 - GL_TEXTURE0);
	}

	glDrawElements(GL_TRIANGLES, numTris_ * 3, GL_UNSIGNED_INT, 0);

	diffuseMap_.release();
	if (hasNormalMap) {
		normalMap_.release();
	}

	vao_.release();
	shader_.release();
}

void Renderable::setModelMatrix(const QMatrix4x4& transform)
{
	modelMatrix_ = transform;
}

void Renderable::setRotationAxis(const QVector3D& axis)
{
	rotationAxis_ = axis;
}

void Renderable::setRotationSpeed(float speed)
{
	rotationSpeed_ = speed;
}
