#include  <Windows.h>
#include <cstdint>

struct Render_State {
	int height, width;
	uint32_t* memory;

	BITMAPINFO bitmap_info;
};

Render_State surface;

inline void relative_coords(int x, int y, int& new_x, int& new_y)
{
	new_x = x * surface.height;
	//half_w *= surface.height;
	new_y = y *= surface.height;
	//half_h *= surface.height;

	new_x = x + surface.width / 2;
	new_y = y + surface.height / 2;
}

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
