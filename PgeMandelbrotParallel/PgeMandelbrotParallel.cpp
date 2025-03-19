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
	// This extension will register itself to the current PGE object
	olc::TransformedView tv;

	void ResetView()
	{
		// Scale is number of pixels per 1(one) world unit
		// We want the complete circle with radius 2 to be visible, as this
		// contains the complete Mandelbrot set
		// Goes from upper left (-2, 1.5) to lower right (1.0, -1.5) in Mandelbrot world
		// Get the smallest scale to fit it all
		float scale =
			std::min<float>(ScreenWidth() / (3.0), ScreenHeight() / (3.0));
		// World y and screen y goes in opposite directions, therefore the negative scale for y
		tv.Initialise(
			{ ScreenWidth(), ScreenHeight() },
			{ scale, -scale });
		// Recalculate world offset
		tv.SetWorldOffset({ 0,0 });
		tv.SetWorldOffset(-(tv.ScreenToWorld(olc::vf2d{ (float) ScreenWidth() / 2, (float) ScreenHeight() / 2 })));
	}

	// Define a type for a drawing function
	using DrawFunction = void ();

	// Define a struct for information about a draw function
	struct DrawFunctionDescription
	{
		olc::Key commandKey;		// Key to select this calculation method
		std::string commandKeyName;	// Name of Key for display
		std::string description;	// Description of this draw function
		DrawFunction PgeMandelbrotParallel::* pDrawFunction;
									// Pointer to the draw function as a member method
	};

	// Vector of information for the draw functions to be investigated
	// Initialized at the bottom of this file
	static std::vector<DrawFunctionDescription> DrawFunctions;

	// Index of the currently selected draw function
	size_t nCurrentDrawFunctionIndex = 0;

	// The Mandelbrot algorithm
	int32_t maxCount;  // Max count for the iterative function
	const float pi = std::acos(-1.0F);
	const float pithird = pi / 3;


	int MandelbrotCount(double x, double y)
	{
		double zx = x;
		double zy = y;
		double zx2 = zx * zx;
		double zy2 = zy * zy;

		int count = 0;

		while (count < maxCount && zx2 + zy2 <= 4.0)
		{
			zy = zy * zx * 2 + y;
			zx = zx2 - zy2 + x;
			zx2 = zx * zx;
			zy2 = zy * zy;

			count++;
		}

		return count;
	}

	void DrawSingleThread()
	{
		// Current area for calculation must be calculated
		olc::vd2d worldTopLeft = tv.GetWorldOffset();
		olc::vd2d worldScale = tv.GetWorldScale();

		double xStep = 1.0/worldScale.x;
		double yStep = 1.0/worldScale.y;

		// Calculate and draw line by line
		double worldY = worldTopLeft.y;
		for (int y = 0; y < ScreenHeight(); y++)
		{
			double worldX = worldTopLeft.x;
			for (int x = 0; x < ScreenWidth(); x++)
			{
				int count = MandelbrotCount(worldX, worldY);
				olc::Pixel currPix;
				if (count >= maxCount)
					currPix = olc::BLACK;
				else
				{
					float angle = 2 * pi * count / maxCount;
					// Palette based on @Eriksonn's calculation, see my post and OneLoneCoder Discord channel
					currPix = olc::PixelF(0.5f * sin(angle) + 0.5f, 0.5f * sin(angle + 2 * pithird) + 0.5f, 0.5f * sin(angle + 4 * pithird) + 0.5f);
				}

				Draw(x, y, currPix);
				worldX += xStep;
			}
			worldY += yStep;
		}
	}



public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		ResetView();

		maxCount = 256;

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

		// START TIMING
		auto tp1 = std::chrono::high_resolution_clock::now();

		// Select the current draw function from the description table
		(this->*DrawFunctions[nCurrentDrawFunctionIndex].pDrawFunction)();

		// STOP TIMING
		auto tp2 = std::chrono::high_resolution_clock::now();
		std::chrono::duration<double> elapsedTime = tp2 - tp1;

		// Text output will be overlayed on the graphics
		uint32_t textScale = 1;
		int32_t lineDistance = 10;
		auto mousePos = GetMousePos();
		auto worldMousePos = tv.ScreenToWorld(mousePos);
		
		DrawString(0, 0,
			"Mouse x: " + std::to_string(worldMousePos.x) + ", y: " + std::to_string(worldMousePos.y), olc::WHITE, textScale);
		DrawString(0, 1*lineDistance,
			"Calculation and DrawTime: " + std::to_string(elapsedTime.count()), olc::WHITE, textScale);


		return true;
	}
};


std::vector<PgeMandelbrotParallel::DrawFunctionDescription> PgeMandelbrotParallel::DrawFunctions 
{
	{ olc::Key::K1, "1", "Single threaded drawing", &PgeMandelbrotParallel::DrawSingleThread},
};

int main()
{
	PgeMandelbrotParallel engine;
	if (engine.Construct(640*3/2, 480*3/2, 1, 1))
		engine.Start();

	return 0;
}
