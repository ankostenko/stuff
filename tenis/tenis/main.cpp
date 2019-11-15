#include <windows.h>

#include "Render_State.h"
#include "draw.h"
#include "input.h"
#include "timer.h"


#define VK_W 0x57
#define VK_S 0x53

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

		// clear screen / too much costs
		//draw_rect(0, 0, 1, 1, Color(0, 0, 0));

		// draw fiel7
		draw_rect(0, 0, 0.46, 0.75, Color(255, 150, 0));

		// Simulate
		static float p1_pos_y = 0;

		if (input.is_pressed[P1_UP])
			p1_pos_y += .001;

		if (input.is_pressed[P1_DOWN])
			p1_pos_y -= .001;

		
		static float p2_pos_y = 0;

		if (input.is_pressed[P2_UP])
			p2_pos_y += .001;

		if (input.is_pressed[P2_DOWN])
			p2_pos_y -= .001;




		draw_rect( 0 + p1_pos_y, 0.4, .1, .1, Color(255, 0, 0));
		draw_rect(0 + p2_pos_y, -0.4, .1, .1, Color(255, 0, 0));


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