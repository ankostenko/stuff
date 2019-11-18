#include <windows.h>
#include <cassert>
#include <vector>
#include <cstdint>
#include <cassert>
#include <string>
#include <malloc.h>
#include <iostream>
#include "Timer.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define MIN(a, b) (a > b? b: a)

using namespace my::Timer;

bool running = true;

struct Render_State {
	int height, width;
	void* memory;

	BITMAPINFO bitmap_info;
};
Render_State render_state;


LRESULT CALLBACK win_callback(HWND hwnd, UINT uMsg, WPARAM wparam, LPARAM lParam)
{
	LRESULT res = 0;

	switch (uMsg)
	{
		case WM_CLOSE:
		case WM_DESTROY:
		{
			running = false;
		} break;

		case WM_SIZE:
		{
			RECT rect;
			GetClientRect(hwnd, &rect);
			render_state.width = rect.right - rect.left;
			render_state.height = rect.bottom - rect.top;

			int size = render_state.width * render_state.height * sizeof(unsigned int);

			if (render_state.memory) VirtualFree(render_state.memory, 0 , MEM_RELEASE);
			render_state.memory = VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			render_state.bitmap_info.bmiHeader.biSize = sizeof(render_state.bitmap_info.bmiHeader);
			render_state.bitmap_info.bmiHeader.biWidth = render_state.width;
			render_state.bitmap_info.bmiHeader.biHeight = render_state.height;
			render_state.bitmap_info.bmiHeader.biPlanes = 1;
			render_state.bitmap_info.bmiHeader.biBitCount = 32;
			render_state.bitmap_info.bmiHeader.biCompression = BI_RGB;

		} break;

		default:
		{
			res = DefWindowProc(hwnd, uMsg, wparam, lParam);
		}
	}
	return res;
}


// game functions

#define PI 3.14159265359f

uint32_t pack_color(const uint8_t r, const uint8_t g, const uint8_t b, const uint8_t a = 255)
{
	return ((uint32_t)a << 24) + ((uint32_t)r << 16) + ((uint32_t)b << 8) + g;
}

void draw_rectangle(uint32_t* img, const size_t img_w, const size_t img_h, const size_t x, const size_t y, const size_t w, const size_t h, const uint32_t color)
{
	//assert(img.size() == img_w * img_h);
	for (size_t i = 0; i < w; i++)
	{
		for (size_t j = 0; j < h; j++) {
			size_t cx = x + i;
			size_t cy = y + j;
			assert(cx < img_w && cy < img_h);
			img[cx + cy * img_w] = color;
		}
	}
}

bool load_texture(const std::string filename, uint32_t* & texture, size_t& text_size, size_t& text_cnt) {
	int nchannels = -1, w, h;
	unsigned char* pixmap = stbi_load(filename.c_str(), &w, &h, &nchannels, 0);
	if (!pixmap) {
		std::cerr << "Error: can not load the textures" << std::endl;
		return false;
	}

	if (nchannels != 4) {
		std::cerr << "Error: the texture must be a 32 bit image" << std::endl;
		stbi_image_free(pixmap);
		return false;
	}

	text_cnt = w / h;
	text_size = w / text_cnt;
	if (w != h * int(text_cnt)) {
		std::cerr << "Error: the texture file must contain N square textures packed horizontally" << std::endl;
		stbi_image_free(pixmap);
		return false;
	}

	texture = new uint32_t[w * h];
	for (int j = 0; j < h; j++) {
		for (int i = 0; i < w; i++) {
			uint8_t r = pixmap[(i + j * w) * 4 + 0];
			uint8_t g = pixmap[(i + j * w) * 4 + 1];
			uint8_t b = pixmap[(i + j * w) * 4 + 2];
			uint8_t a = pixmap[(i + j * w) * 4 + 3];
			texture[i + j * w] = pack_color(r, g, b, a);
		}
	}
	stbi_image_free(pixmap);
	return true;
}

void texture_column(uint32_t* out_colum, const uint32_t* img, const size_t texsize, const size_t ntextures, const size_t texid, const size_t texcoord, const size_t column_height) {
	const size_t img_w = texsize * ntextures;
	const size_t img_h = texsize;
	//uint32_t* column = (uint32_t*)_alloca(sizeof(uint32_t) * column_height);
	//assert(column);
	if (!img)
		return;

	for (size_t y = 0; y < column_height; y++) {
		size_t pix_x = texid * texsize + texcoord;
		size_t pix_y = (y * texsize) / column_height;
		out_colum[y] = img[pix_x + pix_y * img_w];
	}

}


