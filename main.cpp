//TODO 1.implemente z-buffer. 

#include "tgaimage.h"
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
// in world coord
Eigen::Vector4f eye_pos(0, 0, 1000, 1);
Eigen::Vector4f look_dir(0, 0, -1, 0);
Eigen::Vector4f up_dir(0, 1, 0, 0);
const Eigen::Vector4f light_dir(0, 0, -1.0, 0);
// in camera coord
float Far = -2000, Near = -100;
const float fovY = 90.0;
const float aspect = 16. / 9.;


int main(int argc, char** argv) {
	TGAImage image(width, height, TGAImage::RGB);
	Model* model = nullptr;
	//load model
	if (2 == argc) {
		model = new Model(argv[1]);
	}
	else {
		model = new Model("./obj/african_head.obj");
		model->load_texture("./obj/african_head_diffuse.tga");
		//model = new Model("./obj/sllh/sllh.obj");
	}

	Renderer renderer(model);
	renderer.rasterize(image, light_dir, eye_pos, look_dir, up_dir, Near, Far, fovY, aspect);
	image.flip_vertically();
	image.write_tga_file("output.tga");
	return 0;
}

