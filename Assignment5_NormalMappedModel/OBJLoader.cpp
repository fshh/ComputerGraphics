#include "OBJLoader.h"
#include <fstream>

Vec3 tangentFace(const Face& face, const QVector<Vertex>& vertices) {
	Vertex v1 = vertices[face.a];
	Vertex v2 = vertices[face.b];
	Vertex v3 = vertices[face.c];

	Vec3 edge1 = v2.position - v1.position;
	Vec2 deltaUV1 = v2.texCoord - v1.texCoord;

	Vec3 edge2 = v3.position - v1.position;
	Vec2 deltaUV2 = v3.texCoord - v1.texCoord;

	float f = 1.0f / (deltaUV1.u * deltaUV2.v - deltaUV2.u * deltaUV1.v);

	Vec3 tangent;
	tangent.x = f * (deltaUV2.v * edge1.x - deltaUV1.v * edge2.x);
	tangent.y = f * (deltaUV2.v * edge1.y - deltaUV1.v * edge2.y);
	tangent.z = f * (deltaUV2.v * edge1.z - deltaUV1.v * edge2.z);
	tangent.normalize();

	return tangent;
}

bool OBJLoader::isOBJFile(QString filePath) {
	// find last occurence of a period in the string, so we can get the file extension
	return filePath.contains(".obj");
}

void loadMaterial(const QString& objFilePath, const QStringList& line, QString& diffuseMap, QString& normalMap) {
	// get path to mtl file
	QDir dir(objFilePath);
	dir.cdUp();
	QFile mtl(dir.filePath(line[1]));

	// make sure file opened
	if (!mtl.open(QIODevice::ReadOnly | QIODevice::Text)) {
		qDebug() << "ERROR: Failed to open .mtl file" << objFilePath;
		return;
	}

	// scan file for map_Kd
	QTextStream mtlIn(&mtl);
	while (!mtlIn.atEnd() && (diffuseMap == "" || normalMap == "")) {
		QStringList mtlLine = mtlIn.readLine().split(' ');
		if (mtlLine[0] == "map_Kd") {
			diffuseMap = dir.filePath(mtlLine[1]);
		}
		if (mtlLine[0] == "map_Bump") {
			normalMap = dir.filePath(mtlLine[1]);
		}
	}

	if (normalMap.isEmpty()) { qDebug() << "No normal map found in mtl file " + mtl.fileName(); }
}

Vec3 loadVec3(const QStringList& line) {
	return Vec3(line[1].toFloat(), line[2].toFloat(), line[3].toFloat());
}

Vec2 loadVec2(const QStringList& line) {
	return Vec2(line[1].toFloat(), line[2].toFloat());
}

void loadFace(const QStringList& line, const QVector<Vec3>& positions, const QVector<Vec2>& texCoords, const QVector<Vec3>& normals, QVector<Vertex>& vertices, QVector<Face>& faces) {
	Face face;
	for (int ii = 1; ii < line.length(); ++ii) {
		QString str = line[ii];
		QStringList indices = str.split('/');

		// Construct vertex and add to list of vertices if it is unique
		Vertex vert;
		vert.position = positions[indices[0].toUInt() - 1];
		vert.texCoord = texCoords[indices[1].toUInt() - 1];
		vert.normal = normals[indices[2].toUInt() - 1];
		
		// if vertex is new, give existing index to face. otherwise, add vertex to list and give index
		int index = vertices.indexOf(vert);
		if (index == -1) {
			index = vertices.length();
			vertices << vert;
		}
		face[ii - 1] = index;
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
	QVector<Vec3> positions;
	QVector<Vec2> texCoords;
	QVector<Vec3> normals;
	QVector<Vec3> tangents;
	QVector<Vertex> vertices;
	QVector<Face> faces;
	
	// process the file
	QTextStream in(&file);
	while (!in.atEnd()) {
		QStringList line = in.readLine().split(' ');
		
		QString lineType = line[0];
		if (lineType == "mtllib") {
			loadMaterial(filePath, line, diffuseMap, normalMap);
		}
		else if (lineType == "v") {
			positions << loadVec3(line);
		}
		else if (lineType == "vt") {
			texCoords << loadVec2(line);
		}
		else if (lineType == "vn") {
			normals << loadVec3(line);
		}
		else if (lineType == "f") {
			loadFace(line, positions, texCoords, normals, vertices, faces);
		}
	}
	
	/*
	qDebug() << "Positions";
	for (Vec3& pos : positions) {
		qDebug() << pos.x << pos.y << pos.z;
	}
	
	qDebug() << "TexCoords";
	for (Vec2& texCoord : texCoords) {
		qDebug() << texCoord.u << texCoord.v;
	}
	
	qDebug() << "Normals";
	for (Vec3& norm : normals) {
		qDebug() << norm.x << norm.y << norm.z;
	}

	qDebug() << "Vertices";
	for (Vertex& vert : vertices) {
		qDebug() << vert.position.x << vert.position.y << vert.position.z << vert.texCoord.u << vert.texCoord.v << vert.normal.x << vert.normal.y << vert.normal.z << vert.tangent.x << vert.tangent.y << vert.tangent.z;
	}
	
	qDebug() << "Faces";
	for (Face& face : faces) {
		qDebug() << face.a << face.b << face.c;
	}

	qDebug() << "Diffuse Map";
	qDebug() << diffuseMap;

	qDebug() << "Normal Map";
	qDebug() << normalMap;
	*/
	
	Renderable* ren = new Renderable();
	ren->init(vertices, faces, diffuseMap, normalMap);
	
	return ren;
}
