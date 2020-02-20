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

Model& OBJLoader::loadOBJ(std::string fileName, QOpenGLShaderProgram* shader) {
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
				QVector<GLuint> buffer;

				QVector<char*> temp;

				// append values to the list
				while (std::strtok(nullptr, " \n")) {
					// add string to temp buffer
					temp.append(token);
				}

				for (size_t ii = 0; ii < temp.size(); ++ii) {
					char* subtoken = std::strtok(temp[ii], "//");
					while (subtoken) {
						buffer.append((GLfloat)std::atoi(subtoken) - 1);
						subtoken = std::strtok(nullptr, "//");
					}
				}

				// add buffer to list of faces
				faces.append(buffer);
			}
			// handle verts/norms
			else {
				QVector<GLfloat> buffer;

				// select appropriate buffer
				if (std::strcmp(token, "v") == 0) {
					buffer = std::move(verts);
				}
				else if (std::strcmp(token, "vn") == 0) {
					buffer = std::move(norms);
				}

				// process values
				while (std::strtok(nullptr, " \n")) {
					// save vertex data
					buffer.append((GLfloat)std::atoi(token) - 1);
				}
			}

			delete[] cstr;
		}
	}

	Model model(verts, norms, faces, shader);

	// close the file
	infile.close();

	return model;
}
