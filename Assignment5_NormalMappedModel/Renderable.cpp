#include "Renderable.h"

#include <QtCore>
#include <QtGui>
#include <QtOpenGL>
#include <QOpenGLFunctions_3_3_core>

Renderable::Renderable() : vbo_(QOpenGLBuffer::VertexBuffer), ibo_(QOpenGLBuffer::IndexBuffer), diffuseMap_(QOpenGLTexture::Target2D), normalMap_(QOpenGLTexture::Target2D), numTris_(0), vertexSize_(0), rotationAxis_(0.0, 1.0, 0.0), rotationSpeed_(0.1)
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

void Renderable::init(const QVector<Vertex>& vertices, const QVector<Face>& faces, const QString& diffuseMap, const QString& normalMap)
{
	initializeOpenGLFunctions();

	// Set our model matrix to identity
	modelMatrix_.setToIdentity();
	// Load diffuse map
	diffuseMap_.setData(QImage(diffuseMap));

	// Load normal map
	if (!normalMap.isEmpty()) {
		normalMap_.setData(QImage(normalMap));
	}
	
	// set our vertex size
	vertexSize_ = sizeof(Vertex);
	
	// set our number of triangles.
	numTris_ = faces.size();

	// Setup our shader.
	createShaders();

	// Now we can set up our buffers.
	// The VBO is created -- now we must create our VAO
	vao_.create();
	vao_.bind();
	vbo_.create();
	vbo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
	vbo_.bind();
	vbo_.allocate(&vertices[0], vertices.size() * vertexSize_);

	// Create our index buffer
	ibo_.create();
	ibo_.bind();
	ibo_.setUsagePattern(QOpenGLBuffer::StaticDraw);
	ibo_.allocate(&faces[0], faces.size() * sizeof(Face));

	// Make sure we setup our shader inputs properly
	// position
	shader_.enableAttributeArray(0);
	shader_.setAttributeBuffer(0, GL_FLOAT, 0, 3, vertexSize_);
	// texture coords
	shader_.enableAttributeArray(1);
	shader_.setAttributeBuffer(1, GL_FLOAT, 3 * sizeof(float), 2, vertexSize_);
	// normal
	shader_.enableAttributeArray(2);
	shader_.setAttributeBuffer(2, GL_FLOAT, 5 * sizeof(float), 3, vertexSize_);
	// tangent
	shader_.enableAttributeArray(3);
	shader_.setAttributeBuffer(3, GL_FLOAT, 8 * sizeof(float), 3, vertexSize_);

	// Release our vao and THEN release our buffers.
	vao_.release();
	vbo_.release();
	ibo_.release();

	// Set up our lights
	lights_ << PointLight(QVector3D(0.0f, 1.0f, 4.0f), QVector3D(1.0f, 1.0f, 1.0f), 0.2f, 0.8f);
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

void Renderable::draw(const QMatrix4x4& worldMatrix, const QMatrix4x4& viewMatrix, const QMatrix4x4& projection, const QVector3D& viewPosition, const DrawMode drawMode)
{
	// Create model matrix
	QMatrix4x4 rotMatrix;
	rotMatrix.setToIdentity();
	rotMatrix.rotate(rotationAngle_, rotationAxis_);
	QMatrix4x4 modelMat = modelMatrix_ * rotMatrix;
	modelMat = worldMatrix * modelMat;
	QMatrix3x3 normalMat = modelMat.normalMatrix();

	// Bind shader
	shader_.bind();
	
	bool hasNormalMap = normalMap_.isCreated();

	// Set our matrix uniforms!
	shader_.setUniformValue("modelMatrix", modelMat);
	shader_.setUniformValue("viewMatrix", viewMatrix);
	shader_.setUniformValue("projectionMatrix", projection);
	shader_.setUniformValue("normalMatrix", normalMat);
	shader_.setUniformValue("viewPosition", viewPosition);
	shader_.setUniformValue("drawMode", (int)drawMode);
	shader_.setUniformValue("hasNormalMap", hasNormalMap);
	for (int ii = 0; ii < lights_.size(); ++ii) {
		const PointLight& light = lights_[ii];
		char buffer[64];

		sprintf(buffer, "pointLights[%i].position", ii);
		shader_.setUniformValue(buffer, light.position);
		sprintf(buffer, "pointLights[%i].color", ii);
		shader_.setUniformValue(buffer, light.color);
		sprintf(buffer, "pointLights[%i].ambientIntensity", ii);
		shader_.setUniformValue(buffer, light.ambientIntensity);
		sprintf(buffer, "pointLights[%i].specularIntensity", ii);
		shader_.setUniformValue(buffer, light.specularIntensity);
		sprintf(buffer, "pointLights[%i].constant", ii);
		shader_.setUniformValue(buffer, light.constant);
		sprintf(buffer, "pointLights[%i].linear", ii);
		shader_.setUniformValue(buffer, light.linear);
		sprintf(buffer, "pointLights[%i].quadratic", ii);
		shader_.setUniformValue(buffer, light.quadratic);
	}

	// Bind VAO
	vao_.bind();

	// Bind textures
	glActiveTexture(GL_TEXTURE0);
	diffuseMap_.bind();
	shader_.setUniformValue(shader_.attributeLocation("diffuseMap"), GL_TEXTURE0);
	if (hasNormalMap) {
		glActiveTexture(GL_TEXTURE1);
		normalMap_.bind();
		shader_.setUniformValue("normalMap", GL_TEXTURE1 - GL_TEXTURE0);
	}

	// Draw!
	glDrawElements(GL_TRIANGLES, numTris_ * 3, GL_UNSIGNED_INT, 0);

	// Un-bind textures
	diffuseMap_.release();
	if (hasNormalMap) {
		normalMap_.release();
	}

	// Un-bind VAO and shader
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
