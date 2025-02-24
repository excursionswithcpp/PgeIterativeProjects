#define OLC_PGE_APPLICATION
#include "olcPixelGameEngine.h"

class PgeMandelbrot1 : public olc::PixelGameEngine
{
public:
	PgeMandelbrot1()
	{
		sAppName = "PgeMandelbrot1";
	}

public:
	bool OnUserCreate() override
	{
		// Called once at the start, so create things here
		// Setup mapping between pixels to complex number space
		// Make room for the 2 x 2 area
		worldScale = 4.0 / std::max(ScreenWidth(), ScreenHeight());
		// Make sure (0.0 , 0.0 ) is in the middle of the screen
		worldOffset = {
			-worldScale * ScreenWidth() / 2,
			worldScale * ScreenHeight() / 2
		};
		maxCount = 256;
		return true;
	}

	bool OnUserUpdate(float fElapsedTime) override
	{
		// called once per frame
		// Poll keyboard input, there are predefined consts for allmost all keys
		// Can also be checked for being held or released
		if (GetKey(olc::Key::Q).bPressed)
		{
			return false; // Quit immediately
		}

		if (GetKey(olc::Key::R).bPressed)
		{
			// Reset Screen-to-World transformation
			// Make room for the 2 x 2 area
			worldScale = 4.0 / std::max(ScreenWidth(), ScreenHeight());
			// Make sure (0.0 , 0.0 ) is in the middle of the screen
			worldOffset = {
				-worldScale * ScreenWidth() / 2,
				worldScale * ScreenHeight() / 2
			};
			maxCount = 256;
		}

		// Move zoom rectangle
		if (GetKey(olc::Key::RIGHT).bPressed)
		{
			worldOffset.x += worldScale * ScreenWidth() * 0.1;
		}
		if (GetKey(olc::Key::LEFT).bPressed)
		{
			worldOffset.x -= worldScale * ScreenWidth() * 0.1;
		}
		if (GetKey(olc::Key::UP).bPressed)
		{
			worldOffset.y += worldScale * ScreenHeight() * 0.1;
		}
		if (GetKey(olc::Key::DOWN).bPressed)
		{
			// Screen coordinates have increasing Y going down
			worldOffset.y -= worldScale * ScreenHeight() * 0.1;
		}

		// Poll mouse clicks, buttons are LEFT, MIDDLE, RIGHT: 1, 2, 3
		// Can also be checked for being held or released
		if (GetMouse(olc::Mouse::LEFT).bPressed)
		{
			// Get mouse position as it was at the start of the frame update and move the rectangle there
			olc::vi2d mouse = GetMousePos();
			double x = worldOffset.x + mouse.x * worldScale;
			double y = worldOffset.y - mouse.y * worldScale;

			worldOffset = { x,y };
		}

		// Poll mouse wheel
		if (GetMouseWheel() > 0)
		{
			// Zoom in
			worldScale *= 0.9;
		}
		else if (GetMouseWheel() < 0)
		{
			// Zoom out
			worldScale *= 1.1;
		}

		// Clearing is not necessary in this example, since we redraw all pixels
		// But this is how it's done
		Clear(olc::BLACK);

		// Screen y grows going down
		olc::vi2d pixelTopLeft{ 0,0 };
		olc::vi2d pixelBottomRight{ ScreenWidth(), ScreenHeight() };

		// Current world view, should be calculated
		olc::vd2d worldTopLeft = worldOffset;
		olc::vd2d worldBottomRight;
		worldBottomRight.x = worldOffset.x + worldScale * ScreenWidth();
		worldBottomRight.y = worldOffset.y - worldScale * ScreenHeight();

		double xStep = (worldBottomRight.x - worldTopLeft.x) / ScreenWidth();
		double yStep = (worldBottomRight.y - worldTopLeft.y) / ScreenHeight();

		// Calculate and draw line by line
		double worldY = worldTopLeft.y;
		for (int y = pixelTopLeft.y; y < pixelBottomRight.y; y++)
		{
			double worldX = worldTopLeft.x;
			for (int x = 0; x < ScreenWidth(); x++)
			{
				int count = MandelbrotCount(worldX, worldY);
				olc::Pixel currPix(count % 255, count % 255, count % 255);
				if (count >= maxCount)
					currPix = olc::BLACK;
				else
				{
					float angle = 2 * pi * count / maxCount;
					// Palette based on @Eriksonn's calculation, see my post and OneLoneCoder Discord channel
					currPix = olc::PixelF(0.5f * sin(angle) + 0.5f, 0.5f * sin(angle + 2*pithird) + 0.5f, 0.5f * sin(angle + 4 * pithird) + 0.5f);
				}

				Draw(x, y, currPix);
				worldX += xStep;
			}
			worldY += yStep;
		}

		return true;
	}

private:
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

	olc::vd2d worldOffset;
	double worldScale;

	int maxCount;

	const float pi = 3.141593f;
	const float pithird = pi / 3;
};


int main()
{
	PgeMandelbrot1 demo;

	std::cout << "Copyright 2018 - 2024 OneLoneCoder.com" << std::endl;
	std::cout << "Copyright 2024 - Frank B. Jakobsen" << std::endl;
	if (demo.Construct(640, 480, 2, 2, false, false, false, false))
		demo.Start();

	return 0;
}
