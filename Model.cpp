#include "Model.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <Eigen/Dense>
using namespace std;

Model::Model(string modelName, Material _mtr):verts_(), texs_(), norms_(), faces_(), mtr_(_mtr){
	for (int i = 0; i < 5; i++)
		textures_[i] = new TGAImage();

	ifstream in;
	string model = ".\\obj\\" + modelName + "\\" + modelName;
	string tmp = model + ".obj";

	in.open(tmp, ifstream::in);
	if (in.fail()) {
		std::cerr << "filed to open model file: " << tmp << std::endl;
		return;
	}
	std::string line;
	while (!in.eof()) {
		std::getline(in, line);
		std::istringstream iss(line);
		char trash;
		if (!line.compare(0, 2, "v ")) {
			iss >> trash;
			Eigen::Vector4f v;
			iss >> v(0) >> v(1) >> v(2);
			v(3) = 1;
			verts_.push_back(v);
		}
		else if (!line.compare(0, 2, "vt")) {
			iss >> trash >> trash;
			Eigen::Vector2f tex;
			iss >> tex(0) >> tex(1);
			texs_.push_back(tex);
		}
		else if (!line.compare(0, 2, "vn")) {
			iss >> trash >> trash;
			Eigen::Vector4f norm;
			iss >> norm(0) >> norm(1) >> norm(2);
			norm(3) = 0;
			norms_.push_back(norm.normalized());
		}
		else if (!line.compare(0, 2, "f ")) {
			int norm, idx, tex;
			iss >> trash;
			std::vector<Point> f;
			while (iss >> idx >> trash >> tex >> trash >> norm) {
				f.push_back(Point(--idx, --tex, --norm));
			}
			faces_.push_back(f);
		}
	}
	std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() <<" t#"<<texs_.size()<<" n#"<<norms_.size() << std::endl;

	tmp = model + "_diffuse.tga";
	if (textures_[Diffuse]->read_tga_file(tmp.c_str())) 
		std::cerr << "loading " << tmp << std::endl;
	tmp = model + "_glow.tga";
	if (textures_[Glow]->read_tga_file(tmp.c_str()))
		std::cerr << "loading " << tmp << std::endl;
	tmp = model + "_nm.tga";
	if (textures_[Nm]->read_tga_file(tmp.c_str()))
		std::cerr << "loading " << tmp << std::endl;
	tmp = model + "_nm_tangent.tga";
	if (textures_[NmTangent]->read_tga_file(tmp.c_str()))
		std::cerr << "loading " << tmp << std::endl;
	tmp = model + "_spec.tga";
	if (textures_[Spec]->read_tga_file(tmp.c_str()))
		std::cerr << "loading " << tmp << std::endl;
	for (int i = 0; i < 5; i++)
		if (textures_[i]->loaded()) textures_[i]->flip_vertically();
}


Model::~Model() {
}

std::vector<Point> Model::getFace(int idx) {
	return faces_[idx];
}

Eigen::Vector4f Model::getVert(int i) {
	return verts_[i];
}

Eigen::Vector4f Model::getVert(int iface, int ipt) {
	return verts_[faces_[iface][ipt].vert];
}

Eigen::Vector2f Model::getTex(int i) {
	return texs_[i];
}

Eigen::Vector2f Model::getTex(int iface, int ipt) {
	return texs_[faces_[iface][ipt].tex];
}

Eigen::Vector4f Model::getNorm(int i) {
	return norms_[i];
}

Eigen::Vector4f Model::getNorm(int iface, int ipt) {
	return norms_[faces_[iface][ipt].norm];
}

size_t Model::nfaces(){
	return faces_.size();
}

size_t Model::nverts() {
	return verts_.size();
}

size_t Model::ntexs() {
	return texs_.size();
}

size_t Model::nnorms() {
	return norms_.size();
}

TGAColor Model::getDiffuse(float u, float v) {
	return textures_[Diffuse]->get(u * textures_[Diffuse]->get_width(), v * textures_[Diffuse]->get_height());
}

TGAColor Model::getDiffuse(Eigen::Vector2f uv) {
	return textures_[Diffuse]->get(uv.x() * textures_[Diffuse]->get_width(), uv.y() * textures_[Diffuse]->get_height());
}

Eigen::Vector4f Model::getNm(Eigen::Vector2f uv) {
	TGAColor tmp = textures_[Nm]->get(uv.x() * textures_[Nm]->get_width(), uv.y() * textures_[Nm]->get_height());
	return Eigen::Vector4f(tmp.r, tmp.g, tmp.b, 0)/255.f*2.f - Eigen::Vector4f(1, 1, 1, 0);
}

TGAColor Model::getSpce(Eigen::Vector2f uv) {
	return textures_[Spec]->get(uv.x() * textures_[Spec]->get_width(), uv.y() * textures_[Spec]->get_height());
}