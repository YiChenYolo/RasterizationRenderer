#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__
#include <cmath>
#include <ostream>

template<class t> struct Vec2 {
	union {
		struct { t x, y; };
		struct { t u, v; };
		t raw[2];
	};
	Vec2() :u(0), v(0) {}
	Vec2(t _u, t _v) :u(_u), v(_v) {}
	
	inline Vec2<t> operator +(const Vec2<t>& V) { return Vec2(u + V.u, v + V.v); }
	inline Vec2<t> operator -(const Vec2<t>& V) { return Vec2(u - V.u, v - V.v); }
	inline Vec2<t> operator *(float f) { return Vec2(u * f, v * f); }
	template<class > friend std::ostream& operator<<(std::ostream& s,const Vec2<t>& V);
};

template<class t> struct Vec3 {
	union {
		struct { t x, y, z; };
		struct { t u, v, w; };
		t raw[3];
	};
	Vec3() : u(0), v(0), w(0){}
	Vec3(t _u,t _v,t _w) :u(_u), v(_v), w(_w) {}
	inline Vec3<t> operator +(const Vec3<t>& V) { return Vec3(u + V.u, v + V.v, w + V.w); }
	inline Vec3<t> operator -(const Vec3<t>& V) { return Vec3(u - V.u, v - V.v, w - V.w); }
	inline Vec3<t> operator *(float f) { return Vec3(u * f, v * f, w * f); }
	inline t operator *(const Vec3<t>& V) { return u * V.u + v * V.v + w * V.w; }
	template<class > friend std::ostream& operator<<(std::ostream& s, const Vec3<t>& V);
};
#endif


using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;
using Vec3f = Vec3<float>;
using Vec3i = Vec3<int>;

template <class t> std::ostream& operator<<(std::ostream& s, const Vec3<t>& V) {
	s << "(" << V.u << ", " << V.v << ", " << V.w << ")";
	return s;
}

template <class t> std::ostream& operator<<(std::ostream& s, const Vec2<t>& V) {
	s << "(" << V.u << ", " << V.v << " )";
	return s;
}