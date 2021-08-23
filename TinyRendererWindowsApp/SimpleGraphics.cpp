#include "SimpleGraphics.h"
#include <functional>
#include <algorithm>

namespace simple_graphics {
	namespace {

		

		template<int n>
		vec<3> barycentric(const vec<2>& p, const vec<n> triangle[3]) {
			vec<2> a_b = proj<2>(triangle[1]) - proj<2>(triangle[0]);
			vec<2> a_c = proj<2>(triangle[2]) - proj<2>(triangle[0]);
			vec<2> p_a = proj<2>(triangle[0]) - p;
			vec<3> cross_product = cross(vec<3>(a_b.x, a_c.x, p_a.x),  vec<3>(a_b.y, a_c.y, p_a.y));
			if (cross_product.z == 0) {
				return vec<3>(-1,1,1);
			}
			float u = cross_product.x / (float)cross_product.z;
			float v = cross_product.y / (float)cross_product.z;
			return vec<3>(1 - u - v, u, v);
		}

		bool InTriangle(const vec<3>& bary) {
			return bary[0] >= 0 && bary[1] >= 0 && bary[2] >= 0;
		}

		float GetZForPoint(const vec<3>& bary, const vec<3> triangle[3]) {
			return bary.y * triangle[1].z + bary.z * triangle[2].z + bary.x  * triangle[0].z;
		}

		

		SimpleColor GetColorForPoint(const vec<3>& bary, const vec<2> texture[3], const TGAImage& image) {
			const vec<2> texture_coordinate = texture[0] * bary[0] + texture[1] * bary[1]
				+ texture[2] * bary[2];

			int p_x = max(0, min(1, texture_coordinate.x)) * image.get_width();
			int p_y =  max(0, min(1, texture_coordinate.y)) * image.get_height();
			return SimpleColor(image.get(p_x, p_y));
		}


		std::pair<int, int> Bounds(int a, int b, int c , int max) {
			if (a > b) {
				std::swap(a, b);
			}
			if (b > c) {
				std::swap(b, c);
			}
			if (a > b) {
				std::swap(a, b);
			}
			a = std::clamp(a, 0, max-1);
			c = std::clamp(c, 0, max - 1);
			return std::pair<int, int>(a, c);
		}


		float GetBrightnessForPoint(const vec<3>& bary, const vec<3> normals[3], const vec<3>& light_direction) {
			vec<3> normal = normals[0] * bary[0] + normals[1] * bary[1] + normals[2] * bary[2];
			return min(1,max(0,normal.normalize() * light_direction * -1 ));
		}

	}

	mat<4,4> ModelView;
	mat<4, 4> Viewport;
	mat<4, 4> Projection;
	ZBuffer DrawZBuffer;
	ZBuffer ShadowZBuffer;

	void viewport(int x, int y, int w, int h) {
		Viewport = mat<4, 4>::identity();
		Viewport[0][3] = x + w / 2.f;
		Viewport[1][3] = y + h / 2.f;
		Viewport[2][3] = 0;

		Viewport[0][0] = w / 2.f;
		Viewport[1][1] = h / 2.f;
		Viewport[2][2] = 1;
	}

	void projection(float coeff) {
		Projection = mat<4,4>::identity();
		Projection[3][2] = coeff;
	}
	
	void lookat(vec<3>& eye, vec<3>& center, vec<3>& up) {
		vec<3> z = (eye - center).normalize();
		vec<3> x = cross(up, z).normalize();
		vec<3> y = cross(z, x).normalize();
		ModelView = mat<4,4>::identity();
		for (int i = 0; i < 3; i++) {
			ModelView[0][i] = x[i];
			ModelView[1][i] = y[i];
			ModelView[2][i] = z[i];
			ModelView[i][3] = -center[i];
		}
	}


	float SimpleGraphics::DirectionalComponent(const vec<3> triangle[3], const vec<3>& direction) {
		vec<3> normal = cross((triangle[2] - triangle[0]), (triangle[1] - triangle[0]));
		normal.normalize();
		return	direction * normal;
	}

	void SimpleGraphics::DrawTriangle(const vec<3> triangle[3], IShader& shader, ZBuffer& z_buffer)
	{
		std::pair<int, int> bounds_x = Bounds(triangle[0].x, triangle[1].x, triangle[2].x, z_buffer.width);
		std::pair<int, int> bounds_y = Bounds(triangle[0].y, triangle[1].y, triangle[2].y, z_buffer.height);
		bounds_x.first = max(0, bounds_x.first);
		bounds_x.second = min(window_width_ - 1, bounds_x.second);
		bounds_y.first = max(0, bounds_y.first);
		bounds_y.second = min(window_width_ - 1, bounds_y.second);

		vec<2> point;
		SimpleColor color(0,0,0,0);
		for (point.y = bounds_y.first; point.y <= bounds_y.second; ++point.y) {
			for (point.x = bounds_x.first; point.x <= bounds_x.second; ++point.x) {
				vec<3> bary = barycentric(vec<2>(point.x, point.y), triangle);
				if (InTriangle(bary)) {
					if (!shader.fragment(bary, color)) {
						float z = GetZForPoint(bary, triangle);
						if (z > z_buffer(point.x, point.y)) {
							z_buffer(point.x, point.y) = z;
							if(&z_buffer != &ShadowZBuffer){
								SetColor(color);
								DrawPoint(point.x, point.y);
							}
						}
					}
				}
			}
		}
	}
	void SimpleGraphics::DrawLine( const vec<2>& orig_a,  const vec<2>& orig_b)
	{
		vec<2> a = orig_a;
		vec<2> b = orig_b;
		if (a == b) {
			DrawPoint(a.x, a.y);
			return;
		}
		std::function<void(int x, int y)> draw_point = [this](int x, int y) {DrawPoint(x, y); };
		if (std::abs(a.y - b.y) > std::abs(a.x - b.x)) {
			draw_point = [this](int x, int y) {DrawPoint(y, x); };
			std::swap(a.x, a.y);
			std::swap(b.x, b.y);
		}
		if (a.x > b.x) {
			std::swap(a, b);
		}
		int error = 0;
		
		int dx = b.x - a.x;
		int dy = b.y - a.y;
		int derror = 0;
		if (dy != 0) {
			derror = std::abs(dy) * 2;
		}
		int inc_y = dy > 0 ? 1 : -1;
		int y = a.y;
		for (int x = a.x; x <= b.x; ++x) {
			draw_point(x, y);
			error += derror;
			if(error > dx){
				error -= dx * 2;
				y += inc_y;
			}
		}
	}
}
