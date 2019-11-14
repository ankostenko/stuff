#pragma once
#include <cstdint>

struct Render_State {
	int height, width;
	uint32_t* memory;

	BITMAPINFO bitmap_info;
};

struct Color
{
	union
	{
		struct { char b, g, r, a; };
		char raw[4];
		uint32_t whole;
	};

	Color(char r, char g, char b, char a = 255) : r(r), g(g), b(b) {}
};

Render_State surface;
