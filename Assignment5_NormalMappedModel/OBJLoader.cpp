#include "OBJLoader.h"
#include <fstream>

bool OBJLoader::isOBJFile(QString filePath) {
	// find last occurence of a period in the string, so we can get the file extension
	return filePath.contains(".obj");
}

void loadPosition(const QStringList& line, QVector<QVector3D>& positions) {
	QVector3D position(line[1].toFloat(),
										 line[2].toFloat(),
										 line[3].toFloat());
	positions << position;
}

void loadNormal(const QStringList& line, QVector<QVector3D>& normals) {
	QVector3D normal(line[1].toFloat(),
										 line[2].toFloat(),
										 line[3].toFloat());
	normals << normal;
}

void loadTexCoords(const QStringList& line, QVector<QVector2D>& texCoords) {
	QVector2D texCoord(line[1].toFloat(),
										 line[2].toFloat());
	texCoords << texCoord;
}

void loadFace(const QStringList& line, QVector<QVector<unsigned int>>& faces) {
	QStringList::const_iterator it = line.begin() + 1; // skip "f" at beginning of line
	QStringList::const_iterator end = line.end();
	
	// copy values into face
	QVector<unsigned int> face;
	for (; it != end; ++it) {
		QStringList indices = it->split('/');
		face << indices[0].toUInt() - 1 << indices[1].toUInt() - 1 << indices[2].toUInt() - 1;
	}
	
	// add face to list
	faces << face;
}

Renderable* OBJLoader::loadOBJ(QString filePath) {
	// check that we have been given a .obj file
	if (!isOBJFile(filePath)) {
		qDebug() << "ERROR: Expected a .obj file for constructing a model, got" << filePath;
		return nullptr;
	}
	
	// open the file
	QFile file(filePath);
	if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "ERROR: Failed to open .obj file" << filePath;
		return nullptr;
	}
	
	// prepare textureFile
	QString diffuseMap = "";
	QString normalMap = "";
	
	// prepare buffers
	QVector<QVector3D> positions;
	QVector<QVector3D> normals;
	QVector<QVector2D> texCoords;
	QVector<QVector<unsigned int>> faces;
	
	// process the file
	QTextStream in(&file);
	while (!in.atEnd()) {
		QStringList line = in.readLine().split(' ');
		
		QString lineType = line[0];
		if (lineType == "mtllib") {
			// get path to mtl file
			QDir dir(filePath);
			dir.cdUp();
			QFile mtl(dir.filePath(line[1]));
			
			// make sure file opened
			if (!mtl.open(QIODevice::ReadOnly | QIODevice::Text)) {
				qDebug() << "ERROR: Failed to open .mtl file" << filePath;
				return nullptr;
			}
			
			// scan file for map_Kd
			QTextStream mtlIn(&mtl);
			while (!mtlIn.atEnd() && (diffuseMap == "" || normalMap == "")) {
				QStringList line = mtlIn.readLine().split(' ');
				if (line[0] == "map_Kd") {
					diffuseMap = dir.filePath(line[1]);
				}
				if (line[0] == "map_Bump") {
					normalMap = dir.filePath(line[1]);
				}
			}

			if (normalMap.isEmpty()) { qDebug() << "No normal map found in mtl file " + mtl.fileName(); }
		}
		else if (lineType == "v") {
			loadPosition(line, positions);
		}
		else if (lineType == "vt") {
			loadTexCoords(line, texCoords);
		}
		else if (lineType == "vn") {
			loadNormal(line, normals);
		}
		else if (lineType == "f") {
			loadFace(line, faces);
		}
	}
	
	/*
	qDebug() << "Verts";
	for (QVector3D& pos : positions) {
		qDebug() << pos.x() << pos.y() << pos.z();
	}
	
	qDebug() << "Norms";
	for (QVector3D& norm : normals) {
		qDebug() << norm.x() << norm.y() << norm.z();
	}
	
	qDebug() << "TexCoords";
	for (QVector2D& texCoord : texCoords) {
		qDebug() << texCoord.x() << texCoord.y();
	}
	
	qDebug() << "Faces";
	for (QVector<unsigned int>& face : faces) {
		qDebug() << face[0] << face[1] << face[2] << face[3] << face[4] << face[5] << face[6] << face[7] << face[8];
	}
	*/
	
	Renderable* ren = new Renderable();
	ren->init(positions, normals, texCoords, faces, diffuseMap, normalMap);
	
	return ren;
}
