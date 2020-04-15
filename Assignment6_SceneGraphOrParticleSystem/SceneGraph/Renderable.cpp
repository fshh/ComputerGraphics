#include "Renderable.h"

#include <QtCore>
#include <QtGui>
#include <QtOpenGL>
#include <QOpenGLFunctions_3_3_core>

Renderable::Renderable() : vbo_(QOpenGLBuffer::VertexBuffer), ibo_(QOpenGLBuffer::IndexBuffer), numTris_(0), vertexSize_(0)
{

}

Renderable::~Renderable()
{
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

void Renderable::init(const QVector<Vertex>& vertices, const QVector<Face>& faces)
{
	initializeOpenGLFunctions();
	
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
}

void Renderable::draw(const QMatrix4x4& worldSpaceModelMatrix, const QVector3D& viewPosition, const QMatrix4x4& viewMatrix, const QMatrix4x4& projectionMatrix, 
	const DrawMode drawMode, QOpenGLTexture* diffuseMap, QOpenGLTexture* normalMap, QVector<PointLight>* lights)
{
	// Create normal matrix
	const QMatrix3x3 normalMatrix = worldSpaceModelMatrix.normalMatrix();

	// Bind shader
	shader_.bind();

	const bool hasDiffuseMap = diffuseMap && diffuseMap->isCreated();
	const bool hasNormalMap = normalMap && normalMap->isCreated();

	// Set our matrix uniforms!
	shader_.setUniformValue("modelMatrix", worldSpaceModelMatrix);
	shader_.setUniformValue("viewPosition", viewPosition);
	shader_.setUniformValue("viewMatrix", viewMatrix);
	shader_.setUniformValue("projectionMatrix", projectionMatrix);
	shader_.setUniformValue("normalMatrix", normalMatrix);
	shader_.setUniformValue("drawMode", int(drawMode));
	shader_.setUniformValue("hasNormalMap", hasNormalMap);
	for (int ii = 0; ii < lights->size(); ++ii) {
		const PointLight& light = (*lights)[ii];
		const int buflen = 64;
		char buffer[buflen];

		sprintf_s(buffer, buflen, "pointLights[%i].position", ii);
		shader_.setUniformValue(buffer, light.position);
		sprintf_s(buffer, buflen, "pointLights[%i].color", ii);
		shader_.setUniformValue(buffer, light.color);
		sprintf_s(buffer, buflen, "pointLights[%i].ambientIntensity", ii);
		shader_.setUniformValue(buffer, light.ambientIntensity);
		sprintf_s(buffer, buflen, "pointLights[%i].specularIntensity", ii);
		shader_.setUniformValue(buffer, light.specularIntensity);
		sprintf_s(buffer, buflen, "pointLights[%i].constant", ii);
		shader_.setUniformValue(buffer, light.constant);
		sprintf_s(buffer, buflen, "pointLights[%i].linear", ii);
		shader_.setUniformValue(buffer, light.linear);
		sprintf_s(buffer, buflen, "pointLights[%i].quadratic", ii);
		shader_.setUniformValue(buffer, light.quadratic);
	}

	// Bind VAO
	vao_.bind();

	// Bind textures
	if (hasDiffuseMap) {
		glActiveTexture(GL_TEXTURE0);
		diffuseMap->bind();
		shader_.setUniformValue(shader_.attributeLocation("diffuseMap"), GL_TEXTURE0);
	}
	if (hasNormalMap) {
		glActiveTexture(GL_TEXTURE1);
		normalMap->bind();
		shader_.setUniformValue("normalMap", GL_TEXTURE1 - GL_TEXTURE0);
	}

	// Draw!
	glDrawElements(GL_TRIANGLES, numTris_ * 3, GL_UNSIGNED_INT, 0);

	// Un-bind textures
	if (hasDiffuseMap) {
		diffuseMap->release();
	}
	if (hasNormalMap) {
		normalMap->release();
	}

	// Un-bind VAO and shader
	vao_.release();
	shader_.release();
}
