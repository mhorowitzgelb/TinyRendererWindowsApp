#pragma once
#include <memory>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")
#include "geometry.h"
#include <vector>
#include "tgaimage.h"
#include <limits>
#include <algorithm>

namespace simple_graphics {

	struct ZBuffer {
		std::vector<float> buffer;
		int width;
		int height;
		float& operator ()(int x, int y) {
			return buffer[y * width + x];
		}
		void reset(int new_width, int new_height) {
			width = new_width;
			height = new_height;
			buffer.resize(width * height);
			for (float& value : buffer) {
				value = std::numeric_limits<float>::lowest();
			}
		}
	};

	extern mat<4,4> ModelView;
	extern mat<4,4> Viewport;
	extern mat<4,4> Projection;
	extern ZBuffer DrawZBuffer;
	extern ZBuffer ShadowZBuffer;

	void viewport(int x, int y, int w, int h);
	void projection(float coeff = 0.f); // coeff = -1/c
	void lookat(vec<3>& eye, vec<3>& center, vec<3>& up);

	struct Point {
		Point(int x, int y) : x(x), y(y) {}
		int x;
		int y;
	};

	struct SimpleColor {
		SimpleColor(UINT8 r, UINT8 g, UINT8 b, UINT8 a) : r(r), g(g), b(b), a(a) {}
		SimpleColor(const TGAColor& color) : r(color.bgra[2]), g(color.bgra[1]), b(color.bgra[0]), a(255){}
		UINT8 r;
		UINT8 g;
		UINT8 b;
		UINT8 a;
		inline SimpleColor operator *(double f) { return SimpleColor(std::clamp(r * f, 0.0,255.0), std::clamp(g * f, 0.0, 255.0), std::clamp(b * f, 0.0, 255.0), a); }
		inline SimpleColor operator +(uint8_t f) { return SimpleColor(std::clamp(r + f, 0, 255), std::clamp(g + f, 0, 255), std::clamp(b + f, 0, 255), a); }
	};

	class IShader {
	public:
		virtual ~IShader() = default;
		virtual vec<3> vertex(int iface, int nthvert) = 0;
		virtual bool fragment(const vec<3>& bar, SimpleColor& color) = 0;
	};

	class SimpleGraphics {
	public:
		SimpleGraphics() = delete;
		SimpleGraphics(const SimpleGraphics& graphics) = delete;
		explicit SimpleGraphics(const HDC& hdc, int window_width, int window_height) {
			window_width_ = window_width;
			window_height_ = window_height;
			gf_.reset(new Gdiplus::Graphics(hdc));
			bmp_.reset(new Gdiplus::Bitmap(window_width, window_height, PixelFormat32bppARGB));
			DrawZBuffer.reset(window_width, window_height);
			ShadowZBuffer.reset(window_width * 2, window_height * 2);
			FlushWindow();
		}

		void SetColor(const SimpleColor& color) {
			color_ = Gdiplus::Color(color.a, color.r, color.g, color.b);
		}

		void DrawPoint(int x, int y) {
			bmp_->SetPixel(x, window_height_ - 1 - y, color_);
		}

		void DrawPoint(int x, int y, const SimpleColor& color) {
			SetColor(color);
			DrawPoint(x, y);
		}

		float DirectionalComponent(const vec<3> triangle[3], const vec<3>& direction);

		void DrawTriangle(const vec<3> triangle[3], IShader& shader, ZBuffer& z_buffer);

		void FlushWindow() {
			Gdiplus::BitmapData data;
			Gdiplus::Rect rect(0, 0, window_width_, window_height_);
			bmp_->LockBits(&rect, Gdiplus::ImageLockModeWrite,
				PixelFormat32bppARGB, &data);
			memset(data.Scan0, 0, data.Height * data.Stride);
			uint8_t* alpha_ptr = (uint8_t*)(data.Scan0);
			for (int y = 0; y < data.Height; ++y) {
				uint8_t* alpha_row_ptr = alpha_ptr;
				for (int x = 0; x < data.Width; ++x) {
					alpha_row_ptr[0] = 125;
					alpha_row_ptr[1] = 125;
					alpha_row_ptr[2] = 125;
					alpha_row_ptr[3] = 255;
					alpha_row_ptr += 4;
				}
				alpha_ptr += data.Stride;
			}
			bmp_->UnlockBits(&data);
		}

		void Render(int scale = 1) {
			int new_width = scale * bmp_->GetWidth();
			int new_height = scale * bmp_->GetHeight();
			Gdiplus::Rect sizeR(0, 0, new_width, new_height);
			gf_->SetInterpolationMode(Gdiplus::InterpolationModeNearestNeighbor);
			gf_->ScaleTransform(scale, scale);
			gf_->DrawImage(bmp_.get(), sizeR, 0, 0,
				(int)bmp_->GetWidth(),
				(int)bmp_->GetHeight(),
				Gdiplus::UnitPixel);
		}

		void DrawLine(const vec<2>& a, const vec<2>& b);

	private:
		int window_width_;
		int window_height_;
		Gdiplus::Color color_;
		std::unique_ptr<Gdiplus::Graphics> gf_;
		std::unique_ptr<Gdiplus::Bitmap> bmp_;
	};

}
