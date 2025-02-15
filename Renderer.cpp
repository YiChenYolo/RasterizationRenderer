#include "Renderer.h"
#include <iostream>
#include <algorithm>

const float PI = 3.1415926;

Eigen::Vector3f get_barycentric_coordinate(Eigen::Vector2f pts[3], Eigen::Vector2f P) {
	Eigen::Vector3f bary_coord;
	bary_coord(0) = ((pts[1].y() - pts[2].y()) * (P.x() - pts[2].x()) + (pts[2].x() - pts[1].x()) * (P.y() - pts[2].y())) /
		((pts[1].y() - pts[2].y()) * (pts[0].x() - pts[2].x()) + (pts[2].x() - pts[1].x()) * (pts[0].y() - pts[2].y()));
	bary_coord(1) = ((pts[2].y() - pts[0].y()) * (P.x() - pts[2].x()) + (pts[0].x() - pts[2].x()) * (P.y() - pts[2].y())) /
		((pts[1].y() - pts[2].y()) * (pts[0].x() - pts[2].x()) + (pts[2].x() - pts[1].x()) * (pts[0].y() - pts[2].y()));
	bary_coord(2) = 1 - bary_coord(0) - bary_coord(1);
	return bary_coord;
}


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

	if (steep) {
		while (true) {
			if (x == x2) break;
			image.set(y, x, color);
			x++;
			err += derr;
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
			if (err >= 0) {
				err = err - (dx << 1);
				y += dir;
			}
		}
	}
}

void draw_triangle(Eigen::Vector2f pts[3], TGAImage& image, TGAColor color) {
	Eigen::Vector2i bbox[2];
	bbox[0].x() = std::min(pts[0].x(), std::min(pts[1].x(), pts[2].x()));
	bbox[0].y() = std::min(pts[0].y(), std::min(pts[1].y(), pts[2].y()));
	bbox[1].x() = std::max(pts[0].x(), std::max(pts[1].x(), pts[2].x()));
	bbox[1].y() = std::max(pts[0].y(), std::max(pts[1].y(), pts[2].y()));
	for (int i = 0; i < 2; i++) {
		bbox[i].x() = std::clamp(bbox[i].x(), 0, image.get_width() - 1);
		bbox[i].y() = std::clamp(bbox[i].y(), 0, image.get_height() - 1);
	}
	for (int i = bbox[0].x(); i <= bbox[1].x(); i++) {
		for (int j = bbox[0].y(); j <= bbox[1].y(); j++) {
			//TODO maybe bary_coord shouldn't compute at here, cuz it seems will change after perspective transformation
			Eigen::Vector3f bary_coord = get_barycentric_coordinate(pts, Eigen::Vector2f(i, j));
			if(bary_coord.x()>=0 && bary_coord.y()>=0 && bary_coord.z()>=0){
				image.set(i, j, color);
			}
		}
	}
}

Eigen::Matrix4f model_trans() {
	Eigen::Matrix4f trans;
	trans <<
		10, 0, 0, 0,
		0, 10, 0, 0,
		0, 0, 10, 0,
		0, 0, 0, 1;
	return trans;
}

Eigen::Matrix4f view_trans(Eigen::Vector4f eye_pos, Eigen::Vector4f look_dir, Eigen::Vector4f up_dir) {
	Eigen::Matrix4f trans;
	look_dir.head<3>().normalize();
	up_dir.head<3>().normalize();
	Eigen::Vector3f x = look_dir.head<3>().cross(up_dir.head<3>()).normalized();
	trans<<
		x.x(), x.y(), x.z(), -eye_pos.x(),
		up_dir.x(), up_dir.y(), up_dir.z(), -eye_pos.y(),
		look_dir.x(), look_dir.y(), look_dir.z(), -eye_pos.z(),
		0, 0, 0, 1;
	return trans;
}

