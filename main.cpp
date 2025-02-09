#include "tgaimage.h"
#include <iostream>
#include "Model.h"
#include <cmath>
/*
camera coordinate system: right-hand
screen coordinate system: left-hand
world ccordinate system: right-hand
*/

const float PI = 3.1415926;
Eigen::Vector4f eye_pos(-2.5, 0, 50, 1);
const Eigen::Vector4f light_dir(0, 0, -1.0, 0);
const int height = 800;
const int width = 1600;
const TGAColor white = TGAColor(255, 255, 255, 255);
const TGAColor red   = TGAColor(255, 0,   0,   255);
Model *model = nullptr;
float* zbuffer = new float[height * width];
int Far = -50, Near = -10;
const float fovY = 60.0;
const float aspect = 16. / 9.;


void draw_line(Eigen::Vector2i v0, Eigen::Vector2i v1, TGAImage& image, const TGAColor& color) {
	bool steep = false;
	int x1 = v0.x(), y1 = v0.y(), x2 = v1.x(), y2 = v1.y();
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

Eigen::Vector3f get_barycentric_coordinate(Eigen::Vector2f pts[3], Eigen::Vector2f P) {
	Eigen::Vector3f bary_coord;
	bary_coord(0) = ((pts[1].y() - pts[2].y()) * (P.x() - pts[2].x()) + (pts[2].x() - pts[1].x()) * (P.y() - pts[2].y())) /
		((pts[1].y() - pts[2].y()) * (pts[0].x() - pts[2].x()) + (pts[2].x() - pts[1].x()) * (pts[0].y() - pts[2].y()));
	bary_coord(1) = ((pts[2].y() - pts[0].y()) * (P.x() - pts[2].x()) + (pts[0].x() - pts[2].x()) * (P.y() - pts[2].y())) /
		((pts[1].y() - pts[2].y()) * (pts[0].x() - pts[2].x()) + (pts[2].x() - pts[1].x()) * (pts[0].y() - pts[2].y()));
	bary_coord(2) = 1 - bary_coord(0) - bary_coord(1);
	return bary_coord;
}

void draw_triangle(Eigen::Vector4f homo_pts[3], TGAImage& image, TGAColor color) {
	Eigen::Vector2i bbox[2];
	Eigen::Vector3f pts[3];
	for (int i = 0; i < 3; i++) {
		pts[i] = homo_pts[i].hnormalized();
		pts[i].x() *= width;
		pts[i].y() *= height;
	}
	bbox[0].x() = std::min(pts[0].x(), std::min(pts[1].x(), pts[2].x()));
	bbox[0].y() = std::min(pts[0].y(), std::min(pts[1].y(), pts[2].y()));
	bbox[1].x() = std::max(pts[0].x(), std::max(pts[1].x(), pts[2].x()));
	bbox[1].y() = std::max(pts[0].y(), std::max(pts[1].y(), pts[2].y()));
	bbox[0].x() = std::max(0, bbox[0].x()); bbox[0].y() = std::max(0, bbox[0].y());
	bbox[1].x() = std::min(image.get_width(), bbox[1].x()); bbox[1].y() = std::min(image.get_height(), bbox[1].y());

	Eigen::Vector3i edge[3];
	edge[0] = Eigen::Vector3i(pts[1].x() - pts[0].x(), pts[1].y() - pts[0].y(), 0);
	edge[1] = Eigen::Vector3i(pts[2].x() - pts[1].x(), pts[2].y() - pts[1].y(), 0);
	edge[2] = Eigen::Vector3i(pts[0].x() - pts[2].x(), pts[0].y() - pts[2].y(), 0);
	for (int i = bbox[0].x(); i <= bbox[1].x(); i++) {
		for (int j = bbox[0].y(); j <= bbox[1].y(); j++) {
			bool has_pos = false;
			bool has_neg = false;
			for (int k = 0; k < 3; k++) {
				Eigen::Vector3i v(i - pts[k].x(), j - pts[k].y(), 0);
				if ((edge[k] .cross(v)).z() >= 0) has_pos = true;
				else has_neg = true;
			}
			if (!(has_pos && has_neg)) {
				Eigen::Vector2f temp_pts[3];
				for (int i = 0; i < 3; i++) {
					temp_pts[i] = pts[i].head<2>();
				}
				Eigen::Vector3f bary_coord = get_barycentric_coordinate(temp_pts, Eigen::Vector2f(i, j));
				float z = pts[0].z() * bary_coord.x() + pts[1].z() * bary_coord.y() + pts[2].z() * bary_coord.z();
				if (z > zbuffer[i + j * width]) {
					zbuffer[i + j * width] = z;
					image.set(i, j, color);
				}
			}
		}
	}
}

Eigen::Matrix4f get_mvp_matrix() {
	//Model Projection
	Eigen::Matrix4f m_matrix;
	m_matrix <<
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;
	//view projection
	Eigen::Matrix4f v_matrix;
	v_matrix << 1, 0, 0, -eye_pos.x(),
		0, 1, 0, -eye_pos.y(),
		0, 0, 1, -eye_pos.z(),
		0, 0, 0, 0;

	//perspective projection
	Eigen::Matrix4f o_matrix;
	float d = std::abs(eye_pos.z() - Near);
	float t = std::tan(fovY / 180. * PI / 2), b = -t;
	float r = t * aspect, l = -r;
	float f = Far, n = Near;
	Eigen::Matrix4f ortho_proj;
	ortho_proj <<
		2 / (r - l), 0, 0, 0,
		0, 2 / (t - b), 0, 0,
		0, 0, 2 / (n - f), 0,
		0, 0, 0, 1;

	Eigen::Matrix4f p2o_matrix;
	p2o_matrix <<
		n, 0, 0, 0,
		0, n, 0, 0,
		0, 0, n + f, -n * f,
		0, 0, 1, 0;
	Eigen::Matrix4f p_matrix;
	p_matrix = ortho_proj * p2o_matrix;
	return p_matrix * v_matrix * m_matrix;
}

void render(TGAImage& image) {
	Eigen::Matrix4f mvp = get_mvp_matrix();
	for (int i = 0; i < width * height; i++) {
		zbuffer[i] = -std::numeric_limits<float>::max();
	}
	for (int i = 0; i < model->nfaces(); i++) {
		std::vector<int> f = model->getFace(i);
		Eigen::Vector4f world_coords[3];
		Eigen::Vector4f coords[3];
		for (int j = 0; j < 3; j++) {
			world_coords[j] = model->getVert(f[j]);
			coords[j] = mvp * world_coords[j];
			//std::cout << coords[j] << std::endl << std::endl;
		}
		Eigen::Vector3f n = (world_coords[2].head<3>() - world_coords[0].head<3>())
			.cross(world_coords[1].head<3>() - world_coords[0].head<3>());

		n = n.normalized();
		float intensity = n.dot(light_dir.head<3>());
		if (intensity > 0)
			draw_triangle(coords, image, TGAColor(255 * intensity, 255 * intensity, 255 * intensity, 255));
	}
}



int main(int argc, char** argv) {
	TGAImage image(width, height, TGAImage::RGB);
	//load model
	if (2 == argc) {
		model = new Model(argv[1]);
	}
	else {
		model = new Model("./obj/sllh/sllh.obj");
	}

	render(image);
	image.flip_vertically();
	image.write_tga_file("output.tga");
	return 0;
}

