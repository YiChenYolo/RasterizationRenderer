#include "tgaimage.h"
#include <iostream>
#include "Model.h"


const Vec3f light_dir(0, 0, -1.0);
const int height = 800;
const int width = 800;
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
Model *model = nullptr;

void draw_line(Vec2i v0, Vec2i v1, TGAImage& image, const TGAColor& color) {
	bool steep = false;
	int x1 = v0.x, y1 = v0.y, x2 = v1.x, y2 = v1.y;
	//make sure always draws a line with slope lower than 1
	if ((std::abs(x2 - x1)) < std::abs(y2 - y1)) {
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

//void draw_triangle(Vec2i v1, Vec2i v2, Vec2i v3, TGAImage& image, TGAColor color) {
//	if (v1.y > v2.y) std::swap(v1, v2);
//	if (v1.y > v3.y) std::swap(v1, v3);
//	if (v2.y > v3.y) std::swap(v2, v3);
//	float k1 = (float)(v2.x - v1.x) / (v2.y - v1.y);
//	float k2 = (float)(v3.x - v1.x) / (v3.y - v1.y);
//	int half_height = v2.y - v1.y;
//	if (half_height) {
//		for (int j = 0; j <= half_height; j++) {
//			int left_x = v1.x + k1 * j;
//			int right_x = v1.x + k2 * j;
//			if (left_x > right_x) std::swap(left_x, right_x);
//			for (int i = left_x; i < right_x; i++) {
//				image.set(i, v1.y + j, color);
//			}
//		}
//	}
//	
//	k1 = (float)(v2.x - v3.x)/(v2.y - v3.y);
//	half_height = v3.y - v2.y;
//	for (int j = 0; j < half_height; j++) {
//		int left_x = v3.x + k1 * -j;
//		int right_x = v3.x + k2 * -j;
//		if (left_x > right_x) std::swap(left_x, right_x);
//		for (int i = left_x; i < right_x; i++)
//			image.set(i, v3.y - j, color);
//	}
//	
//}

void draw_triangle(Vec2i v[3], TGAImage& image, TGAColor color) {
	Vec2i bbox[2];
	bbox[0].x = std::min(v[0].x, std::min(v[1].x, v[2].x));
	bbox[0].y = std::min(v[0].y, std::min(v[1].y, v[2].y));
	bbox[1].x = std::max(v[0].x, std::max(v[1].x, v[2].x));
	bbox[1].y = std::max(v[0].y, std::max(v[1].y, v[2].y));
	bbox[0].x = std::max(0, bbox[0].x); bbox[0].y = std::max(0, bbox[0].y);
	bbox[1].x = std::min(image.get_width(), bbox[1].x); bbox[1].y = std::min(image.get_height(), bbox[1].y);


	for (int i = bbox[0].x; i <= bbox[1].x; i++) {
		for (int j = bbox[0].y; j <= bbox[1].y; j++) {
			float lambda1 = ((float)(v[1].y - v[2].y) * (i - v[2].x) + (v[2].x - v[1].x) * (j - v[2].y)) /
				((v[1].y - v[2].y) * (v[0].x - v[2].x) + (v[2].x - v[1].x) * (v[0].y - v[2].y));
			float lambda2 = ((float)(v[2].y - v[0].y) * (i - v[2].x) + (v[0].x - v[2].x) * (j - v[2].y)) /
				((v[1].y - v[2].y) * (v[0].x - v[2].x) + (v[2].x - v[1].x) * (v[0].y - v[2].y));
			float lambda3 = 1 - lambda1 - lambda2;
			if (lambda1 < 0 || lambda2 < 0 || lambda3 < 0) continue;
			image.set(i, j, color);
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
		Vec3f world_coords[3];
		Vec2i screen_coords[3];
		for (int j = 0; j < 3; j++) {
			world_coords[j] = model->getVert(f[j]);
			screen_coords[j] = Vec2i((world_coords[j].x + 1.0) / 2 * width, (world_coords[j].y + 1.0) / 2 * height);
		}
		Vec3f n = (world_coords[2] - world_coords[0]) ^ (world_coords[1] - world_coords[0]);
		
		n.normalize();
		float intensity = n * light_dir;
		if(intensity > 0)
			draw_triangle(screen_coords, image, TGAColor(255 * intensity, 255 * intensity, 255 * intensity, 0));

	}
	image.flip_vertically();
	image.write_tga_file("output.tga");
	return 0;
}

