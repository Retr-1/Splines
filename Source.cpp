#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class Spline {
	std::vector<olc::vi2d> points;

public:
	olc::vi2d getPointByT(float t) {
		
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