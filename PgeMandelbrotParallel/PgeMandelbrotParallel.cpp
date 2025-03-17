#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

#define OLC_PGEX_TRANSFORMEDVIEW
#include "olcPGEX_TransformedView.h"

class PgeMandelbrotParallel : public olc::PixelGameEngine
{
public:
	PgeMandelbrotParallel()
	{
		sAppName = "PgeMandelbrotParallel";
	}

private:
	// This extension will register itself to the 
	olc::TransformedView tv;

	void ResetView()
	{
		// Scale is number of pixels per 1(one) world unit
		// We want the complete circle with radius 2 to be visible, as this
		// contains the complete Mandelbrot set
		// Goes from upper left (-2, 2) to lower right (2, -2) in Mandelbrot world
		// Get the smallest scale to fit it all
		float scale =
			std::min<float>(ScreenWidth() / (4.0), ScreenHeight() / (4.0));
		// World y and screen y goes in opposite directions, therefore the negative scale for y
		tv.Initialise(
			{ ScreenWidth(), ScreenHeight() },
			{ scale, -scale });
		// Recalculate world offset
		tv.SetWorldOffset({ 0,0 });
		tv.SetWorldOffset(-(tv.ScreenToWorld(olc::vf2d{ (float) ScreenWidth() / 2, (float) ScreenHeight() / 2 })));
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		ResetView();

		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// called once per frame

		// Handle zoom and pan through the transform view extension
		// This will use the scroll wheel as middle mouse button when clicked or dragged
		// and zoom on scrolling
		tv.HandlePanAndZoom(olc::Mouse::MIDDLE, 0.1F, true, true);

		// Handle other user input from keyboard and/or mouse
		// pressed, held or released can be distinguished

		if (GetKey(olc::Key::Q).bPressed
			||
			GetKey(olc::Key::ESCAPE).bPressed)
		{
			return false; // Quit immediately
		}

		if (GetKey(olc::Key::R).bPressed)
		{
			// Reset Screen-to-World transformation
			ResetView();
		}

		// Clear, even if we redraw all pixels
		Clear(olc::BLACK);

		for (float radius = 0; radius <= 20; radius++)
			tv.DrawCircle(0, 0, radius/10);

		// Text output will be overlayed on the graphics
		uint32_t textScale = 1;
		int32_t lineDistance = 10;
		auto mousePos = GetMousePos();
		auto worldMousePos = tv.ScreenToWorld(mousePos);
		
		DrawString(0, 0, 
			"Mouse x: " + std::to_string(worldMousePos.x) + ", y: " + std::to_string(worldMousePos.y), olc::WHITE, textScale);


		return true;
	}
};


int main()
{
	PgeMandelbrotParallel engine;
	if (engine.Construct(1280, 960, 1, 1))
		engine.Start();

	return 0;
}
