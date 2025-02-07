#ifndef __MODEL_H__
#define __MODEL_H__
#include <vector>
#include "geometry.h"

class Model
{
private:
	std::vector<std::vector<int>> faces_;
	std::vector<Vec3f> verts_;
public:
	Model(const char* filename);
	~Model();
	size_t nverts();
	size_t nfaces();
	std::vector<int> getFace(int idx);
	Vec3f getVert(int i);
};
#endif // __MODEL_H__

