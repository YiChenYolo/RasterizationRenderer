﻿#include "tgaimage.h"
#include <iostream>
#include "Model.h"
#include <cmath>
#include "Renderer.h"
#include <Eigen\Dense>
/*
camera coordinate system: right-hand
screen coordinate system: left-hand
world ccordinate system: right-hand
*/

const int height = 900;
const int width = 1600;
float light_itensity = 5;
// in world coord
Eigen::Vector4f eye_pos(1, 1, 3, 1);
Eigen::Vector4f look_dir(-0.2, -0.2, -1, 0);
Eigen::Vector4f up_dir(0, 1, 0, 0);
Eigen::Vector4f light_pos(1, 2, 2, 1);
// in camera coord
float Far = -200, Near = -2;
const float fovY = 90.0;
const float aspect = 16. / 9.;

TGAImage* image = new TGAImage(width, height, TGAImage::RGB);
float* zbuffer = new float[height * width];

int main(int argc, char** argv) {
	Model* model = nullptr;
	Material m(1, 1, 0.1);// d , s , a
	//load model
	if (2 == argc) {
		model = new Model(argv[1], m);
	}
	else {
		model = new Model("diablo3_pose", m);
	}
	
	Eigen::Matrix4f mvp = Renderer::get_mvp_matrix(eye_pos, look_dir, up_dir, Near, Far, fovY, aspect);
	Renderer renderer(image ,model,zbuffer, mvp, light_pos, eye_pos, light_itensity);
	renderer.render();
	image->flip_vertically();
	image->write_tga_file("output.tga");
	return 0;
}

