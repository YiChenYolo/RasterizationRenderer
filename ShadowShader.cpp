#include "ShadowShader.h"
#include "Geometry.h"
#include <iostream>
using namespace Eigen;

void ShadowShader::vertex(int iface, int ivert) {
	/*calculate the attributes of the vertex*/
	world_coords_[ivert] = model_->getVert(iface, ivert); world_coords_[ivert] /= world_coords_[ivert].w();
	clip_coords_[ivert] = mvp_ * world_coords_[ivert]; clip_coords_[ivert] /= clip_coords_[ivert].w();
	sc_[ivert] = (vp_ * clip_coords_[ivert]).head<2>();
}


bool ShadowShader::fragment(int i, int j, TGAColor& color, float& z) {
	//get barycentric coordinate
	Vector3f bary_coord = Geometry::get_barycentric_coordinate(sc_, Vector2f(i, j));
	if (bary_coord.x() < 0 || bary_coord.y() < 0 || bary_coord.z() < 0) return false;
	//zbuffer check
	Vector4f world_coord = Geometry::bary_interpolate(
		bary_coord, std::array<Vector4f, 3>{world_coords_[0], world_coords_[1], world_coords_[2]});
	float tmp_z = (eye_pos_.hnormalized() - world_coord.hnormalized()).norm();
	if (tmp_z > z) { return false; }
	z = tmp_z;
	return true;
}