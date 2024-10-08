#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"


class Spline {
private:
	float calcLength(int i) {
		const float step = 0.01f;
		float l = 0;
		for (float t = i; t + step < 1+i; t += step) {
			auto p1 = getPointByT(t);
			auto p2 = getPointByT(t + step);
			l += (p1 - p2).mag();
		}
		return l;
	}

public:
	std::vector<olc::vf2d> points;
	float totalLength = 0;
	std::vector<float> lengths;

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
			float currLength = lengths[i];
			if (distance < currLength) {
				return getPointByT(distance / currLength + i);
			}
			else {
				distance -= currLength;
			}
		}
		throw std::invalid_argument("Distance bigger than total length!");
	}

	olc::vf2d getDirByDistance(float distance) {
		for (int i = 0; i < points.size() - 3; i++) {
			float currLength = lengths[i];
			if (distance < currLength) {
				return getDirByT(distance / currLength + i);
			}
			else {
				distance -= currLength;
			}
		}
		throw std::invalid_argument("Distance bigger than total length!");
	}

	void recalculate() {
		totalLength = 0;
		lengths.resize(points.size()-3);
		for (int i = 0; i < points.size() - 3; i++) {
			lengths[i] = calcLength(i);
			totalLength += lengths[i];
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
		for (int i = 0; i < 10; i++) {
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