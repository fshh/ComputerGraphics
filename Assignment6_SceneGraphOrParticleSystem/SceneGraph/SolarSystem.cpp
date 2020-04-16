#include "SolarSystem.h"
#include "RotatingNode.h"
#include "Sphere.h"


void SolarSystem::createGeometryAndLights()
{
	if (!sphere)
	{
		const Sphere sphereTris = Sphere();
		sphere = new Renderable();
		sphere->init(sphereTris.vertices(), sphereTris.faces());
	}

	if (!sunLight)
	{
		sunLight = new QVector<PointLight>();
		sunLight->append(PointLight(QVector3D(0, 0, 0), QVector3D(1, 1, 1), 0.0f, 1.0f, 1.0f, 0.0f, 0.0f));
	}

	if (!lightForSun)
	{
		lightForSun = new QVector<PointLight>();
		lightForSun->append(PointLight(QVector3D(0, 0, 0), QVector3D(1, 1, 1), 2.0f, 0.0f));
	}
}

void SolarSystem::deleteGeometryAndLights()
{
	if (sphere) { delete sphere; }
	if (sunLight) { delete sunLight; }
	if (lightForSun) { delete lightForSun; }
}

SolarSystem::~SolarSystem()
{
	deleteGeometryAndLights();
}


SolarSystem::SolarSystem(): sphere(nullptr), sunLight(nullptr), lightForSun(nullptr)
{
	// Prepare texture directory
	QDir texDir = QDir::current();
	while (!texDir.exists("textures")) {
		texDir.cdUp();
	}
	texDir.cd("textures");

	// Helper function for loading textures
	auto loadImage = [&](const QString& imageName) -> QImage
	{
		return QImage(texDir.filePath(imageName));
	};

	// Helper function for constructing transforms
	auto translationMatrix = [](const QVector3D& translation) -> QMatrix4x4
	{
		QMatrix4x4 matrix = QMatrix4x4();
		matrix.translate(translation);
		return matrix;
	};
	
	qDebug() << "Loading solar system...";
	createGeometryAndLights();

	// ~~~~~~~~~~ SUN ~~~~~~~~~~
	qDebug() << "  Loading Sun...";
	SceneNode* sun = new RotatingNode(sphere, 0.02f);
	sun->setModelScale(QVector3D(3.0f, 3.0f, 3.0f));
	sun->setDiffuseMap(loadImage("sun.ppm"));
	sun->setLights(lightForSun);
	addChild(sun);

	// ~~~~~~~~~~ MERCURY ~~~~~~~~~~
	qDebug() << "  Loading Mercury...";
	SceneNode* mercury = new RotatingNode(sphere, 0.02f);
	mercury->setLocalTransform(translationMatrix(QVector3D(-2.0f, 0.0f, -5.0f)));
	mercury->setModelScale(QVector3D(0.25f, 0.25f, 0.25f));
	mercury->setDiffuseMap(loadImage("mercury.ppm"));
	mercury->setLights(sunLight);
	sun->addChild(mercury);

	// ~~~~~~~~~~ VENUS ~~~~~~~~~~
	qDebug() << "  Loading Venus...";
	SceneNode* venus = new RotatingNode(sphere, 0.03f);
	venus->setLocalTransform(translationMatrix(QVector3D(6.0f, 0.0f, 3.0f)));
	venus->setModelScale(QVector3D(0.35f, 0.35f, 0.35f));
	venus->setDiffuseMap(loadImage("venus.ppm"));
	venus->setLights(sunLight);
	sun->addChild(venus);

	// ~~~~~~~~~~ EARTH & MOON ~~~~~~~~~~
	qDebug() << "  Loading Earth...";
	SceneNode* earth = new RotatingNode(sphere, 0.05f);
	earth->setLocalTransform(translationMatrix(QVector3D(10.0f, 0.0f, 0.0f)));
	earth->setModelScale(QVector3D(0.5f, 0.5f, 0.5f));
	earth->setDiffuseMap(loadImage("earth.ppm"));
	earth->setLights(sunLight);
	sun->addChild(earth);

	qDebug() << "    Loading Moon...";
	SceneNode* moon = new RotatingNode(sphere, 0.0f);
	moon->setLocalTransform(translationMatrix(QVector3D(0.0f, 0.0f, 1.0f)));
	moon->setModelScale(QVector3D(0.1f, 0.1f, 0.1f));
	moon->setDiffuseMap(loadImage("moon.ppm"));
	moon->setLights(sunLight);
	earth->addChild(moon);

	// ~~~~~~~~~~ MARS & MOONS ~~~~~~~~~~
	qDebug() << "  Loading Mars...";
	SceneNode* mars = new RotatingNode(sphere, 0.045f);
	mars->setLocalTransform(translationMatrix(QVector3D(-10.0f, 0.0f, 5.0f)));
	mars->setModelScale(QVector3D(0.4f, 0.4f, 0.4f));
	mars->setDiffuseMap(loadImage("mars.ppm"));
	mars->setLights(sunLight);
	sun->addChild(mars);

	qDebug() << "    Loading Phobos...";
	SceneNode* phobos = new RotatingNode(sphere, 0.07f);
	phobos->setLocalTransform(translationMatrix(QVector3D(-0.4f, 0.0f, -0.4f)));
	phobos->setModelScale(QVector3D(0.09f, 0.09f, 0.09f));
	phobos->setDiffuseMap(loadImage("moon.ppm"));
	phobos->setLights(sunLight);
	mars->addChild(phobos);

	qDebug() << "    Loading Deimos...";
	SceneNode* deimos = new RotatingNode(sphere, 0.05f);
	deimos->setLocalTransform(translationMatrix(QVector3D(-0.2f, 0.0f, 0.5f)));
	deimos->setModelScale(QVector3D(0.08f, 0.08f, 0.08f));
	deimos->setDiffuseMap(loadImage("moon.ppm"));
	deimos->setLights(sunLight);
	mars->addChild(deimos);

	// ~~~~~~~~~~ JUPITER & MOONS ~~~~~~~~~~
	qDebug() << "  Loading Jupiter...";
	SceneNode* jupiter = new RotatingNode(sphere, 0.06f);
	jupiter->setLocalTransform(translationMatrix(QVector3D(13.0f, 0.0f, 18.0f)));
	jupiter->setModelScale(QVector3D(1.5f, 1.5f, 1.5f));
	jupiter->setDiffuseMap(loadImage("jupiter.ppm"));
	jupiter->setLights(sunLight);
	sun->addChild(jupiter);

	qDebug() << "    Loading Europa...";
	SceneNode* europa = new RotatingNode(sphere, 0.08f);
	europa->setLocalTransform(translationMatrix(QVector3D(2.0f, 0.0f, 0.0f)));
	europa->setModelScale(QVector3D(0.12f, 0.12f, 0.12f));
	europa->setDiffuseMap(loadImage("europa.ppm"));
	europa->setLights(sunLight);
	jupiter->addChild(europa);

	qDebug() << "    Loading Ganymede...";
	SceneNode* ganymede = new RotatingNode(sphere, 0.08f);
	ganymede->setLocalTransform(translationMatrix(QVector3D(-1.5f, 0.0f, 2.5f)));
	ganymede->setModelScale(QVector3D(0.15f, 0.15f, 0.15f));
	ganymede->setDiffuseMap(loadImage("ganymede.ppm"));
	ganymede->setLights(sunLight);
	jupiter->addChild(ganymede);

	qDebug() << "    Loading Io...";
	SceneNode* io = new RotatingNode(sphere, 0.08f);
	io->setLocalTransform(translationMatrix(QVector3D(-1.0f, 0.0f, -2.0f)));
	io->setModelScale(QVector3D(0.1f, 0.1f, 0.1f));
	io->setDiffuseMap(loadImage("io.ppm"));
	io->setLights(sunLight);
	jupiter->addChild(io);
}

