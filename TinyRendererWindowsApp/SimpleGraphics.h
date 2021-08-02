#pragma once
#include <memory>
#include <windows.h>
#include <objidl.h>
#include <gdiplus.h>
#pragma comment (lib,"Gdiplus.lib")

namespace simple_graphics {
	struct Point {
		Point(int x, int y) : x(x), y(y) {}
		int x;
		int y;
	};

	struct SimpleColor {
		SimpleColor(UINT8 r, UINT8 g, UINT8 b, UINT8 a) : r(r), g(g), b(b), a(a) {}
		UINT8 r;
		UINT8 g;
		UINT8 b;
		UINT8 a;
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
		void FlushWindow() {
			Gdiplus::BitmapData data;
			Gdiplus::Rect rect(0, 0, window_width_, window_height_);
			bmp_->LockBits(&rect, Gdiplus::ImageLockModeWrite,
				PixelFormat32bppARGB, &data);
			memset(data.Scan0, 0, data.Height * data.Stride);
			uint8_t* alpha_ptr = (uint8_t*)(data.Scan0) + 3;
			for (int y = 0; y < data.Height; ++y) {
				uint8_t* alpha_row_ptr = alpha_ptr;
				for (int x = 0; x < data.Width; ++x) {
					*alpha_row_ptr = 255;
					alpha_row_ptr += 4;
				}
				alpha_ptr += data.Stride;
			}
			bmp_->UnlockBits(&data);
		}

		void Render() {
			Gdiplus::Rect sizeR(0, 0, bmp_->GetWidth(), bmp_->GetHeight());
			gf_->DrawImage(bmp_.get(), sizeR, 0, 0,
				(int)bmp_->GetWidth(),
				(int)bmp_->GetHeight(),
				Gdiplus::UnitPixel);
		}

		void DrawLine(Point a, Point b);

	private:
		int window_width_;
		int window_height_;
		Gdiplus::Color color_;
		std::unique_ptr<Gdiplus::Graphics> gf_;
		std::unique_ptr<Gdiplus::Bitmap> bmp_;
	};

}
