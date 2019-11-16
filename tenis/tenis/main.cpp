#include <windows.h>

#include "Render_State.h"
#include "draw.h"
#include "input.h"
#include "timer.h"

#define PI 3.14159265359

#define VK_W 0x57
#define VK_S 0x53

struct Ball
{
	float pos_x{0};
	float pos_y{0};
	float angle{0};

	float new_pos_x{0};
	float new_pos_y{0};

	float speed_x{0.6};
	float speed_y{0};
	float speed_dif{0.015f};

	float w{0.01f};
	float h{0.01f};

	void start_position()
	{
		angle = 0;
		pos_x = new_pos_x = 0;
		pos_y = new_pos_y = 0;
		speed_x = 0.6;
		speed_y = 0;
	}
};

bool running = true;


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
			surface.width = rect.right - rect.left;
			surface.height = rect.bottom - rect.top;

			int size = surface.width * surface.height * sizeof(unsigned int);

			if (surface.memory) VirtualFree(surface.memory, 0 , MEM_RELEASE);
			surface.memory = (uint32_t*)VirtualAlloc(0, size, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);

			surface.bitmap_info.bmiHeader.biSize = sizeof(surface.bitmap_info.bmiHeader);
			surface.bitmap_info.bmiHeader.biWidth = surface.width;
			surface.bitmap_info.bmiHeader.biHeight = surface.height;
			surface.bitmap_info.bmiHeader.biPlanes = 1;
			surface.bitmap_info.bmiHeader.biBitCount = 32;
			surface.bitmap_info.bmiHeader.biCompression = BI_RGB;

		} break;

		default:
		{
			res = DefWindowProc(hwnd, uMsg, wparam, lParam);
		}
	}
	return res;
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
	HWND window = CreateWindow(window_class.lpszClassName, "Game!!!", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 900, 600, 0, 0, hInst, 0);
	HDC hdc = GetDC(window);


	// Game vars --------------------------------
	// arena size
	float arena_h = 0.46f;
	float arena_w = 0.77f;

	// player size
	float player_w = 0.015f;
	float player_h = 0.13f;

	// speed
	float max_speed = 5;
	float p1_speed = 0;
	float p2_speed = 0;
	float speed_dif = 0.02f;

	// position
	const float p1_pos_x = 0.74f;
	const float p2_pos_x = -p1_pos_x;

	float p1_pos_y = 0;
	float p2_pos_y = 0;

	float p1_pos_new = 0;
	float p2_pos_new = 0;

	// ball
	Ball ball;

	// points
	int p1_points = 0;
	int p2_points = 0;


	// Game Loop --------------------------------
	Input input;
	Timer timer;

	while (running)
	{

		// Input
		MSG msg;
		while (PeekMessage(&msg, window, 0, 0, PM_REMOVE))
		{
			switch (msg.message)
				{
					case WM_KEYDOWN:
					case WM_KEYUP:
					{
						uint32_t vk_code = (uint32_t)msg.wParam;
						bool is_down = ((msg.lParam & (1 << 31)) == 0);
						switch (vk_code)
						{
							case VK_UP:
							{
								input.change[P1_UP] = is_down != input.is_down[P1_UP];
								input.is_down[P1_UP] = is_down;
								input.is_pressed[P1_UP] = (!input.change[P1_UP] && input.is_down[P1_UP]) ||
														  (input.change[P1_UP] && input.is_down[P1_UP]);
							} break;
							case VK_DOWN:
							{
								input.change[P1_DOWN] = is_down != input.is_down[P1_DOWN];
								input.is_down[P1_DOWN] = is_down;
								input.is_pressed[P1_DOWN] = (!input.change[P1_DOWN] && input.is_down[P1_DOWN]) ||
															(input.change[P1_DOWN] && input.is_down[P1_DOWN]);
							} break;

							case VK_W:
							{
								input.change[P2_UP] = is_down != input.is_down[P2_UP];
								input.is_down[P2_UP] = is_down;
								input.is_pressed[P2_UP] = (!input.change[P2_UP] && input.is_down[P2_UP]) ||
									(input.change[P2_UP] && input.is_down[P2_UP]);
							} break;
							case VK_S:
							{
								input.change[P2_DOWN] = is_down != input.is_down[P2_DOWN];
								input.is_down[P2_DOWN] = is_down;
								input.is_pressed[P2_DOWN] = (!input.change[P2_DOWN] && input.is_down[P2_DOWN]) ||
									(input.change[P2_DOWN] && input.is_down[P2_DOWN]);
							} break;
						}
					}
				default:
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}

;

		}

		// Simulate -----------------------------------

		// speed p1
		if (input.is_pressed[P1_UP])
			p1_speed = p1_speed > max_speed ? p1_speed : p1_speed + speed_dif;
		else if (input.is_pressed[P1_DOWN])
			p1_speed = p1_speed < -max_speed ? p1_speed : p1_speed - speed_dif;
		else
			if (fabs(p1_speed) > 0.01f)
				p1_speed = p1_speed > 0 ? p1_speed - speed_dif : p1_speed + speed_dif;
			else
				p1_speed = 0;

		// speed p2
		if (input.is_pressed[P2_UP])
			p2_speed += speed_dif;
		else if (input.is_pressed[P2_DOWN])
			p2_speed -= speed_dif;
		else
			if (fabs(p2_speed) > 0.01f)
				p2_speed = p2_speed > 0 ? p2_speed - speed_dif : p2_speed + speed_dif;
			else
				p2_speed = 0;


		p1_pos_new = p1_pos_y + p1_speed * timer.elapsed;
		p2_pos_new = p2_pos_y + p2_speed * timer.elapsed;

		// collission detection
		if (fabs(p1_pos_new) < arena_h - player_h)
			p1_pos_y = p1_pos_new;
		else
			p1_speed *= -1. / 4;


		if (fabs(p2_pos_new) < arena_h - player_h)
			p2_pos_y = p2_pos_new;
		else
			p2_speed *= -1. / 4;


		// ball
		// new pos
		ball.new_pos_x += ball.speed_x * timer.elapsed;
		ball.new_pos_y += ball.speed_y * timer.elapsed;

		// ball: wall collission detection
		if (fabs(ball.new_pos_y) > arena_h - ball.h)
			ball.speed_y *= -1;
		else
		{
			ball.pos_x = ball.new_pos_x;
			ball.pos_y = ball.new_pos_y;
		}

		// ball: player collision detection
		if (p1_pos_x - ball.new_pos_x < ball.w + player_w && fabs(p1_pos_y - ball.new_pos_y) < ball.h + player_h)
		{
			ball.speed_x *= -1;
			ball.speed_y += p1_speed / 4;
		}

		if (ball.new_pos_x - p2_pos_x < ball.w + player_w && fabs(ball.new_pos_y - p2_pos_y) < ball.h + player_h)
		{
			ball.speed_x *= -1;
			ball.speed_y += p2_speed / 4;
		}


		// Point handler
		if (ball.new_pos_x > arena_w)
		{
			ball.start_position();
			p2_points++;
		}
		
		if (ball.new_pos_x < -arena_w)
		{
			ball.start_position();
			p1_points++;
		}
		 

		// Draw ----------------------------------------

		// clear screen / too much costs
		draw_rect(0, 0, 1, 1, Color(70, 50, 255));

		// draw arena
		draw_rect(0, 0, arena_h, arena_w, Color(255, 150, 0));


		// draw players
		draw_rect(p1_pos_y, p1_pos_x, player_h, player_w, Color(255, 0, 0));
		draw_rect(p2_pos_y, p2_pos_x, player_h, player_w, Color(255, 0, 0));

		// draw ball
		draw_rect(ball.pos_y, ball.pos_x, ball.h, ball.w, Color(255, 0, 255));

		// draw points
		for (int i = 0; i < p1_points; i++)
			draw_rect(0.48, 0.75 - i * 0.03, 0.01, 0.01, Color(255, 0, 0));

		for (int i = 0; i < p2_points; i++)
			draw_rect(0.48, -0.75 + i * 0.03, 0.01, 0.01, Color(255, 200, 0));

		// Render
		StretchDIBits(hdc, 0, 0, surface.width, surface.height, 0, 0, surface.width, surface.height, surface.memory, &surface.bitmap_info, DIB_RGB_COLORS, SRCCOPY);

		// Timer
		timer.update();

		// out fps to debug output
		char out[10];
		sprintf_s(out, "%d\n", timer.FPS);
		OutputDebugString(out);
	}
		
	return 0;
}