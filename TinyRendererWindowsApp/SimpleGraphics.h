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
			brush_.reset(new Gdiplus::SolidBrush(Gdiplus::Color(0, 0, 0)));
			FlushWindow();
		}

		void SetColor(const SimpleColor& color) {
			brush_->SetColor(Gdiplus::Color(color.a, color.r, color.g, color.b));
		}

		void DrawPoint(int x, int y) {
			gf_->FillRectangle(brush_.get(), Gdiplus::Rect(x, window_height_ - 1 - y, 1, 1));
		}

		void DrawPoint(int x, int y, const SimpleColor& color) {
			SetColor(color);
			DrawPoint(x, y);
		}
		void FlushWindow() {
			brush_->SetColor(Gdiplus::Color(0, 0, 0));
			gf_->FillRectangle(brush_.get(), Gdiplus::Rect(0, 0, window_width_, window_height_));
		}

		void DrawLine(const Point& a, const Point& b);

	private:
		int window_width_;
		int window_height_;
		std::unique_ptr<Gdiplus::Graphics> gf_;
		std::unique_ptr<Gdiplus::SolidBrush> brush_;
	};

}
