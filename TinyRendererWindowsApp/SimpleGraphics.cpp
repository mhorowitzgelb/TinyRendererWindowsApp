#include "SimpleGraphics.h"
#include <functional>
#include <algorithm>

namespace simple_graphics {
	namespace {

		


		template<typename t>
		Vec3f barycentric(const Vec2<t>& p, const Vec3<t> triangle[3]) {
			Vec2<t> a_b = triangle[1].AsVec2() - triangle[0].AsVec2();
			Vec2<t> a_c = triangle[2].AsVec2() - triangle[0].AsVec2();
			Vec2<t> p_a = triangle[0].AsVec2() - p;
			Vec3<t> cross = Vec3<t>(a_b.x, a_c.x, p_a.x) ^ Vec3<t>(a_b.y, a_c.y, p_a.y);
			if (cross.z == 0) {
				return Vec3f(-1,1,1);
			}
			float u = cross.x / (float)cross.z;
			float v = cross.y / (float)cross.z;
			return Vec3f(1 - u - v, u, v);
		}

		bool InTriangle(const Vec3f& bary) {
			return bary[0] >= 0 && bary[1] >= 0 && bary[2] >= 0;
		}

		float GetZForPoint(const Vec3f& bary, const Vec3f triangle[3]) {
			return bary.y * triangle[1].z + bary.z * triangle[2].z + bary.x  * triangle[0].z;
		}

		

		SimpleColor GetColorForPoint(const Vec3f& bary, const Vec2f texture[3], const TGAImage& image) {
			const Vec2f texture_coordinate = texture[0] * bary[0] + texture[1] * bary[1]
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


		float GetBrightnessForPoint(const Vec3f& bary, const Vec3f normals[3], const Vec3f& light_direction) {
			Vec3f normal = normals[0] * bary[0] + normals[1] * bary[1] + normals[2] * bary[2];
			return min(1,max(0,normal.normalize() * light_direction * -1 ));
		}

	}

	Matrix ModelView;
	Matrix Viewport;
	Matrix Projection;

	void viewport(int x, int y, int w, int h) {
		Viewport = Matrix::identity(4);
		Viewport[0][3] = x + w / 2.f;
		Viewport[1][3] = y + h / 2.f;
		Viewport[2][3] = 0;

		Viewport[0][0] = w / 2.f;
		Viewport[1][1] = h / 2.f;
		Viewport[2][2] = 1;
	}

	void projection(float coeff) {
		Projection = Matrix::identity(4);
		Projection[3][2] = coeff;
	}
	
	void lookat(Vec3f eye, Vec3f center, Vec3f up) {
		Vec3f z = (eye - center).normalize();
		Vec3f x = (up ^ z).normalize();
		Vec3f y = (z ^ x).normalize();
		ModelView = Matrix::identity(4);
		for (int i = 0; i < 3; i++) {
			ModelView[0][i] = x[i];
			ModelView[1][i] = y[i];
			ModelView[2][i] = z[i];
			ModelView[i][3] = -center[i];
		}
	}


	float SimpleGraphics::DirectionalComponent(const Vec3f triangle[3], const Vec3f& direction) {
		Vec3f normal = (triangle[2] - triangle[0]) ^ (triangle[1] - triangle[0]);
		normal.normalize();
		return	direction * normal;
	}


	void SimpleGraphics::DrawTriangleOutline(Vec2i a, Vec2i b, Vec2i c)
	{
		DrawLine(a, b);
		DrawLine(b, c);
		DrawLine(c, a);
	}
	
	void SimpleGraphics::DrawTriangle(const Vec3f triangle[3], IShader& shader)
	{
		std::pair<int, int> bounds_x = Bounds(triangle[0].x, triangle[1].x, triangle[2].x, window_width_);
		std::pair<int, int> bounds_y = Bounds(triangle[0].y, triangle[1].y, triangle[2].y, window_height_);
		bounds_x.first = max(0, bounds_x.first);
		bounds_x.second = min(window_width_ - 1, bounds_x.second);
		bounds_y.first = max(0, bounds_y.first);
		bounds_y.second = min(window_width_ - 1, bounds_y.second);

		Vec2i point;
		SimpleColor color(0,0,0,0);
		for (point.y = bounds_y.first; point.y <= bounds_y.second; ++point.y) {
			for (point.x = bounds_x.first; point.x <= bounds_x.second; ++point.x) {
				Vec3f bary = barycentric(Vec2f(point.x, point.y), triangle);
				
				if (InTriangle(bary)) {
					if (!shader.fragment(bary, color)) {
						float z = GetZForPoint(bary, triangle);
						if (z > z_buffer_[point.y * window_width_ + point.x]) {
							SetColor(color);
							z_buffer_[point.y * window_width_ + point.x] = z;
							DrawPoint(point.x, point.y);
						}
					}
				}
			}
		}
	}
	void SimpleGraphics::DrawLine( Vec2i a,  Vec2i b)
	{
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
