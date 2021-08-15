#ifndef __GEOMETRY_H__
#define __GEOMETRY_H__

#include <cmath>
#include <iostream>
#include <vector>


const int DEFAULT_ALLOC = 4;

class Matrix {
	std::vector<std::vector<float> > m;
	int rows, cols;
public:
	Matrix(int r = DEFAULT_ALLOC, int c = DEFAULT_ALLOC);
	inline int nrows() const;
	inline int ncols() const;

	static Matrix identity(int dimensions);
	std::vector<float>& operator[](int i);
	const std::vector<float>& operator[](int i) const {
		return m[i];
	}
	Matrix operator*(const Matrix& a);
	Matrix transpose();
	Matrix inverse();

	friend std::ostream& operator<<(std::ostream& s, Matrix& m);
};

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

template <class t> struct Vec2 {
	union {
		struct {t u, v;};
		struct {t x, y;};
		t raw[2];
	};
	Vec2() : u(0), v(0) {}
	Vec2(t _u, t _v) : u(_u),v(_v) {}
	inline Vec2<t> operator +(const Vec2<t> &V) const { return Vec2<t>(u+V.u, v+V.v); }
	inline Vec2<t> operator -(const Vec2<t> &V) const { return Vec2<t>(u-V.u, v-V.v); }
	inline Vec2<t> operator *(float f)          const { return Vec2<t>(u*f, v*f); }
	inline bool operator ==(const Vec2<t>& V) const { return u == V.u && v == V.v; }
	inline t&	   operator [](int i) { return raw[i]; }
	inline const t& operator[](int i) const { return raw[i]; }
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec2<t>& v);
	template <class c> inline Vec2<c> Cast() const { return Vec2<c>((c)x, (c)y); }
};

template <class t> struct Vec3 {
	union {
		struct {t x, y, z;};
		struct { t ivert, iuv, inorm; };
		t raw[3];
	};
	Vec3() : x(0), y(0), z(0) {}
	Vec3(t _x, t _y, t _z) : x(_x),y(_y),z(_z) {}
	Vec3(const Matrix& homogenous);
	inline Vec3<t> operator ^(const Vec3<t> &v) const { return Vec3<t>(y*v.z-z*v.y, z*v.x-x*v.z, x*v.y-y*v.x); }
	inline Vec3<t> operator +(const Vec3<t> &v) const { return Vec3<t>(x+v.x, y+v.y, z+v.z); }
	inline Vec3<t> operator -(const Vec3<t> &v) const { return Vec3<t>(x-v.x, y-v.y, z-v.z); }
	inline Vec3<t> operator *(float f)          const { return Vec3<t>(x*f, y*f, z*f); }
	inline t       operator *(const Vec3<t> &v) const { return x*v.x + y*v.y + z*v.z; }
	inline t&	   operator [](int i) { return raw[i]; }
	inline const t& operator[](int i) const { return raw[i]; }
	float norm () const { return std::sqrt(x*x+y*y+z*z); }
	Vec3<t> & normalize(t l=1) { *this = (*this)*(l/norm()); return *this; }
	Vec2<t> AsVec2() const { return Vec2<t>(x, y); }
	Matrix AsHomogenous() const { 
		Matrix matrix(4, 1); 
		matrix[0][0] = x; 
		matrix[1][0] = y; 
		matrix[2][0] = z; 
		matrix[3][0] = 1;
		return matrix; 
	}
	template <class > friend std::ostream& operator<<(std::ostream& s, Vec3<t>& v);
	template <class c> inline Vec3<c> Cast() const { return Vec3<c>((c)x, (c)y, (c)z); }
};

typedef Vec2<float> Vec2f;
typedef Vec2<int>   Vec2i;
typedef Vec3<float> Vec3f;
typedef Vec3<int>   Vec3i;

template <class t> std::ostream& operator<<(std::ostream& s, Vec2<t>& v) {
	s << "(" << v.x << ", " << v.y << ")\n";
	return s;
}

template <class t> std::ostream& operator<<(std::ostream& s, Vec3<t>& v) {
	s << "(" << v.x << ", " << v.y << ", " << v.z << ")\n";
	return s;
}

template<>
Vec3f::Vec3(const Matrix& matrix);



#endif //__GEOMETRY_H__


