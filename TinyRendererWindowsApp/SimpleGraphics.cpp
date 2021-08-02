#include "SimpleGraphics.h"
#include <functional>

namespace simple_graphics {

	void SimpleGraphics::DrawLine( Point a,  Point b)
	{
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
