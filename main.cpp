#include "tgaimage.h"
#include <iostream>
#include <random>

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

void draw_line(int x1, int y1, int x2, int y2, TGAImage& image,const TGAColor &color) {
	bool steep = false;
	if ((x2 - x1) < std::abs(y2 - y1)) {
		steep = true;
		std::swap(x1, y1); std::swap(x2, y2);
	}
	if (x1 > x2) {
		std::swap(x1, x2);
		std::swap(y1, y2);
	}

	float k = std::abs((float)(y2 - y1) / (x2 - x1));
	float err = -0.5;
	int y = y1, x = x1;
	int dy = y2 > y1 ? 1 : -1;
	while (true) {
		if (x == x2) break;
		if (steep) image.set(y, x, color);
		else image.set(x, y, color);
		x++;
		err += k;
		//std::cout << err << std::endl;
		if (err >= 0) {
			err--;
			y += dy;
		}
	}
}

void draw_line1(int x1, int y1, int x2, int y2, TGAImage& image, const TGAColor& color) {
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
	int err = - dx;
	int y = y1, x = x1;
	int dir = y2 > y1 ? 1 : -1;
	while (true) {
		if (x == x2) break;
		if (steep) image.set(y, x, color);
		else image.set(x, y, color);
		x++;
		err += derr;
		//std::cout << err << std::endl;
		if (err >= 0) {
			err = err - (dx<<1);
			y += dir;
		}
	}
}

void effciency_test(TGAImage &image) {
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 100);
	for (int i = 0; i < 10000000; i++) {
		int x1 = dis(gen);
		int y1 = dis(gen);
		int x2 = dis(gen);
		int y2 = dis(gen);
		//std::cout << "drawing (" << x1 << ", " << y1 << ") ->(" << x2 << ", " << y2 << ")" << std::endl;
		draw_line(x1, y1, x2, y2, image, white);
	}
}


int main(int argc, char** argv) {
	TGAImage image(100, 100, TGAImage::RGB);
	draw_line1(77, 81, 74, 98, image, white);
	//effciency_test(image);
	image.flip_vertically();
	image.write_tga_file("output.tga");
	return 0;
}

