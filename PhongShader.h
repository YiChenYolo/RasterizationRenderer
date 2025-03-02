#ifndef __PHONG_SHADER__
#define __PHONG_SHADER__
#pragma once
#include "Shader.h"
#include "Model.h"
#include <Eigen/Dense>

class PhongShader : public Shader
{
public:
	int p_ = 10;
	Model* model_;
	Eigen::Vector4f world_coords_[3];
	Eigen::Vector4f clip_coords_[3];
	Eigen::Vector4f n_[3];
	Eigen::Vector2f uv_[3];
	Eigen::Vector2f sc_[3];
	Eigen::Matrix4f shadow_mvp_;
	Eigen::Matrix3f TBN_[3];
	float* shadow_zbuffer_;


	PhongShader(Eigen::Matrix4f _vp,Eigen::Matrix4f _mvp, Eigen::Vector4f _light_pos, Eigen::Vector4f _eye_pos, float _light_intensity,
		Model* _model, float* _shadow_zbuffer, Eigen::Matrix4f _shadow_mvp)
		:Shader(_vp, _mvp, _light_pos, _eye_pos, _light_intensity), model_(_model), shadow_zbuffer_(_shadow_zbuffer), shadow_mvp_(_shadow_mvp){}

	void vertex(int iface, int ivert);
	bool fragment(int i, int j, TGAColor& color, float& z);
};

#endif