#include "Model.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <Eigen/Dense>

Model::Model(const char* filename):verts_(), texs_(), norms_(), faces_(), texture_(nullptr){
	std::ifstream in;
	in.open(filename, std::ifstream::in);
	if (in.fail()) {
		std::cerr << "filed to open model file: " << filename << std::endl;
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

}


Model::~Model() {
}

void Model::load_texture(const char* model_path) {
	texture_ = new TGAImage();
	texture_->read_tga_file(model_path);
}

std::vector<Point> Model::getFace(int idx) {
	return faces_[idx];
}

Eigen::Vector4f Model::getVert(int i) {
	return verts_[i];
}

Eigen::Vector2f Model::getTex(int i) {
	return texs_[i];
}

Eigen::Vector4f Model::getNorm(int i) {
	return norms_[i];
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

bool Model::hasTexture() {
	return texture_ != nullptr;
}

TGAImage* Model::getTexture() {
	return texture_;
}