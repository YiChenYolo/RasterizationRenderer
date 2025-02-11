#ifndef __MODEL_H__
#define __MODEL_H__
#include <vector>
#include <Eigen/Dense>
#include "tgaimage.h"

struct Point {
	int vert;
	int tex;
	int norm;
	Point() :vert(-1), tex(-1), norm(-1) {}
	Point(int _vert, int _tex, int _norm) :vert(_vert), tex(_tex), norm(_norm) {}
};

class Model{
private:
	TGAImage *texture_;
	std::vector<std::vector<Point>> faces_;
	std::vector<Eigen::Vector4f> verts_;
	std::vector<Eigen::Vector2f> texs_;
	std::vector<Eigen::Vector4f> norms_;
public:
	Model(const char* filename);
	~Model();
	size_t nverts();
	size_t nfaces();
	size_t ntexs();
	size_t nnorms();
	void load_texture(const char* model_path);
	std::vector<Point> getFace(int idx);
	bool hasTexture();
	TGAImage* getTexture();
	Eigen::Vector4f getVert(int i);
	Eigen::Vector2f getTex(int i);
	Eigen::Vector4f getNorm(int i);
};
#endif // __MODEL_H__

