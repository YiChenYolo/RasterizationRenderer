#ifndef __SHADOW_SHADER__
#define __SHADOW_SHADER__

#include "Shader.h"
#include <Eigen\Dense>
#include "Model.h"
class ShadowShader : public Shader
{
public:
	Eigen::Vector4f world_coords_[3];
	Eigen::Vector4f clip_coords_[3];
	Eigen::Vector2f sc_[3];
	Model* model_;

	ShadowShader(Model *_model, Eigen::Matrix4f _vp, Eigen::Matrix4f _mvp, Eigen::Vector4f _light_pos, float _light_intensity)
		:Shader(_vp, _mvp, _light_pos, _light_pos, _light_intensity), model_(_model) {
	}

    void vertex(int iface, int ivert);

    bool fragment(int i, int j, TGAColor& color, float& z);
};

#endif
