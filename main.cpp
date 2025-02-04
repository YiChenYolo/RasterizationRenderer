#include "tgaimage.h"

const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);

void draw_line(int x1, int y1, int x2, int y2, TGAImage& image,const TGAColor &color) {
	for (int i = x1; i < x2; i++) {
			
	}
}

int main(int argc, char** argv) {
	TGAImage image(100, 100, TGAImage::RGB);
	image.flip_vertically(); // i want to have the origin at the left bottom corner of the image

	image.write_tga_file("output.tga");
	return 0;
}

