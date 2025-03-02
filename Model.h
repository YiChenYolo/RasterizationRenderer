#ifndef __MODEL_H__
#define __MODEL_H__
#include <vector>
#include <Eigen/Dense>
#include "tgaimage.h"

struct Face {
	int verts_[3];
	int texs_[3];
	int norms_[3];
	Face() :verts_(), texs_(), norms_() {}
};

struct Material {
	float kd_, ks_, ka_;
	Material(float _kd, float _ks, float _ka) :kd_(_kd), ks_(_ks), ka_(_ka) {}
};

struct Vertex {
	Eigen::Vector4f coord_;
	std::vector<int> adjFaces_;
};

enum Texture {
	Diffuse,
	Spec,
	Glow,
	Nm,
	NmTangent
};

class Model{
private:
	Material mtr_;
	TGAImage* textures_[5];

	std::vector<Face> faces_;
	std::vector<Vertex> verts_;
	std::vector<Eigen::Vector2f> texs_;
	std::vector<Eigen::Vector4f> norms_;
public:
	Model(std::string modelName,Material _mtr);
	~Model();
	size_t nverts();
	size_t nfaces();
	size_t ntexs();
	size_t nnorms();
	int nAdjFaces(int ivert);
	Face getFace(int idx);
	
	int getAdjFace(int ivert, int iface);
	TGAColor getDiffuse(float u, float v);
	TGAColor getDiffuse(Eigen::Vector2f uv);
	TGAColor getSpce(Eigen::Vector2f uv);
	Eigen::Vector4f getNm(Eigen::Vector2f uv);
	Eigen::Vector4f getTanNorm(Eigen::Vector2f uv);
	bool hasDiffuse(){ return textures_[Diffuse]->loaded(); }
	bool hasSpec() { return textures_[Spec]->loaded(); }
	bool hasNm() { return textures_[Nm]->loaded(); }
	bool hasNmTangent() { return textures_[NmTangent]->loaded(); }
	bool hasGlow() { return textures_[Glow]->loaded(); }

	float getKd() { return mtr_.kd_; }
	float getKs() { return mtr_.ks_; }
	float getKa() { return mtr_.ka_; }
	Eigen::Vector4f getVert(int iface, int ipt);
	Eigen::Vector2f getTex(int i);
	Eigen::Vector2f getTex(int iface, int ipt);
	Eigen::Vector4f getNorm(int i);
	Eigen::Vector4f getNorm(int iface, int ipt);
};
#endif // __MODEL_H__

