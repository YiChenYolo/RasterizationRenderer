#ifndef __RENDERER__
#define __RENDERER__ 
#pragma once
#include "tgaimage.h"
#include <Eigen/Dense>
#include "Model.h"

struct GouraudShader {
	Model* model_;
	TGAImage* image_;
	float* zbuffer_;

	Eigen::Vector4f clip_coords_[3];
	Eigen::Vector4f n_[3];
	Eigen::Vector2f uv_[3];
	Eigen::Vector2f sc_[3];

	GouraudShader(Model* _model, float* _zbuffer, TGAImage* _image) :model_(_model), zbuffer_(_zbuffer), image_(_image){}
	void raster_tri(int iface, Eigen::Matrix4f mvp, Eigen::Vector4f light_dir);
	void vertex_shader(int iface, int ivert, Eigen::Matrix4f mvp);
	bool fragment_shader(int i, int j,Eigen::Vector4f light_dir, TGAColor &color);
};

class Renderer
{
private:
	GouraudShader shader_;
	Model* model_;
public:
	Renderer(TGAImage *_image, Model *_model ,float* _zbuffer) :model_(_model),shader_(_model, _zbuffer, _image) {}
	void render(Eigen::Vector4f light_dir, Eigen::Matrix4f mvp);

	static Eigen::Matrix4f get_mvp_matrix(Eigen::Vector4f eye_pos, Eigen::Vector4f look_dir, Eigen::Vector4f up_dir,
		float near, float far, float fovY, float aspect);
private:

};

#endif