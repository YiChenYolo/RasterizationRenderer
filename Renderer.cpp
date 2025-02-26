#include "Renderer.h"
#include <iostream>
#include <algorithm>
#include "geometry.h"
using namespace Eigen;
const float PI = 3.1415926;


Renderer::Renderer(TGAImage* _image, Model* _model):
	model_(_model), image_(_image), zbuffer_(nullptr), shadow_buffer_(nullptr) {
	int size = _image->get_height() * _image->get_width();
	zbuffer_ = new float[size];
	shadow_buffer_ = new float[size];
	for (int i = 0; i < size; i++) {
		zbuffer_[i] = -std::numeric_limits<float>::max();
		shadow_buffer_[i] = -std::numeric_limits<float>::max();
	}
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


void Renderer::render() {
	Matrix4f mvp = Geometry::get_mvp_matrix(Eye_pos, Gaze_at, Up_dir, Near, Far, FovY, Aspect);
	Matrix4f vp = Geometry::get_viewport_matrix(Height, Width);
	PhongShader *phong_shader = new PhongShader(vp, mvp, Light_pos, Eye_pos, Light_itensity, model_);
	for (int iface = 0; iface < model_->nfaces(); iface++) {

		for (int i = 0; i < 3; i++) {
			phong_shader->vertex(iface, i);
		}

		Vector2i bbox[2];
		get_bound_box(bbox, phong_shader->sc_, image_->get_width() - 1, image_->get_height() - 1);

		for (int i = bbox[0].x(); i <= bbox[1].x(); i++) {
			for (int j = bbox[0].y(); j <= bbox[1].y(); j++) {
				TGAColor color;
				float z= zbuffer_[i + j*image_->get_width()];
				if (phong_shader->fragment(i, j, color, z)) {
					image_->set(i, j, color);
					zbuffer_[i + image_->get_width() * j] = z;
				}
			}
		}
	}
}