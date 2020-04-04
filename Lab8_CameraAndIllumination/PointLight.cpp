#include "PointLight.h"

PointLight::PointLight(QVector3D color, QVector3D position):
	color_(color), position_(position),
	ambientIntensity_(0.5f),
	specularIntensity_(0.5f),
	constant_(1.0f),
	linear_(0.09f),
	quadratic_(0.032f)
{
}

void
PointLight::update(const qint64 msSinceLastFrame)
{
	// This is where we want to maintain our light.
	float secs = (float)msSinceLastFrame / 1000.0f;
	float angle = secs * 180.0f;
	// Rotate our light around the scene
	QMatrix4x4 rot;
	rot.setToIdentity();
	rot.rotate(angle, 0.0, 1.0, 0.0);
	
	position_ = rot * position_;
}
