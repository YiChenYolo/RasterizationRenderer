#ifndef __RENDERER__
#define __RENDERER__ 
#pragma once
#include "tgaimage.h"
#include <Eigen/Dense>
#include "Model.h"

struct Shader {
	Model* model_;
	TGAImage* image_;
	float* zbuffer_;
	Eigen::Matrix4f mvp_;
	Eigen::Vector4f light_pos_;
	float light_intensity_;
	Eigen::Vector4f eye_pos_;
	int p_ = 10;
	Eigen::Vector4f world_coords_[3];
	Eigen::Vector4f clip_coords_[3];
	Eigen::Vector4f n_[3];
	Eigen::Vector2f uv_[3];
	Eigen::Vector2f sc_[3];

	Shader(Model* _model, float* _zbuffer, TGAImage* _image, Eigen::Matrix4f _mvp, Eigen::Vector4f _light_pos, Eigen::Vector4f _eye_pos, float _light_intensity)
		:model_(_model), zbuffer_(_zbuffer), image_(_image), mvp_(_mvp), light_pos_(_light_pos), eye_pos_(_eye_pos), light_intensity_(_light_intensity){}
	void raster_tri(int iface);
	void vertex_shader(int iface, int ivert);
	bool fragment_shader(int i, int j ,TGAColor &color, float &z);
};

class Renderer
{
private:
	Shader shader_;
	Model* model_;
public:
	Renderer(TGAImage *_image, Model *_model ,float* _zbuffer, Eigen::Matrix4f _mvp, Eigen::Vector4f _light_pos, Eigen::Vector4f _eye_pos, float _light_intensity) :
		model_(_model),shader_(_model, _zbuffer, _image, _mvp, _light_pos, _eye_pos, _light_intensity) {}
	void render();

	static Eigen::Matrix4f get_mvp_matrix(Eigen::Vector4f eye_pos, Eigen::Vector4f look_dir, Eigen::Vector4f up_dir,
		float near, float far, float fovY, float aspect);
private:

};

#endif