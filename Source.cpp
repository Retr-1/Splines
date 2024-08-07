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

		return points[i] * v0 + points[i + 1] * v1 + points[i + 2] * v2 + points[i + 3] * v3;
	}
};

// Override base class with your custom functionality
class Example : public olc::PixelGameEngine
{
	Spline spline;
	int selected = -1;

	void drawPoints(const std::vector<olc::vi2d>& points) {
		for (int i = 0; i< spline.points.size(); i++) {
			FillCircle(spline.points[i], 10, i==selected ? olc::YELLOW : olc::GREY);
		}
	}

public:
	Example()
	{
		// Name your application
		sAppName = "Example";
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

		return true;
	}
};

int main()
{
	Example win;
	if (win.Construct(800, 800, 1, 1))
		win.Start();
	return 0;
}