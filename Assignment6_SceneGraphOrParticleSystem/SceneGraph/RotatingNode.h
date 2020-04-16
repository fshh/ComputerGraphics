#pragma once

#include "SceneNode.h"

class RotatingNode: public SceneNode
{
public:
	RotatingNode(Renderable* renderable = nullptr, float rotationsPerSecond = 1.0f, const QVector3D& rotationAxis = QVector3D(0, 1, 0));
	~RotatingNode() {}

	void update(const qint64 msSinceLastFrame) override;

	inline float getRotationsPerSecond() const { return rotationsPerSecond; }
	inline void setRotationsPerSecond(float rotationsPerSecond) { this->rotationsPerSecond = rotationsPerSecond; }

	inline QVector3D getRotationAxis() const { return rotationAxis; }
	inline void setRotationAxis(const QVector3D& rotationAxis) { this->rotationAxis = rotationAxis; }

private:
	float rotationsPerSecond;
	QVector3D rotationAxis;
};
