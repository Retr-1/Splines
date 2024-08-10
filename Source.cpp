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

		int s = points.size();
		float x = 0.5f * (points[i % s].x * v0 + points[(i + 1) % s].x * v1 + points[(i + 2) % s].x * v2 + points[(i + 3) % s].x * v3);
		float y = 0.5f * (points[i % s].y * v0 + points[(i + 1) % s].y * v1 + points[(i + 2) % s].y * v2 + points[(i + 3) % s].y * v3);
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

		int s = points.size();
		float x = 0.5f * (points[i%s].x * v0 + points[(i + 1)%s].x * v1 + points[(i + 2)%s].x * v2 + points[(i + 3)%s].x * v3);
		float y = 0.5f * (points[i%s].y * v0 + points[(i + 1)%s].y * v1 + points[(i + 2)%s].y * v2 + points[(i + 3)%s].y * v3);
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

	void draw(olc::PixelGameEngine* canvas) {
		for (float t = 0; t < totalLength; t += 1) {
			auto point = getPointByDistance(t);
			canvas->Draw(point);
		}
	}

	void drawByT(olc::PixelGameEngine* canvas, bool cyclic=false) {
		int stop = cyclic ? points.size() : points.size() - 3;
		for (float t = 0; t < stop; t += 0.01f) {
			auto point = getPointByT(t);
			canvas->Draw(point);
		}
	}
};

class Track {
public:
	Spline middle;
	Spline outer;
	Spline inner;

	int breadth = 40;
	std::vector<olc::vf2d>& points = middle.points;
	
	void recalculate() {
		middle.recalculate();
		inner.points.resize(points.size());
		outer.points.resize(points.size());
		for (int i = 0; i < points.size(); i++) {
			auto dir = middle.getDirByT(i);
			dir = (dir.norm()) * breadth;
			auto dirN = olc::vf2d(dir.y, -dir.x);
			outer.points[i] = points[i] + dirN;
			inner.points[i] = points[i] - dirN;
		}
		outer.recalculate();
		inner.recalculate();
	}

	void draw(olc::PixelGameEngine* canvas) {
		outer.draw(canvas);
		middle.draw(canvas);
		inner.draw(canvas);
	}

	void drawByT(olc::PixelGameEngine* canvas, bool cyclic=false) {
		outer.drawByT(canvas,cyclic);
		middle.drawByT(canvas,cyclic);
		inner.drawByT(canvas,cyclic);
	}
	
	void fill(olc::PixelGameEngine* canvas) {
		const float step = 0.25f;
		for (float t = 0; t < points.size(); t += step) {
			auto p1 = outer.getPointByT(t);
			auto p2 = inner.getPointByT(t);
			auto p3 = inner.getPointByT(t + step);
			auto p4 = outer.getPointByT(t + step);
			canvas->FillTriangle(p1, p2, p3, olc::DARK_RED);
			canvas->FillTriangle(p1, p4, p3, olc::DARK_RED);
		}
		middle.drawByT(canvas, true);
	}
};

// Override base class with your custom functionality
class Window : public olc::PixelGameEngine
{
	Track track;
	int selected = -1;
	float carDistance = 0;

	void drawPoints(const std::vector<olc::vf2d>& points) {
		for (int i = 0; i< points.size(); i++) {
			FillCircle(points[i], 10, i==selected ? olc::YELLOW : olc::GREY);
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
		//int x = 100;
		//for (int i = 0; i < 10; i++) {
		//	track.points.push_back(olc::vf2d(x, 300));
		//	x += 50;
		//}

		for (int i = 0; i < 10; i++) {
			float a = 3.1415 * 2 * (i / 10.0f);
			track.points.push_back({ cosf(a) * 200+300, sinf(a) * 200+300 });
		}

		track.recalculate();
		return true;
		
	}

	bool OnUserUpdate(float fElapsedTime) override
	{

		if (GetMouse(olc::Mouse::LEFT).bHeld && selected != -1) {
			track.points[selected] = GetMousePos();
			track.recalculate();
		}
		else {
			selected = -1;
			for (int i = 0; i < track.points.size(); i++) {
				auto p = track.points[i];
				auto m = GetMousePos();
				float d = sqrtf(powf(m.x - p.x, 2) + powf(m.y - p.y, 2));
				if (d < 10)
					selected = i;
			}
		}

		carDistance += fElapsedTime * 100;
		carDistance = fmodf(carDistance, track.middle.totalLength);

		Clear(olc::BLACK);
		track.fill(this);
		drawPoints(track.points);
		drawPoints(track.inner.points);
		drawPoints(track.outer.points);
		//track.drawByT(this,true);


		auto carPos = track.middle.getPointByDistance(carDistance);
		auto carDir = (track.middle.getDirByDistance(carDistance).norm())*10;
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