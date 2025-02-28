/*
This code is:

Copyright 2018 - 2024 OneLoneCoder.com
Copyright 2024 - 2025 Frank B. Jakobsen
Color palette courtesy of @Eriksonn

It is released under a fairly free usage license, the OLC 3, which is fully described at the end of this code file

The application is using the Pixel Game Engine - PGE - created by One Lone Coder
aka javidx9, and David Barr in his job life.

Links and references at the bottom in the license section.

Have fun! That's the main purpose for my activities on Excursions With C++ as well.

*/

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
			// Get mouse position and move the rectangle there
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
	std::cout << "Color palette courtesy of @Eriksonn" << std::endl;
	if (demo.Construct(640, 480, 2, 2, false, false, false, false))
		demo.Start();

	return 0;
}

/*
* 
License(OLC - 3)
~~~~~~~~~~~~~~~

Copyright 2018 - 2024 OneLoneCoder.com
Copyright 2024 - 2025 Frank B. Jakobsen
Color palette courtesy of @Eriksonn

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met :

1. Redistributions or derivations of source code must retain the above copyright
notice, this list of conditions and the following disclaimer.

2. Redistributions or derivative works in binary form must reproduce the above
copyright notice.This list of conditions and the following	disclaimer must be
reproduced in the documentation and /or other materials provided with the distribution.

3. Neither the name of the copyright holder nor the names of its contributors may
be used to endorse or promote products derived from this software without specific
prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS	"AS IS" AND ANY
EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.IN NO EVENT
SHALL THE COPYRIGHT	HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES(INCLUDING, BUT NOT LIMITED
TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR
BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
CONTRACT, STRICT LIABILITY, OR TORT(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN
ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
SUCH DAMAGE.

	Links for OneLoneCoder
	~~~~~
	YouTube:	https://www.youtube.com/javidx9
	https://www.youtube.com/javidx9extra
	Discord:	https://discord.gg/WhwHUMV
	Twitter:	https://www.twitter.com/javidx9
	Twitch:		https://www.twitch.tv/javidx9
	GitHub:		https://www.github.com/onelonecoder
	Homepage:	https://www.onelonecoder.com
	Patreon:	https://www.patreon.com/javidx9
	Community:  https://community.onelonecoder.com

	Javidx9's YouTube video on MandelBrot: https://www.youtube.com/watch?v=PBvLs88hvJ8

	Links for Frank B. Jakobsen
	~~~~~
	Excursions with C++ blog: https://excursionswithcpp.com/
	Github repository for this code: https://github.com/excursionswithcpp/PgeIterativeProjects

*/