#include "tgaimage.h"
#include <iostream>
#include "Model.h"
#include <cmath>
#include "Renderer.h"
#include <Eigen\Dense>
#include "Geometry.h"
/*
camera coordinate system: right-hand
screen coordinate system: left-hand
world ccordinate system: right-hand
*/

int Height = 900;
int Width = 1600;
float Light_itensity = 5;
// in world coord
Eigen::Vector4f Eye_pos(1, 1, 3, 1);
Eigen::Vector4f Gaze_at(0, 0, 0, 1);
Eigen::Vector4f Up_dir(0, 1, 0, 0);
Eigen::Vector4f Light_pos(-1, 2, 2, 1);
// in camera coord
float Far = -200, Near = -2;
float FovY = 90.0;
float Aspect = 16. / 9.;

TGAImage* Image = new TGAImage(Width, Height, TGAImage::RGB);

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
	
	Renderer renderer(Image ,model);
	renderer.render();
	Image->flip_vertically();
	Image->write_tga_file("output.tga");
	return 0;
}

