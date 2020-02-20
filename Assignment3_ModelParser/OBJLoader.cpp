#include "OBJLoader.h"
#include <fstream>

bool OBJLoader::isOBJFile(std::string fileName) {
	// find last occurence of a period in the string, so we can get the file extension
	std::size_t dot = fileName.rfind(".");

	// if filename has a dot, check if extension is ppm
	if (dot != std::string::npos) {
		return fileName.substr(dot + 1) == "obj";
	}
	return false;
}

Model& OBJLoader::loadOBJ(std::string fileName) {
	QVector<GLfloat> verts;
	QVector<GLfloat> norms;
	QVector<QVector<GLuint>> faces;

	// check that we have been given a .obj file
	if (!isOBJFile(fileName)) {
		qDebug() << "ERROR: Expected a .obj file for constructing a model";
		return;
	}

	// open the file
	std::ifstream infile(fileName);

	// string to read file data into
	std::string line;

	// read the file
	if (infile.is_open()) {
		while (std::getline(infile, line)) {
			// skip lines without verts, normals, or faces
			if (line[0] != 'v' && line[0] != 'f') { continue; }

			// save line data as C-style string for strtok
			char* cstr = &(line[0]);

			// read each token in the line
			char* token = std::strtok(cstr, " \n");

			// handle faces
			if (std::strcmp(token, "f") == 0) {
				// create new list for this face (since faces can have any number of vertices)
				QVector<GLuint> ibuffer;

				QVector<char*> temp;

				// append values to the list
				token = std::strtok(nullptr, " \n");
				while (token) {
					// add string to temp buffer
					temp.append(token);
					token = std::strtok(nullptr, " \n");
				}

				for (size_t ii = 0; ii < temp.size(); ++ii) {
					char* subtoken = std::strtok(temp[ii], "//");
					while (subtoken) {
						ibuffer.append((GLuint)std::atoi(subtoken) - 1);
						subtoken = std::strtok(nullptr, "//");
					}
				}

				// add buffer to list of faces
				faces.append(ibuffer);
			}
			// handle verts/norms
			else {
				QVector<GLfloat>* vbuffer;

				// select appropriate buffer
				if (std::strcmp(token, "v") == 0) {
					vbuffer = &verts;
				}
				else if (std::strcmp(token, "vn") == 0) {
					vbuffer = &norms;
				}

				// process values
				token = std::strtok(nullptr, " \n");
				while (token) {
					// save vertex data
					vbuffer->append((GLfloat)std::atof(token));
					token = std::strtok(nullptr, " \n");
				}
			}
		}
	}
	
	qDebug() << "Verts";
	for (size_t ii = 0; ii < verts.size() / 3; ++ii) {
		size_t index = ii * 3;
		qDebug() << verts[index] << verts[index + 1] << verts[index + 2];
	}
	
	qDebug() << "Norms";
	for (size_t ii = 0; ii < norms.size() / 3; ++ii) {
		size_t index = ii * 3;
		qDebug() << norms[index]<< norms[index + 1] << norms[index + 2];
	}
	
	qDebug() << "Faces";
	for (QVector<GLuint>& face : faces) {
		for (size_t ii = 0; ii < face.size() / 6; ++ii) {
			size_t index = ii * 6;
			qDebug() << face[index] << face[index + 1] << face[index + 2] << face[index + 3] << face[index + 4] << face[index + 5];
		}
	}

	Model model(verts, norms, faces);

	// close the file
	infile.close();

	return model;
}
