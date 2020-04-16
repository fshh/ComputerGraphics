#pragma once

#include "SceneNode.h"

class SolarSystem final: public SceneNode
{
public:
	SolarSystem();
	~SolarSystem();

	void createGeometryAndLights();
	void deleteGeometryAndLights();

protected:
	Renderable* sphere;
	QVector<PointLight>* sunLight;
	QVector<PointLight>* lightForSun;
};
