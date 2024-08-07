#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

typedef int cubic[3];
typedef float cubicf[3];

class Spline {
	olc::vi2d* p0;
	olc::vi2d* p1;
	olc::vi2d* p2;
	olc::vi2d* p3;

	cubic e0 = { -1,2,-1 };
	cubic e1 = { 3,-5,2 };
	cubic e2 = { -3, 4, 1 };
	cubic e3 = { 1,-1,0 };

	float evaluateEq(cubic eq, float x) {
		return eq[0] * x * x * x + eq[1] * x * x + eq[2] * x;
	}

public:
	Spline(olc::vi2d* p0, olc::vi2d* p1, olc::vi2d* p2, olc::vi2d* p3) : p0(p0), p1(p1), p2(p2), p3(p3) {}

	olc::vi2d getPointByT(float t) {
		//int x = 0;
		//x += e0[0] * powf(p0->x, 3) + e0[1] * powf(p0->x, 2) + e0[2] * p0->x;
		//x += e1[0] * powf(p1->x, 3) + e1[1] * powf(p1->x, 2) + e1[2] * p1->x;
		//x += e2[0] * powf(p2->x, 3) + e2[1] * powf(p2->x, 2) + e2[2] * p2->x;
		//x += e3[0] * powf(p3->x, 3) + e3[1] * powf(p3->x, 2) + e3[2] * p3->x;

		//int y = 0;
		//y += e0[0] * powf(p0->y, 3) + e0[1] * powf(p0->y, 2) + e0[2] * p0->y;
		//y += e1[0] * powf(p1->y, 3) + e1[1] * powf(p1->y, 2) + e1[2] * p1->y;
		//y += e2[0] * powf(p2->y, 3) + e2[1] * powf(p2->y, 2) + e2[2] * p2->y;
		//y += e3[0] * powf(p3->y, 3) + e3[1] * powf(p3->y, 2) + e3[2] * p3->y;
		//return olc::vi2d((int)x, (int)y);

		float v0 = evaluateEq(e0, t);
		float v1 = evaluateEq(e1, t);
		float v2 = evaluateEq(e2, t);
		float v3 = evaluateEq(e3, t);

		int x = v0 * p0->x + v1 * p1->x + v2 * p2->x + v3 * p3->x;
		int y = v0 * p0->y + v1 * p1->y + v2 * p2->y + v3 * p3->y;
		return { x,y };
	}
};

class Track {
	std::vector<Spline> splines;
	const std::vector<olc::vi2d> points;

public:
	Track(std::vector<olc::vi2d> points) : points(points) {
		for (int i = 0; i < points.size() - 3; i++) {
			splines.push_back(Spline(&points[i], &points[i + 1], &points[i + 2], &points[i + 3]));
		}
	}

	olc::vi2d getPosByT(float t) {
		return splines[(int)t].getPointByT(t - (int)t);
	}
};

// Override base class with your custom functionality
class Example : public olc::PixelGameEngine
{
	Track track;

public:
	Example() : 
	{
		// Name your application
		sAppName = "Example";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		return true;
		
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// Called once per frame, draws random coloured pixels
	}
};

int main()
{
	Example win;
	if (win.Construct(256, 240, 4, 4))
		win.Start();
	return 0;
}