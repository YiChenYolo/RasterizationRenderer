#ifndef __MODEL_H__
#define __MODEL_H__
#include <vector>
#include <Eigen/Dense>

class Model
{
private:
	std::vector<std::vector<int>> faces_;
	std::vector<Eigen::Vector4f> verts_;
public:
	Model(const char* filename);
	~Model();
	size_t nverts();
	size_t nfaces();
	std::vector<int> getFace(int idx);
	Eigen::Vector4f getVert(int i);
};
#endif // __MODEL_H__

