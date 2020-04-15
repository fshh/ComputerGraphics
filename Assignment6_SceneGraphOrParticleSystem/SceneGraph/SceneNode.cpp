#include "SceneNode.h"

SceneNode::SceneNode(Renderable* renderable): parent(nullptr), modelScale(1, 1, 1), diffuseMap(QOpenGLTexture::Target2D), normalMap(QOpenGLTexture::Target2D), lights(nullptr)
{
	this->renderable = renderable;
}

SceneNode::~SceneNode() {
	if (diffuseMap.isCreated()) {
		diffuseMap.destroy();
	}
	if (normalMap.isCreated()) {
		normalMap.destroy();
	}
	for (SceneNode* child : children) {
		delete child;
	}
}

void SceneNode::update(const qint64 msSinceLastFrame) {
	if (parent) {
		worldTransform = parent->worldTransform * localTransform;
	}
	else {
		worldTransform = localTransform;
	}

	for (SceneNode* child : children) {
		child->update(msSinceLastFrame);
	}
}

void SceneNode::draw(const QMatrix4x4& worldSpaceModelMatrix, const QVector3D& viewPosition, const QMatrix4x4& viewMatrix, const QMatrix4x4& projectionMatrix, const DrawMode drawMode) {
	if (renderable)
	{
		renderable->draw(worldSpaceModelMatrix, viewPosition, viewMatrix, projectionMatrix, drawMode, &diffuseMap, &normalMap, lights);
	}
}

void SceneNode::addChild(SceneNode* node) {
	node->parent = this;
	children << node;
}