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

namespace simple_graphics {



	extern Matrix ModelView;
	extern Matrix Viewport;
	extern Matrix Projection;

	void viewport(int x, int y, int w, int h);
	void projection(float coeff = 0.f); // coeff = -1/c
	void lookat(Vec3f eye, Vec3f center, Vec3f up);

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
	};

	class IShader {
	public:
		virtual ~IShader() = default;
		virtual Vec3f vertex(int iface, int nthvert) = 0;
		virtual bool fragment(Vec3f bar, SimpleColor& color) = 0;
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
			z_buffer_.resize(window_height * window_width);
			for (int i = 0; i < z_buffer_.size(); ++i) {
				z_buffer_[i] = std::numeric_limits<float>::lowest();
			}
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

		float DirectionalComponent(const Vec3f triangle[3], const Vec3f& direction);

		void DrawTriangleOutline(Vec2i a, Vec2i b, Vec2i c);

		void DrawTriangle(const Vec3f triangle[3], IShader& shader);

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

		void DrawLine(Vec2i a, Vec2i b);

	private:
		int window_width_;
		int window_height_;
		Gdiplus::Color color_;
		std::unique_ptr<Gdiplus::Graphics> gf_;
		std::unique_ptr<Gdiplus::Bitmap> bmp_;
		std::vector<float> z_buffer_;
	};

}
