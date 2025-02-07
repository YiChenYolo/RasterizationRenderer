#include "Model.h"
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

Model::Model(const char* filename):verts_(),faces_()  {
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
			Vec3f v;
			iss >> v.raw[0] >> v.raw[1] >> v.raw[2];
			verts_.push_back(v);
		}
		else if (!line.compare(0, 2, "f ")) {
			int itrash, idx;
			iss >> trash;
			std::vector<int> f;
			while (iss >> idx >> trash >> itrash >> trash >> itrash) {
				f.push_back(--idx);
			}
			faces_.push_back(f);
		}
	}
	std::cerr << "# v# " << verts_.size() << " f# " << faces_.size() << std::endl;

}

Model::~Model() {
}

std::vector<int> Model::getFace(int idx) {
	return faces_[idx];
}

Vec3f Model::getVert(int i) {
	return verts_[i];
}

size_t Model::nfaces(){
	return faces_.size();
}

size_t Model::nverts() {
	return verts_.size();
}