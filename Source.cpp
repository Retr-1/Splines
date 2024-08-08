#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

//typedef float cubic[4];
//
//float ev(cubic eq, float x) {
//	return eq[0] + eq[1] * x + eq[2] * x * x + eq[3] * x * x * x;
//}

struct cubic {
	float eq[4];

	float ev(float x) {
		return eq[0] + eq[1] * x + eq[2] * x * x + eq[3] * x * x * x;
	}
};

class Spline {
public:
	std::vector<olc::vf2d> points;
	float totalLength = 0;

	olc::vf2d getPointByT(float t) {
		int i = (int)t;
		t = t - i;

		float tt = t * t;
		float ttt = tt * t;

		float v0 = -ttt + 2 * tt - t;
		float v1 = 3 * ttt - 5 * tt + 2;
		float v2 = -3 * ttt + 4 * tt + t;
		float v3 = ttt - tt;

		float x = 0.5f * (points[i].x * v0 + points[i + 1].x * v1 + points[i + 2].x * v2 + points[i + 3].x * v3);
		float y = 0.5f * (points[i].y * v0 + points[i + 1].y * v1 + points[i + 2].y * v2 + points[i + 3].y * v3);
		return { x,y };
	}

	olc::vf2d getDirByT(float t) {
		int i = (int)t;
		t = t - i;

		float tt = t * t;
		float ttt = tt * t;

		float v0 = -3*tt + 4 * t - 1;
		float v1 = 9 * tt - 10 * t;
		float v2 = -9 * tt + 8 * t + 1;
		float v3 = 3*tt - 2*t;

		float x = 0.5f * (points[i].x * v0 + points[i + 1].x * v1 + points[i + 2].x * v2 + points[i + 3].x * v3);
		float y = 0.5f * (points[i].y * v0 + points[i + 1].y * v1 + points[i + 2].y * v2 + points[i + 3].y * v3);
		return { x,y };
	}

	olc::vf2d getPointByDistance(float distance) {
		for (int i = 0; i < points.size() - 3; i++) {
			float currLength = getLength(i);
			if (distance < currLength) {
				return getPointByT(distance / currLength);
			}
			else {
				distance -= currLength;
			}
		}
		throw std::invalid_argument("Distance bigger than total length!");
	}

	olc::vf2d getDirByDistance(float distance) {
		for (int i = 0; i < points.size() - 3; i++) {
			float currLength = getLength(i);
			if (distance < currLength) {
				return getDirByT(distance / currLength);
			}
			else {
				distance -= currLength;
			}
		}
		throw std::invalid_argument("Distance bigger than total length!");
	}

	float getLength(int i) {
		cubic F0 = { 0, -1, 2, -1 };
		cubic F1 = { 2, 0, -5, 3 };
		cubic F2 = { 0, 1, 4, -3 };
		cubic F3 = { 0,0,-1,1 };
		
		float v0 = 8 / 27.0f;//-(F0.ev(1 / 3.0f) - F0.ev(0)) + (F0.ev(1) - F0.ev(1 / 3.0f));
		float v1 = 2;//-(F1.ev(1) - F1.ev(0));
		float v2 = 2;//F2.ev(1) - F2.ev(0);
		float v3 = 8 / 27.0f;//-(F3.ev(2 / 3.0f) - F3.ev(0)) + F3.ev(1) - F3.ev(2 / 3.0f);

		float x = 0.5f * (points[i].x * v0 + points[i + 1].x * v1 + points[i + 2].x * v2 + points[i + 3].x * v3);
		float y = 0.5f * (points[i].y * v0 + points[i + 1].y * v1 + points[i + 2].y * v2 + points[i + 3].y * v3);
		return x + y;
	}

	//float getTotalLength() {
	//	float sum = 0;
	//	for (int i = 0; i < points.size()-3; i++) {
	//		sum += getLength(i);
	//	}
	//}

	void recalculate() {
		totalLength = 0;
		for (int i = 0; i < points.size() - 3; i++) {
			totalLength += getLength(i);
		}
	}
};

// Override base class with your custom functionality
class Window : public olc::PixelGameEngine
{
	Spline spline;
	int selected = -1;
	float carDistance = 0;

	void drawPoints(const std::vector<olc::vf2d>& points) {
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
		for (int i = 0; i < 4; i++) {
			spline.points.push_back(olc::vf2d(x, 300));
			x += 50;
		}
		spline.recalculate();
		return true;
		
	}

	bool OnUserUpdate(float fElapsedTime) override
	{

		if (GetMouse(olc::Mouse::LEFT).bHeld && selected != -1) {
			spline.points[selected] = GetMousePos();
			spline.recalculate();
			std::cout << "Spline length: " << spline.totalLength << '\n';
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

		carDistance += fElapsedTime * 100;
		carDistance = fmodf(carDistance, spline.totalLength);

		Clear(olc::BLACK);
		drawPoints(spline.points);
		for (float t = 0; t < spline.points.size() - 3; t += 0.01f) {
			auto point = spline.getPointByT(t);
			//std::cout << point.str();
			Draw(point);
		}
		auto carPos = spline.getPointByDistance(carDistance);
		auto carDir = (spline.getDirByDistance(carDistance).norm())*10;
		auto carN = olc::vf2d(carDir.y, -carDir.x);
		FillTriangle(carPos + carN, carPos - carN, carPos + carDir, olc::RED);

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