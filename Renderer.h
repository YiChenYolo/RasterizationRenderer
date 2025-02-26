#ifndef __RENDERER__
#define __RENDERER__ 
#pragma once
#include "tgaimage.h"
#include <Eigen/Dense>
#include "Model.h"
#include "PhongShader.h"

extern Eigen::Vector4f Eye_pos, Gaze_at, Up_dir, Light_pos;
extern int Height, Width;
extern float Light_itensity, Far, Near, FovY, Aspect;

class Renderer{
private:
	Model* model_;
	TGAImage* image_;
	float* zbuffer_;
	float* shadow_buffer_;
public:
	Renderer(TGAImage* _image, Model* _model);
	void render();
private:
};

#endif