int WINAPI WinMain(HINSTANCE hInst, HINSTANCE hPiv, LPSTR args, int someshit)
{
	// create window class
	WNDCLASS window_class = {};
	window_class.style = CS_HREDRAW | CS_VREDRAW;
	window_class.lpszClassName = "Game";
	window_class.lpfnWndProc = win_callback;

	// reg window
	RegisterClass(&window_class);

	// create window
	HWND window = CreateWindow(window_class.lpszClassName, "Game!!!", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 1024, 512, 0, 0, hInst, 0);
	HDC hdc = GetDC(window);

	// game init 
	RECT rect;
	GetClientRect(window, &rect);
	const size_t win_w = rect.right - rect.left;
	const size_t win_h = rect.bottom - rect.top;

	uint32_t* framebuffer = new uint32_t[win_w * win_h]; //(win_w * win_h, pack_color(255, 255, 255)); // the image itself, initialized to red

	float player_x = 3.; // player x position
	float player_y = 3.; // player y position
	float player_a = 2.523; // player view direction

	const size_t map_w = 16; // map width
	const size_t map_h = 16; // map height
	const char map[] =
		"0000222222220000"
		"1              0"
		"1     111111   0"
		"1     0        0"
		"0     0  1110000"
		"0     3        0"
		"0   10000      0"
		"0   0   11100  0"
		"0   0   0      0"
		"0   0   1  00000"
		"0       1      0"
		"2       1      0"
		"0       0      0"
		"0 0000000      0"
		"0              0"
		"0002222222200000"; // our game map
	assert(sizeof(map) == map_w * map_h + 1); // +1 for the null terminated string

	const size_t rect_w = win_w / map_w / 2;
	const size_t rect_h = win_h / map_h;


	// column of texture
	uint32_t column[2000];


	// colors
	const size_t ncolors = 10;
	uint32_t* colors = new uint32_t[ncolors];
	for (size_t i = 0; i < ncolors; i++) {
		colors[i] = pack_color((i * 435654364 + 456765475644)% 255, (i * 5647 + 4567) % 255, (i * 7777 + 5634) % 255);
	}

	uint32_t* walltext = NULL; // textures for the walls
	size_t walltext_size;  // texture dimensions (it is a square)
	size_t walltext_cnt;   // number of different textures in the image
	if (!load_texture("walltext.png", walltext, walltext_size, walltext_cnt))
	{
		std::cerr << "Failed to load wall textures" << std::endl;
		return -1;
	}


	timer_start(FRAMELOCK);

	while (running)
	{
		// Input
		MSG msg;
		while (PeekMessage(&msg, window, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// Simulate

		for (size_t j = 0; j < map_h; j++)  // draw the map
		{
			for (size_t i = 0; i < map_w; i++)
			{
				size_t rect_x = i * rect_w;
				size_t rect_y = j * rect_h;
				if (map[i + j * map_w] != ' ')
					draw_rectangle(framebuffer, win_w, win_h, rect_x, rect_y, rect_w, rect_h, colors[map[i + j * map_w] - 48]);
				else
					draw_rectangle(framebuffer, win_w, win_h, rect_x, rect_y, rect_w, rect_h, pack_color(255, 255, 255));
			}
		}

		// draw the player on the map
		draw_rectangle(framebuffer, win_w, win_h, player_x * rect_w - 2, player_y * rect_h - 2, 5, 5, pack_color(60, 60, 60));

		// ray casting
		const float fov = PI / 3.0f; // field of view

		for (size_t i = 0; i < win_w / 2; i++) // draw the visibility cone AND the "3D" view
		{
			float angle = player_a - fov / 2 + fov * i / float(win_w / 2);

			// clear screen
			if (i == 0)
			{										       //  x          y         w            h
				draw_rectangle(framebuffer, win_w, win_h,   win_w / 2,    0,	   win_w / 2, win_h / 2, pack_color(190, 190, 190));
				draw_rectangle(framebuffer, win_w, win_h,   win_w / 2, win_h / 2,  win_w / 2, win_h / 2, pack_color(255, 255, 255));
			}

			for (float t = 0; t < 20; t += 0.032f)
			{
				float cx = player_x + t * cos(angle);
				float cy = player_y + t * sin(angle);
				//if (map[int(cx) + int(cy) * map_w] != ' ') break;

				size_t pix_x = cx * rect_w;
				size_t pix_y = cy * rect_h;
				framebuffer[pix_x + pix_y * win_w] = pack_color(255, 255, 255);
				framebuffer[pix_x + pix_y * win_w] = pack_color(160, 160, 160); // this draws the visibility cone

				if (map[int(cx) + int(cy) * map_w] != ' ') // our ray touches a wall, so draw the vertical column to create an illusion of 3D
				{
					size_t column_height = win_h / t / cos(angle - player_a);
					column_height = MIN(400, column_height);
					int texid = (int)map[int(cx) + int(cy) * map_w] - 48;
					//draw_rectangle(framebuffer, win_w, win_h, win_w / 2 + i, win_h / 2 - column_height / 2, 1, column_height, colors[map[int(cx) + int(cy) * map_w] - 48]);


					float hitx = cx - floor(cx + .5); // hitx and hity contain (signed) fractional parts of cx and cy,
					float hity = cy - floor(cy + .5); // they vary between -0.5 and +0.5, and one of them is supposed to be very close to 0
					int x_texcoord = hitx * walltext_size;
					if (std::abs(hity) > std::abs(hitx)) { // we need to determine whether we hit a "vertical" or a "horizontal" wall (w.r.t the map)
						x_texcoord = hity * walltext_size;
					}
					if (x_texcoord < 0) x_texcoord += walltext_size; // do not forget x_texcoord can be negative, fix that
					assert(x_texcoord >= 0 && x_texcoord < (int)walltext_size);


					texture_column(column, walltext, walltext_size, walltext_cnt, texid, x_texcoord, column_height);
					pix_x = win_w / 2 + i;
					for (size_t j = 0; j < column_height; j++) {
						pix_y = j + win_h / 2 - column_height / 2;
						if (pix_y < 0 || pix_y >= (int)win_h) continue;
						framebuffer[pix_x + pix_y * win_w] = column[j];
					}
					break;
				}
			}
		}

		player_a += 0.01f;
		
		render_state.memory = framebuffer;

		// Render
		StretchDIBits(hdc, 0, 0, render_state.width, render_state.height, 0, 0, render_state.width, render_state.height, render_state.memory, &render_state.bitmap_info, DIB_RGB_COLORS, SRCCOPY);


		// Timer
		timer_update();
	}

	delete[] framebuffer;
	delete[] colors;
		
	return 0;
}