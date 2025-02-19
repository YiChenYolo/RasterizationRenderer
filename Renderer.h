#ifndef __RENDERER__
#define __RENDERER__ 
#pragma once
#include "tgaimage.h"
#include <Eigen/Dense>
#include "Model.h"

class Renderer
{
private:
	Model *model;
	float* zbuffer;
public:
	Renderer(Model *_model) :model(_model), zbuffer(nullptr){}
	void render(TGAImage& image, Eigen::Vector4f light_dir, Eigen::Vector4f eye_pos, Eigen::Vector4f look_dir, Eigen::Vector4f up_dir,
		float near, float far, float fovY, float aspect);
};

#endif