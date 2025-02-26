#include "PhongShader.h"
#include "Geometry.h"
using namespace Eigen;

void PhongShader::vertex(int iface, int ivert) {
	/*calculate the attributes of the vertex*/
	world_coords_[ivert] = model_->getVert(iface, ivert); world_coords_[ivert] /= world_coords_[ivert].w();
	clip_coords_[ivert] = mvp_ * world_coords_[ivert]; clip_coords_[ivert] /= clip_coords_[ivert].w();
	uv_[ivert] = model_->getTex(iface, ivert);
	n_[ivert] = model_->getNorm(iface, ivert);
	sc_[ivert] = (vp_ * clip_coords_[ivert]).head<2>();
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

	Vector4f norm;
	if (model_->hasNm()) {
		norm = model_->getNm(tex_coord);
	}
	else {
		norm = Geometry::bary_interpolate(bary_coord, std::array<Vector4f, 3>{n_[0], n_[1], n_[2]});
	}

	//shading
	Vector4f vert2light = (light_pos_ - world_coord).normalized();
	Vector4f vert2eye = (eye_pos_ - world_coord).normalized();
	float r = (light_pos_ - world_coord).norm();
	float diffuse_intensity = model_->getKd() * (light_intensity_ / std::pow(r, 2)) * std::max(norm.dot(vert2light), 0.f);
	float ambient_intensity = model_->getKa() * light_intensity_;
	Vector4f h = (vert2light + vert2eye).normalized();
	float spec_intensity = model_->getKs() * (light_intensity_ / std::pow(r, 2)) * std::pow(std::max(0.f, norm.dot(h)), p_);
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
