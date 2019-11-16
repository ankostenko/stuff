#include <windows.h>
#include <array>

bool running = true;

#include "render_state.cpp"
#include "geometry.cpp"
#include "draw.cpp"
#include "timer.cpp"
#include "mouse_input.cpp"
#include "line.cpp"


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

		if (surface.memory) VirtualFree(surface.memory, 0, MEM_RELEASE);
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
	HWND window = CreateWindow(window_class.lpszClassName, "Game", WS_OVERLAPPEDWINDOW | WS_VISIBLE, CW_USEDEFAULT, CW_USEDEFAULT, 1280, 720, 0, 0, hInst, 0);
	HDC hdc = GetDC(window);

	// shapes
	/*std::array<Line, 10> lines
	{
		Line(Vert2i(0, 0), Vec2i(1,0), surface.width - 1),

	};*/

	Line line(Vert2f(0, 0), Vec2f(1, 0).normalize(), surface.width - 1);
	Line line1(Vert2f(0, 0), Vec2f(0, 1).normalize(), surface.height - 1);
	Line line2(Vert2f(surface.width - 1, 0), Vec2f(0, 1).normalize(), surface.height - 1);
	Line line3(Vert2f(0, surface.height - 1), Vec2f(1, 0).normalize(), surface.width - 1);



	// mouse input
	Mouse_Input mouse_input;

	// timer
	Timer timer(true);
	while (running)
	{
		// Input
		MSG msg;
		while (PeekMessage(&msg, window, 0, 0, PM_REMOVE))
		{
			switch (msg.message)
			{
				case WM_MOUSEMOVE:
				{
					mouse_input.x = int(LOWORD(msg.lParam));
					mouse_input.y = surface.height - int(HIWORD(msg.lParam));
				}break;
				case WM_LBUTTONDOWN:
				{
					OutputDebugString("mouse down\n");
					mouse_input.buttons[LBUTTON].change = !mouse_input.buttons[LBUTTON].is_dawn;
					mouse_input.buttons[LBUTTON].is_dawn = true;
				}break;
				case WM_LBUTTONUP:
				{
					OutputDebugString("mouse relised\n");
					mouse_input.buttons[LBUTTON].change = mouse_input.buttons[LBUTTON].is_dawn;
					mouse_input.buttons[LBUTTON].is_dawn = false;
				}break;
				default:
				{
					TranslateMessage(&msg);
					DispatchMessage(&msg);
				}
			}
		}

		// Simulate -----------------------------------------------------------
		


		// Draw ---------------------------------------------------------------
		
		// clear screen
		draw_filled_rect(0, 0, surface.width, surface.height, Color(0, 0, 0));

		// draw field rect
		//drawLine(0, 0, surface.width-1, 0, Color(255, 255, 255));
		//drawLine(0, 0, 0, surface.height-1, Color(255, 255, 255));
		//drawLine(0, surface.height-1, surface.width-1, surface.height-1, Color(255, 255, 255));
		//drawLine(surface.width-1, 0, surface.width-1, surface.height-1, Color(255, 255, 255));

		// draw some shapes


		line.draw(Color(255, 255, 255));
		line1.draw(Color(255, 255, 255));
		line2.draw(Color(255, 255, 255));
		line3.draw(Color(255, 255, 255));

		// draw mouse point
		draw_filled_circle(mouse_input.x, mouse_input.y, 5, Color(255, 0, 0));


		// timer
		timer.update();

		// Render
		StretchDIBits(hdc, 0, 0, surface.width, surface.height, 0, 0, surface.width, surface.height, surface.memory, &surface.bitmap_info, DIB_RGB_COLORS, SRCCOPY);

	}

	return 0;
}