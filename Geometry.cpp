#include "geometry.h"
using namespace Eigen;
const float PI = 3.1415926;

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

