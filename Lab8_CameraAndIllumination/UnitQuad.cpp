#include "UnitQuad.h"

UnitQuad::UnitQuad() : sign_(1.0f)
{}

UnitQuad::~UnitQuad()
{}

void UnitQuad::init(const QString& textureFile, QList<std::shared_ptr<PointLight>>& lights)
{
	// The unit quad goes from 0.0 to 1.0 in each dimension.
	QVector<QVector3D> pos;
	QVector<QVector3D> norm;
	QVector<QVector2D> texCoord;
	QVector<unsigned int> idx;
    // unit positions.
    pos << QVector3D(0.0, 0.0, 0.0);
    pos << QVector3D(1.0, 0.0, 0.0);
    pos << QVector3D(0.0, 1.0, 0.0);
    pos << QVector3D(1.0, 1.0, 0.0);
    // We use normals for shading and lighting
    norm << QVector3D(0.0, 0.0, 1.0);
    norm << QVector3D(0.0, 0.0, 1.0);
    norm << QVector3D(0.0, 0.0, 1.0);
    norm << QVector3D(0.0, 0.0, 1.0);
    // Add in the texcoords
    texCoord << QVector2D(0.0, 0.0);
    texCoord << QVector2D(1.0, 0.0);
    texCoord << QVector2D(0.0, 1.0);
    texCoord << QVector2D(1.0, 1.0);
    idx << 0 << 1 << 2 << 2 << 1 << 3;
    Renderable::init(pos, norm, texCoord, idx, textureFile);
	
	lights_ = lights;
}

void UnitQuad::update(const qint64 msSinceLastFrame)
{
	shader_.bind();
	for (int ii = 0; ii < lights_.size(); ii++)
	{
		std::shared_ptr<PointLight> light = lights_[ii];
		QVector3D lightPos = light->position();
		lightPos.setX(0.5);
		// Because we aren't doing any occlusion, the lighting on the walls looks
		// super wonky.  Instead, just move the light on the z axis.
		
		std::string prefix = "pointLights[" + std::to_string(ii) + "].";
		shader_.setUniformValue((prefix + "color").c_str(), light->color());
		shader_.setUniformValue((prefix + "position").c_str(), lightPos);
		shader_.setUniformValue((prefix + "ambientIntensity").c_str(), light->ambientIntensity());
		shader_.setUniformValue((prefix + "specularIntensity").c_str(), light->specularIntensity());
		shader_.setUniformValue((prefix + "constant").c_str(), light->constant());
		shader_.setUniformValue((prefix + "linear").c_str(), light->linear());
		shader_.setUniformValue((prefix + "quadratic").c_str(), light->quadratic());
	}
	shader_.release();
}
