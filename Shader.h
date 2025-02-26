#pragma once
#ifndef __SHADER__
#define __SHADER__

#include <Eigen/Dense>
#include "tgaimage.h"

class Shader {
public:
    Shader(Eigen::Matrix4f vp,Eigen::Matrix4f mvp, Eigen::Vector4f light_pos, Eigen::Vector4f eye_pos, float light_intensity)
        :vp_(vp), mvp_(mvp), light_pos_(light_pos), eye_pos_(eye_pos), light_intensity_(light_intensity) { }

    virtual ~Shader() = default;

    virtual void vertex(int iface, int ivert) = 0;

    virtual bool fragment(int i, int j, TGAColor& color, float& z) = 0;

    Eigen::Matrix4f vp_;
    Eigen::Matrix4f mvp_;
    Eigen::Vector4f light_pos_;
    Eigen::Vector4f eye_pos_;
    float light_intensity_;

protected:

};

#endif 