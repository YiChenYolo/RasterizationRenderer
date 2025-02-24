#include "Renderer.h"
#include <iostream>
#include <algorithm>
#include "geometry.h"
using namespace Eigen;
const float PI = 3.1415926;



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
			Eigen::Vector3f bary_coord = Geometry::get_barycentric_coordinate(pts, Eigen::Vector2f(i, j));
			if(bary_coord.x()>=0 && bary_coord.y()>=0 && bary_coord.z()>=0){
				image.set(i, j, color);
			}
		}
	}
}

Eigen::Matrix4f model_trans() {
	Eigen::Matrix4f trans;
	trans <<
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;
	return trans;
}

Eigen::Matrix4f view_trans(Eigen::Vector4f eye_pos, Eigen::Vector4f look_dir, Eigen::Vector4f up_dir) {
	// Normalize input vectors (make sure they are unit vectors)
	look_dir.head<3>().normalize();
	up_dir.head<3>().normalize();

	// Calculate right vector (perpendicular to look and up)
	Eigen::Vector3f right = up_dir.head<3>().cross(-look_dir.head<3>()).normalized();

	// Recalculate up vector to make sure all vectors are orthogonal
	up_dir.head<3>() = -look_dir.head<3>().cross(right).normalized();

	// Construct view matrix (rotation + translation)
	Eigen::Matrix4f trans;
	trans <<
		right.x(), right.y(), right.z(), -eye_pos.head<3>().dot(right),
		up_dir.x(), up_dir.y(), up_dir.z(), -eye_pos.head<3>().dot(up_dir.head<3>()),
		-look_dir.x(), -look_dir.y(), -look_dir.z(), eye_pos.head<3>().dot(look_dir.head<3>()),
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

Matrix4f Renderer::get_mvp_matrix(Eigen::Vector4f eye_pos, Eigen::Vector4f look_dir, Eigen::Vector4f up_dir,
	float near, float far, float fovY, float aspect) {
	//Model Projection
	Matrix4f model = model_trans();
	//view projection
	Matrix4f view = view_trans(eye_pos, look_dir, up_dir);
	//perspective projection
	Matrix4f pers = pers_trans(near, far, fovY, aspect);
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

void get_bound_box(Vector2i* bbox, Vector2f sc[3], int x_max, int y_max) {
	bbox[0].x() = std::min(sc[0].x(), std::min(sc[1].x(), sc[2].x()));
	bbox[0].y() = std::min(sc[0].y(), std::min(sc[1].y(), sc[2].y()));
	bbox[1].x() = std::max(sc[0].x(), std::max(sc[1].x(), sc[2].x()));
	bbox[1].y() = std::max(sc[0].y(), std::max(sc[1].y(), sc[2].y()));
	for (int i = 0; i < 2; i++) {
		bbox[i].x() = std::clamp(bbox[i].x(), 0, x_max);
		bbox[i].y() = std::clamp(bbox[i].y(), 0, y_max);
	}
}

void GouraudShader::vertex_shader(int iface ,int ivert, Matrix4f mvp ) {
	/*calculate the attributes of the vertex*/
	Vector4f coord = model_->getVert(iface, ivert); coord /= coord.w();
	clip_coords_[ivert] = mvp * coord; clip_coords_[ivert] /= clip_coords_[ivert].w();
	uv_[ivert] = model_->getTex(iface, ivert);
	n_[ivert] = model_->getNorm(iface, ivert);
}

bool GouraudShader::fragment_shader(int i, int j,Vector4f light_dir ,TGAColor& color) {
	Vector3f bary_coord = Geometry::get_barycentric_coordinate(sc_, Vector2f(i, j));
	if (bary_coord.x() < 0 || bary_coord.y() < 0 || bary_coord.z() < 0) return false;
	
	float z = Geometry::bary_interpolate(
		bary_coord, std::array<float, 3>{clip_coords_[0].z(), clip_coords_[1].z(), clip_coords_[2].z()});
	if (z < zbuffer_[i + image_->get_width() * j]) { return false; }

	Vector2f tex_coord = Geometry::bary_interpolate(bary_coord, std::array<Vector2f, 3>{uv_[0], uv_[1], uv_[2]});
	tex_coord.x() *= (model_->getTexture()->get_width() - 1); tex_coord.y() *= (model_->getTexture()->get_height() - 1);
	color = model_->getTexture()->get((int)tex_coord.x(), (int)tex_coord.y());
	Vector4f norm = Geometry::bary_interpolate(bary_coord, std::array<Vector4f, 3>{n_[0], n_[1], n_[2]});
	float intensity = norm.dot(-light_dir);
	if (intensity <= 0) return false;
	for (int k = 0; k < 3; k++) color.raw[k] *= intensity;
	
	zbuffer_[i + image_->get_width() * j] = z;
	return true;
}

void GouraudShader::raster_tri(int iface, Matrix4f mvp, Vector4f light_dir) {
	Matrix4f vp = get_viewport_matrix(this->image_->get_height(),image_->get_width());

	for (int i = 0; i < 3; i++) { 
		vertex_shader(iface, i, mvp);
		sc_[i] = (vp * clip_coords_[i]).head<2>();
	}

	Vector2i bbox[2];
	get_bound_box(bbox, sc_, image_->get_width() - 1, image_->get_height() - 1);

	for (int i = bbox[0].x(); i <= bbox[1].x(); i++) {
		for (int j = bbox[0].y(); j <= bbox[1].y(); j++) {
			TGAColor color;
			if(fragment_shader(i, j, light_dir, color))
				image_->set(i, j, color);
		}
	}
}

void Renderer::render(Vector4f light_dir, Matrix4f mvp) {
	//to triangle rasterizer
	for (int i = 0; i < model_->nfaces(); i++) {
		shader_.raster_tri(i, mvp, light_dir);
	}
}