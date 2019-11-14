#pragma once
#include "Render_State.h"

static int clam(int point, int min, int max)
{
	if (point < min)
		return min;

	if (point > max)
		return max;

	return point;
}

inline void draw_rect_pixel(int y0, int x0, int y1, int x1, Color color)
{
	x0 = clam(x0, 0, surface.width);
	x1 = clam(x1, 0, surface.width);
	y0 = clam(y0, 0, surface.height);
	y1 = clam(y1, 0, surface.height);

	for (int i = y0; i < y1; i++)
	{
		for (int j = x0; j < x1; j++)
		{
			surface.memory[i * surface.width + j] = color.whole;
		}
	}
}

inline void draw_rect(float y, float x, float half_h, float half_w, Color color)
{
	// relative sizes to real

	x *= surface.height;
	half_w *= surface.height;
	y *= surface.height;
	half_h *= surface.height;

	x += surface.width / 2;
	y += surface.height / 2;

	int x0 = x - half_w;
	int x1 = x + half_w;
	int y0 = y - half_h;
	int y1 = y + half_h;

	
	draw_rect_pixel(y0, x0, y1, x1, color);
}