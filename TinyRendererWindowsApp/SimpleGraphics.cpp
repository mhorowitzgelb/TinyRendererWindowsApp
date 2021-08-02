#include "SimpleGraphics.h"

namespace simple_graphics {

	void SimpleGraphics::DrawLine(const Point& a, const Point& b)
	{
		for (int x = a.x; x <= b.x; ++x) {
			int y = (x - a.x) / static_cast<float>(b.x - a.x) * (b.y - a.y);
			DrawPoint(x, y);
		}
	}
}
