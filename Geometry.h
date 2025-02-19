#ifndef __GEOMETRY__
#define __GEOMETRY__
#include<Eigen/Dense>

class Geometry
{
public:
	static Eigen::Vector3f get_barycentric_coordinate(Eigen::Vector2f pts[3], Eigen::Vector2f P);
	static Eigen::Matrix4f rotate_y(float angle);
	template<class T> static T bary_interpolate(Eigen::Vector3f bary_coord, const std::array<T, 3>& v) {
		return v[0] * bary_coord[0] + v[1] * bary_coord[1] + v[2] * bary_coord[2];
	}
};
#endif // !__GEOMETRY__

