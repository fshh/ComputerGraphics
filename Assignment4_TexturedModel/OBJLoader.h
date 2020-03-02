#ifndef OBJ_LOADER_H
#define OBJ_LOADER_H

#include "Model.h"
#include "Renderable.h"

class OBJLoader {
private:
	static bool isOBJFile(QString filePath);
public:
	static Renderable* loadOBJ(QString filePath);
};

#endif
