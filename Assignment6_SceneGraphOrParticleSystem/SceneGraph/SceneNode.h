#pragma once

#include <QtCore>
#include <QtOpenGL>
#include "Renderable.h"

class SceneNode {
public:
	SceneNode(Renderable* renderable = nullptr);
	virtual ~SceneNode();

	// Update and draw
	virtual void update(const qint64 msSinceLastFrame);
	virtual void draw(const QMatrix4x4& worldSpaceModelMatrix, const QVector3D& viewPosition, const QMatrix4x4& viewMatrix, const QMatrix4x4& projectionMatrix, const DrawMode drawMode);

	// Add children
	void addChild(SceneNode* node);

	// Setters/getters
	inline void setLocalTransform(const QMatrix4x4& transform) { localTransform = transform; }
	inline const QMatrix4x4& getLocalTransform() const { return localTransform; }

	inline const QMatrix4x4& getWorldTransform() const { return worldTransform; }

	inline const QVector3D& getModelScale() const { return modelScale; }
	inline void setModelScale(const QVector3D& scale) { modelScale = scale; }
	
	inline void setRenderable(Renderable* renderable) { this->renderable = renderable; }
	inline Renderable* getRenderable() const { return renderable; }

	inline const QOpenGLTexture& getDiffuseMap() const { return diffuseMap; }
	inline void setDiffuseMap(const QImage& diffuseMap) { this->diffuseMap.setData(diffuseMap); }
	
	inline const QOpenGLTexture& getNormalMap() const { return normalMap; }
	inline void setNormalMap(const QImage& normalMap) { this->normalMap.setData(normalMap); }

	inline void setLights(QVector<PointLight>* lights) { this->lights = lights; }
	inline QVector<PointLight>* getLights() const { return lights; }
	
	// Iterators for children
	inline QVector<SceneNode*>::const_iterator begin() { return children.begin(); }
	inline QVector<SceneNode*>::const_iterator end() { return children.end(); }

protected:
	SceneNode* parent;
	QVector<SceneNode*> children;
	
	Renderable* renderable;
	
	QMatrix4x4 localTransform;
	QMatrix4x4 worldTransform;
	QVector3D modelScale;

	QOpenGLTexture diffuseMap;
	QOpenGLTexture normalMap;

	QVector<PointLight>* lights;
};