Eigen::Matrix4f pers_trans(float near, float far, float fovY, float aspect) {
	float t = std::tan(fovY / 180. * PI / 2), b = -t;
	float r = t * aspect, l = -r;
	float f = far, n = near;
	Eigen::Matrix4f ortho_proj;
	ortho_proj <<
		2 / (r - l), 0, 0, 0,
		0, 2 / (t - b), 0, 0,
		0, 0, 2 / (n - f), -(n+f)/(n-f),
		0, 0, 0, 1;
	Eigen::Matrix4f squish_matrix;
	squish_matrix <<
		n, 0, 0, 0,
		0, n, 0, 0,
		0, 0, n + f, -n * f,
		0, 0, 1, 0;
	return ortho_proj * squish_matrix;
}

Eigen::Matrix4f get_mvp_matrix(Eigen::Vector4f eye_pos, Eigen::Vector4f look_dir, Eigen::Vector4f up_dir,
	float near, float far, float fovY, float aspect) {
	//Model Projection
	Eigen::Matrix4f model = model_trans();
	//view projection
	Eigen::Matrix4f view = view_trans(eye_pos, look_dir, up_dir);
	//perspective projection
	Eigen::Matrix4f pers = pers_trans(near, far, fovY, aspect);
	//Eigen::Matrix4f pers = Eigen::Matrix4f::Identity();
	return pers * view * model;
}

Eigen::Matrix4f get_viewport_matrix(int height, int width) {
	Eigen::Matrix4f trans;
	trans <<
		width / 2, 0, 0, width / 2,
		0, height / 2, 0, height / 2,
		0, 0, 1, 0,
		0, 0, 0, 1;
	return trans;
}

void Renderer::rasterize(TGAImage& image,Eigen::Vector4f light_dir ,Eigen::Vector4f eye_pos, Eigen::Vector4f look_dir, Eigen::Vector4f up_dir,
	//initialize
	float near, float far, float fovY, float aspect) {
	Eigen::Matrix4f mvp = get_mvp_matrix(eye_pos, look_dir, up_dir, near, far, fovY, aspect);
	int width = image.get_width();
	int height = image.get_height();
	zbuffer = new float[width * height];
	for (int i = 0; i < width * height; i++) {
		zbuffer[i] = -std::numeric_limits<float>::max();
	}
	Eigen::Matrix4f viewport = get_viewport_matrix(height, width);

	//rasterize
	//float max = -100000, min = 1000000;
	for (int i = 0; i < model->nfaces(); i++) {
		std::vector<Point> face_pts = model->getFace(i);
		Eigen::Vector4f world_coords[3];
		Eigen::Vector4f coords[3];
		//do mvp transformation
		for (int j = 0; j < 3; j++) {
			world_coords[j] = model->getVert(face_pts[j].vert);
			coords[j] = mvp * world_coords[j];
		}
		//calculate face norm
		Eigen::Vector3f n = (world_coords[2].head<3>() - world_coords[0].head<3>())
			.cross(world_coords[1].head<3>() - world_coords[0].head<3>());
		n.normalize();

		//render
		float intensity = n.dot(light_dir.head<3>());
		if (intensity > 0) {
			//viewport transform
			TGAColor colors[3];
			for (int j = 0; j < 3; j++) colors[j] = TGAColor(255 * intensity, 255 * intensity, 255 * intensity, 255);
			// get texture color
			if (model->hasTexture()) {
				TGAImage texture = *model->getTexture();
				texture.flip_vertically();
				for (int j = 0; j < 3; j++) {
					Eigen::Vector2f uv;
					uv = model->getTex(face_pts[j].tex);
					colors[j] = texture.get(uv.x() * texture.get_width(), uv.y() * texture.get_height());
				}
			}
			Eigen::Vector2f screen_coords[3];
			for (int j = 0; j < 3; j++){
				screen_coords[j] = (viewport * (coords[j] / coords[j].w())).head<2>();
			}
			TGAColor color(0, 0, 0, 255);
			for (int j = 0; j < 3; j++) {
				color.r += colors[j].r / 3;
				color.g += colors[j].g / 3;
				color.b += colors[j].b / 3;
			}
			draw_triangle(screen_coords, image, color);
		}
	}
	//std::cout << min << " " << max << std::endl;
}