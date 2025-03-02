#include "geometry.h"
using namespace Eigen;
const float PI = 3.1415926;

Matrix3f Geometry::get_TBN(Model* model, int iface) {
	Vector2f uv[3];
	Vector4f world_coords[3];
	for (int i = 0; i < 3; i++) {
		uv[i] = model->getTex(iface, i);
		world_coords[i] = model->getVert(iface, i);
		world_coords[i] /= world_coords[i].w();
	}
	float deltaU1 = uv[1].x() - uv[0].x(), deltaU2 = uv[2].x() - uv[0].x();
	float deltaV1 = uv[1].y() - uv[0].y(), deltaV2 = uv[2].y() - uv[0].y();
	Matrix<float, 2, 3> E, TB;
	Matrix2f deltaUV;
	E << (world_coords[1].head<3>() - world_coords[0].head<3>()).transpose(),
		(world_coords[2].head<3>() - world_coords[0].head<3>()).transpose();
	deltaUV << deltaV2, -deltaV1, -deltaU2, deltaU1;
	TB = 1.f / (deltaU1 * deltaV2 - deltaU2 * deltaV1) * deltaUV * E;
	Vector3f T = TB.row(0).normalized(), B = TB.row(1).normalized();
	Vector3f N = E.row(0).cross(E.row(1)).normalized();
	Matrix3f TBN;
	TBN << T, B, N;
	return TBN;
}

Vector3f Geometry::get_barycentric_coordinate(Vector2f pts[3], Vector2f P) {
	Vector3f bary_coord;
	bary_coord(0) = ((pts[1].y() - pts[2].y()) * (P.x() - pts[2].x()) + (pts[2].x() - pts[1].x()) * (P.y() - pts[2].y())) /
		((pts[1].y() - pts[2].y()) * (pts[0].x() - pts[2].x()) + (pts[2].x() - pts[1].x()) * (pts[0].y() - pts[2].y()));
	bary_coord(1) = ((pts[2].y() - pts[0].y()) * (P.x() - pts[2].x()) + (pts[0].x() - pts[2].x()) * (P.y() - pts[2].y())) /
		((pts[1].y() - pts[2].y()) * (pts[0].x() - pts[2].x()) + (pts[2].x() - pts[1].x()) * (pts[0].y() - pts[2].y()));
	bary_coord(2) = 1 - bary_coord(0) - bary_coord(1);
	return bary_coord;
}

Matrix4f Geometry::rotate_y(float angle) {
	float arc = angle / 180. * PI;
	Matrix4f trans;
	trans <<
		std::cos(arc), 0, std::sin(arc), 0,
		0, 1, 0, 0,
		- std::sin(arc), 0, std::cos(arc), 0,
		0, 0, 0, 1;
	return trans;
}

Eigen::Matrix4f Geometry::model_trans() {
	Eigen::Matrix4f trans;
	trans <<
		1, 0, 0, 0,
		0, 1, 0, 0,
		0, 0, 1, 0,
		0, 0, 0, 1;
	return trans;
}

Eigen::Matrix4f Geometry::view_trans(Eigen::Vector4f eye_pos, Eigen::Vector4f gaze_at, Eigen::Vector4f up_dir) {
	// Normalize input vectors (make sure they are unit vectors)
	Vector4f look_dir = gaze_at - eye_pos;
	look_dir.head<3>().normalize();
	up_dir.head<3>().normalize();

	// Calculate right vector (perpendicular to look and up)
	Eigen::Vector3f right = up_dir.head<3>().cross(-look_dir.head<3>()).normalized();

	// Recalculate up vector to make sure all vectors are orthogonal
	up_dir.head<3>() = -look_dir.head<3>().cross(right).normalized();

	// Construct view matrix (rotation + translation)
	Eigen::Matrix4f trans;
	trans <<
		right.x(), right.y(), right.z(), -eye_pos.head<3>().dot(right),
		up_dir.x(), up_dir.y(), up_dir.z(), -eye_pos.head<3>().dot(up_dir.head<3>()),
		-look_dir.x(), -look_dir.y(), -look_dir.z(), eye_pos.head<3>().dot(look_dir.head<3>()),
		0, 0, 0, 1;

	return trans;
}

Eigen::Matrix4f Geometry::pers_trans(float near, float far, float fovY, float aspect) {
	float t = std::tan(fovY / 180. * PI / 2), b = -t;
	float r = t * aspect, l = -r;
	float f = far, n = near;
	Eigen::Matrix4f ortho_proj;
	ortho_proj <<
		2 / (r - l), 0, 0, 0,
		0, 2 / (t - b), 0, 0,
		0, 0, 2 / (n - f), -(n + f) / (n - f),
		0, 0, 0, 1;
	Eigen::Matrix4f squish_matrix;
	squish_matrix <<
		n, 0, 0, 0,
		0, n, 0, 0,
		0, 0, n + f, -n * f,
		0, 0, 1, 0;
	return ortho_proj * squish_matrix;
}


Matrix4f Geometry::get_mvp_matrix(Eigen::Vector4f eye_pos, Eigen::Vector4f look_dir, Eigen::Vector4f up_dir,
	float near, float far, float fovY, float aspect) {
	//Model Projection
	Matrix4f model = Geometry::model_trans();
	//view projection
	Matrix4f view = Geometry::view_trans(eye_pos, look_dir, up_dir);
	//perspective projection
	Matrix4f pers = Geometry::pers_trans(near, far, fovY, aspect);
	return pers * view * model;
}

Eigen::Matrix4f Geometry::get_viewport_matrix(int height, int width) {
	Eigen::Matrix4f trans;
	trans <<
		width / 2, 0, 0, width / 2,
		0, height / 2, 0, height / 2,
		0, 0, 1, 0,
		0, 0, 0, 1;
	return trans;
}