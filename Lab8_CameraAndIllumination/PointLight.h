//
//  PointLight.h
//  App
//
//  Created by Andrew Knollmeyer on 4/1/20.
//

#pragma once

#include <QtCore>
#include <QtGui>
#include <QtOpenGL>

class PointLight
{
public:
	PointLight(QVector3D color, QVector3D position);
	
	void update(const qint64 msSinceLastFrame);
	
	inline QVector3D color() const { return color_; }
	inline QVector3D position() const { return position_; }
	inline float ambientIntensity() const { return ambientIntensity_; }
	inline float specularIntensity() const { return specularIntensity_; }
	inline float constant() const { return constant_; }
	inline float linear() const { return linear_; }
	inline float quadratic() const { return quadratic_; }
	
private:
	QVector3D color_;
	QVector3D position_;
	float ambientIntensity_;
	float specularIntensity_;
	float constant_;
	float linear_;
	float quadratic_;
};
