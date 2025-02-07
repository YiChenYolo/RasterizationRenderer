#include "tgaimage.h"
#include <iostream>
#include "Model.h"


const int height = 800;
const int width = 800;
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
Model *model = nullptr;

void draw_line(int x1, int y1, int x2, int y2, TGAImage& image, const TGAColor& color) {
	bool steep = false;
	//make sure always draws a line with slope lower than 1
	if ((x2 - x1) < std::abs(y2 - y1)) {
		steep = true;
		std::swap(x1, y1); std::swap(x2, y2);
	}
	//assure x1 is at the left of x2
	if (x1 > x2) {
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	int dx = x2 - x1;
	int dy = std::abs(y2 - y1);
	int derr = dy << 1;
	int err = -dx;
	int y = y1, x = x1;
	int dir = y2 > y1 ? 1 : -1;

	if (steep){
		while (true) {
			if (x == x2) break;
			image.set(y, x, color);
			x++;
			err += derr;
			//std::cout << err << std::endl;
			if (err >= 0) {
				err = err - (dx << 1);
				y += dir;
			}
		}
	}
	else {
		while (true) {
			if (x == x2) break;
			image.set(x, y, color);
			x++;
			err += derr;
			//std::cout << err << std::endl;
			if (err >= 0) {
				err = err - (dx << 1);
				y += dir;
			}
		}
	}
}



int main(int argc, char** argv) {
	TGAImage image(width, height, TGAImage::RGB);
	if (2 == argc) {
		model = new Model(argv[1]);
	}
	else {
		model = new Model("./obj/african_head.obj");
	}

	for (int i = 0; i < model->nfaces(); i++) {
		std::vector<int> f = model->getFace(i);
		for (int j = 0; j < 3; j++) {
			Vec3f v0 = model->getVert(f[j]);
			Vec3f v1 = model->getVert(f[(j + 1) % 3]);
			int x0 = (v0.x + 1.) * width / 2.;
			int y0 = (v0.y + 1.) * height / 2.;
			int x1 = (v1.x + 1.) * width / 2.;
			int y1 = (v1.y + 1.) * height / 2.;
			draw_line(x0, y0, x1, y1, image, white);
		}
	}
	image.flip_vertically();
	image.write_tga_file("output.tga");
	return 0;
}

