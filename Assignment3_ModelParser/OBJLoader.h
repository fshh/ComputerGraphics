#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include "Model.h"

class OBJLoader {
private:
	static bool isOBJFile(std::string fileName);
public:
	static Model& loadOBJ(std::string fileName, QOpenGLShaderProgram* shader);
};

#endif