#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class Spline {
public:
	std::vector<olc::vi2d> points;

	olc::vi2d getPointByT(float p) {

		/*cubic e0 = { -1,2,-1 };
		cubic e1 = { 3,-5,2 };
		cubic e2 = { -3, 4, 1 };
		cubic e3 = { 1,-1,0 };*/
		int i = (int)p;
		float t = p - i;

		float tt = t * t;
		float ttt = tt * t;

		float v0 = -ttt + 2 * tt - t;
		float v1 = 3 * ttt - 5 * tt + 2 * t;
		float v2 = -3 * ttt + 4 * tt + t;
		float v3 = ttt - tt;

		int x = points[i].x * v0 + points[i + 1].x * v1 + points[i + 2].x * v2 + points[i + 3].x * v3;
		int y = points[i].y * v0 + points[i + 1].y * v1 + points[i + 2].y * v2 + points[i + 3].y * v3;
		return { x,y };
	}
};

// Override base class with your custom functionality
class Window : public olc::PixelGameEngine
{
	Spline spline;
	int selected = -1;

	void drawPoints(const std::vector<olc::vi2d>& points) {
		for (int i = 0; i< spline.points.size(); i++) {
			FillCircle(spline.points[i], 10, i==selected ? olc::YELLOW : olc::GREY);
		}
	}

public:
	Window()
	{
		// Name your application
		sAppName = "Window";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		int x = 100;
		for (int i = 0; i < 10; i++) {
			spline.points.push_back(olc::vi2d(x, 300));
			x += 50;
		}
		return true;
		
	}

	bool OnUserUpdate(float fElapsedTime) override
	{

		if (GetMouse(olc::Mouse::LEFT).bHeld && selected != -1) {
			spline.points[selected] = GetMousePos();
		}
		else {
			selected = -1;
			for (int i = 0; i < spline.points.size(); i++) {
				auto p = spline.points[i];
				auto m = GetMousePos();
				float d = sqrtf(powf(m.x - p.x, 2) + powf(m.y - p.y, 2));
				if (d < 10)
					selected = i;
			}
		}

		Clear(olc::BLACK);
		drawPoints(spline.points);
		for (float t = 0; t < spline.points.size() - 3; t += 0.01f) {
			auto point = spline.getPointByT(t);
			//std::cout << point.str();
			Draw(point);
		}

		return true;
	}
};

int main()
{
	Window win;
	if (win.Construct(800, 800, 1, 1))
		win.Start();
	return 0;
}