#include "PhongShader.h"
#include "Geometry.h"
#include <iostream>
using namespace Eigen;

extern int Width, Height;


void PhongShader::vertex(int iface, int ivert) {
	/*calculate the attributes of the vertex*/
	world_coords_[ivert] = model_->getVert(iface, ivert); world_coords_[ivert] /= world_coords_[ivert].w();
	clip_coords_[ivert] = mvp_ * world_coords_[ivert]; clip_coords_[ivert] /= clip_coords_[ivert].w();
	uv_[ivert] = model_->getTex(iface, ivert);
	n_[ivert] = model_->getNorm(iface, ivert);
	sc_[ivert] = (vp_ * clip_coords_[ivert]).head<2>();
	TBN_[ivert] = Matrix3f::Zero();
	int i;
	int vert = model_->getFace(iface).verts_[ivert];
	for (i = 0; i < model_->nAdjFaces(vert); i++) {
		TBN_[ivert] += Geometry::get_TBN(model_, model_->getAdjFace(vert, i));
	}
	TBN_[ivert] /= i;
}


bool PhongShader::fragment(int i, int j, TGAColor& color, float& z) {
	//get barycentric coordinate
	Vector3f bary_coord = Geometry::get_barycentric_coordinate(sc_, Vector2f(i, j));
	if (bary_coord.x() < 0 || bary_coord.y() < 0 || bary_coord.z() < 0) return false;
	//zbuffer check
	float tmp_z = Geometry::bary_interpolate(
		bary_coord, std::array<float, 3>{clip_coords_[0].z(), clip_coords_[1].z(), clip_coords_[2].z()});
	if (tmp_z < z) { return false; }
	z = tmp_z;
	//interpolate uv and normal vector
	Vector2f tex_coord = Geometry::bary_interpolate(bary_coord, std::array<Vector2f, 3>{uv_[0], uv_[1], uv_[2]});
	Vector4f world_coord = Geometry::bary_interpolate(bary_coord,
		std::array<Vector4f, 3>{world_coords_[0], world_coords_[1], world_coords_[2]});
	world_coord = world_coord / world_coord.w();
	Vector4f norm;
	if (model_->hasNmTangent()) {
		Matrix3f TBN = Geometry::bary_interpolate(bary_coord, std::array<Matrix3f, 3>{TBN_[0], TBN_[1], TBN_[2]});
		norm = model_->getTanNorm(tex_coord);
		norm = (TBN.transpose() * norm.head<3>()).homogeneous();
		norm.w() = 0;
		norm.normalize();
	}
	else if (model_->hasNm()) {
		norm = model_->getNm(tex_coord);
	}
	else {
		norm = Geometry::bary_interpolate(bary_coord, std::array<Vector4f, 3>{n_[0], n_[1], n_[2]});
	}

	//shading
	world_coord /= world_coord.w();
	light_pos_ /= light_pos_.w();
	eye_pos_ /= light_pos_.w();
	Vector4f vert2light = (light_pos_ - world_coord);
	Vector4f vert2eye = (eye_pos_ - world_coord);
	float r = vert2light.norm();

	float diffuse_intensity=0, spec_intensity=0;
	Vector4f tmp = shadow_mvp_ * world_coord; tmp /= tmp.w();
	Vector2f shadow_coord = (vp_*tmp).head<2>();
	//std::cout << vert2light.norm() - shadow_zbuffer_[(int)shadow_coord.x() + (int)shadow_coord.y() * Width] << std::endl;
	if (std::abs(r - shadow_zbuffer_[(int)shadow_coord.x() + (int)shadow_coord.y() * Width]) < 0.1) {
		diffuse_intensity = model_->getKd() * (light_intensity_ / std::pow(r, 2)) * std::max(norm.dot(vert2light), 0.f);
		Vector4f h = (vert2light + vert2eye).normalized();
		spec_intensity = model_->getKs() * (light_intensity_ / std::pow(r, 2)) * std::pow(std::max(0.f, norm.dot(h)), p_);
	}
	float ambient_intensity = model_->getKa() * light_intensity_;
	TGAColor diffuse = model_->getDiffuse(tex_coord);
	TGAColor spec(0, 0, 0, 255);
	if (model_->hasSpec())
		spec = model_->getSpce(tex_coord);
	for (int k = 0; k < 3; k++) {
		int tmp = (ambient_intensity + diffuse_intensity) * diffuse.raw[k] + spec_intensity * spec.raw[k];
		color.raw[k] = (unsigned char)std::clamp(tmp, 0, 255);
	}

	return true;
}
