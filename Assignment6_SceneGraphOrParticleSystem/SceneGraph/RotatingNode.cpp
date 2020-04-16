#include "RotatingNode.h"

RotatingNode::RotatingNode(Renderable* renderable, float rotationsPerSecond, const QVector3D& rotationAxis):
	SceneNode(renderable)
{
	this->rotationsPerSecond = rotationsPerSecond;
	this->rotationAxis = rotationAxis;
}


void RotatingNode::update(const qint64 msSinceLastFrame)
{
	// angleThisFrame = anglePerSecond * percentOfSecondPassed
	//								= (fullRotationAngle * rotationsPerSecond) * (msSinceLastFrame / msPerSecond)
	const float angleThisFrame = (360.0f * rotationsPerSecond) * (float(msSinceLastFrame) / 1000.0f);
	QMatrix4x4 rotation = QMatrix4x4();
	rotation.rotate(angleThisFrame, rotationAxis);
	localTransform *= rotation;

	SceneNode::update(msSinceLastFrame);
}

