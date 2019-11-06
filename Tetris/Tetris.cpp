#define _CRT_NON_CONFORMING_SWPRINTFS
#define _CRT_SECURE_NO_WARNINGS
#include <Windows.h> 
#include <stdio.h>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <string>
#include <cstdint>
#include <memory.h>
#include "Timer.h"


int nWindowHeight = 0;
int nWindowWidht = 0;

int nFieldWidth = 12;
int nFieldHeight = 18;
char* pField = nullptr;

std::wstring figures[7];


// game field top left position

int nFieldPosX = 0;
int nFieldPosY = 0;


// game logic vars

int nCurFigure = 0;
int nRotation = 0;
int nCurPosX = 0;
int nCurPosY = 0;
int nLvl = 0;   // relation (FallingTiming = second / nSpeed)  if nSpeed = 2, figure will fall every 0.5 sec
int nTotalScore = 0;

bool bFilledRowFlag = false;


int Rotate(int nPosX, int nCurY, int nCurRotation) // return new pos in figure's string
{
	int nNewPos = 0;
	switch (nCurRotation % 4)
	{
	case 0:
	{
		nNewPos = nCurY * 4 + nPosX;
		break;
	}
	case 1:
	{
		nNewPos = 12 - (4 * nPosX) + nCurY;
		break;
	}
	case 2:
	{
		nNewPos = 15 - nPosX - (4 * nCurY);
		break;
	}
	case 3:
	{
		nNewPos = 3 + (4 * nPosX) - nCurY;
		break;
	}
	}

	return nNewPos;
}

bool CollisionDetection(int nFigureNum, int nRotation, int nPosX, int nPosY) // false if have collision
{
	for (int y = 0; y < 4; y++)
	{
		for (int x = 0; x < 4; x++)
		{
			if (figures[nCurFigure][Rotate(x, y, nRotation)] != L'.' && pField[(y + nPosY - nFieldPosY) * nFieldWidth + nPosX - nFieldPosX + x] != 0)
				return false;
		}
	}

	return true;
}

int main(void)
{

	// set window size
	CONSOLE_SCREEN_BUFFER_INFO csbi;
	GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
	nWindowHeight = csbi.srWindow.Bottom + 1;
	nWindowWidht = csbi.srWindow.Right + 1;

	// screen
	wchar_t* pScreen = new wchar_t[nWindowHeight * nWindowWidht];
	for (int i = 0; i < nWindowHeight * nWindowWidht; i++) pScreen[i] = ' ';
	HANDLE hConsole = CreateConsoleScreenBuffer(GENERIC_READ | GENERIC_WRITE, 0, NULL, CONSOLE_TEXTMODE_BUFFER, NULL);
	SetConsoleActiveScreenBuffer(hConsole);
	DWORD dwBytesWitten = 0;

	// set font size
	CONSOLE_FONT_INFOEX cfi;
	cfi.cbSize = sizeof(cfi);
	cfi.nFont = 0;
	cfi.dwFontSize.X = 12;
	cfi.dwFontSize.Y = 18;
	cfi.FontFamily = FF_DONTCARE;
	cfi.FontWeight = FW_NORMAL;
	std::wcscpy(cfi.FaceName, L"Consolas"); // Choose your font

	SetCurrentConsoleFontEx(hConsole, false, &cfi);




	// define Field position on screen
	nFieldPosX = nWindowWidht / 2 - 12;
	nFieldPosY = 5;

	// input keys
	bool bKey[4];
	bool bKeyTrace[4];

	// frames and timing
	bool bFrameLock = false;
	uint64_t llFrameTime = 0;
	uint64_t llFrameTimeSum = 0;
	float fAvarangeFrameTime = 0;
	int nFramesPerSecond = 0;
	int nFrames= 0;

	// helpfull timings
	int64_t llTimeShowKeyInput = 1500000000; // 1.5 sec for showing Keys
	int64_t llFallingTimeCount = 0;
	
	int nTimeFilledRow = 300000000;
	// figures

	figures[0].append(L"..X.");
	figures[0].append(L"..X.");
	figures[0].append(L"..X.");
	figures[0].append(L"..X.");

	figures[1].append(L"..X.");
	figures[1].append(L".XX.");
	figures[1].append(L".X..");
	figures[1].append(L"....");

	figures[2].append(L".XX.");
	figures[2].append(L".XX.");
	figures[2].append(L"....");
	figures[2].append(L"....");

	figures[3].append(L"..X.");
	figures[3].append(L".XX.");
	figures[3].append(L"..X.");
	figures[3].append(L"....");

	figures[4].append(L".XX.");
	figures[4].append(L".X..");
	figures[4].append(L".X..");
	figures[4].append(L"....");

	figures[5].append(L".XX.");
	figures[5].append(L"..X.");
	figures[5].append(L"..X.");
	figures[5].append(L"....");

	figures[6].append(L".X..");
	figures[6].append(L".XX.");
	figures[6].append(L"..X.");
	figures[6].append(L"....");

	// field initializing
	pField = new char[nFieldHeight * nFieldWidth];

	for (int x = 0; x < nFieldWidth; x++)
		for (int y = 0; y < nFieldHeight; y++)
				pField[y * nFieldWidth + x] = (x == 0 || x == nFieldWidth - 1 || y == nFieldHeight - 1) ? 9 : 0;

	// key input init
	for (int i = 0; i < 4; i++)
	{
		bKey[i] = false;
		bKeyTrace[i] = true;
	}

	// game logic define vars
	nLvl = 1;
	nRotation = 0;
	nCurPosX = nFieldPosX + 4;  // center of field
	nCurPosY = nFieldPosY - 1;
	nCurFigure = my::now() % 7; // instead rand()

	my::timer::startTimer();

	while (1)
	{
	
		// Key input =======================

		for (int i = 0; i < 4; i++)
		{
			bKey[i] = (0x0001 & (GetAsyncKeyState("\x25\x28\x27\x20"[i]))) != 0; // keys was pressed
		}

		// Describing game logic ===========

		nCurPosX -= (bKey[0] && CollisionDetection(nCurFigure, nRotation, nCurPosX - 1, nCurPosY)) ? 1 : 0;
		nCurPosX += (bKey[2] && CollisionDetection(nCurFigure, nRotation, nCurPosX + 1, nCurPosY)) ? 1 : 0;
		nCurPosY += (bKey[1] && CollisionDetection(nCurFigure, nRotation, nCurPosX, nCurPosY + 1)) ? 1 : 0;
		nRotation += (bKey[3] && CollisionDetection(nCurFigure, nRotation + 1, nCurPosX, nCurPosY)) ? 1 : 0;

		if (llFallingTimeCount <= 0)
		{
			llFallingTimeCount = 300000000 / nLvl;
			
			if (CollisionDetection(nCurFigure, nRotation, nCurPosX, nCurPosY + 1))
				nCurPosY++;
			else
			{
				if (nCurPosY == nFieldPosY)
				{
					swprintf(&pScreen[(nFieldPosY + 10) * nWindowWidht + nFieldWidth + nFieldPosX + 2], L"game over");
					break;
				}

				for (int y = 0; y < 4; y++) {
					for (int x = 0; x < 4; x++)
						if (figures[nCurFigure][Rotate(x, y, nRotation)] != L'.')
							pField[(nCurPosY - nFieldPosY + y) * nFieldWidth + nCurPosX - nFieldPosX + x] = nCurFigure + 1;
				}

				// filled row situation
				for (int y = 0; y < 4; y++)
				{
					for (int x = 1; x < nFieldWidth; x++)
					{
						if (pField[(y + nCurPosY - nFieldPosY) * nFieldWidth + x] == 0 || y + nCurPosY - nFieldPosY > nFieldHeight - 2)
							break;
						if (x == nFieldWidth - 1)
						{
							for (int i = 1; i < nFieldWidth - 1; i++)
								pField[(y + nCurPosY - nFieldPosY) * nFieldWidth + i] = 8;

							nTotalScore += 100;
							bFilledRowFlag = true;
						}
					}
				}

				nCurPosX = nFieldPosX + 4;
				nCurPosY = nFieldPosY;
				nCurFigure = my::now() % 7; 
				nTotalScore += 25;
				nLvl = nTotalScore / 500 == 0? 1 : nTotalScore / 500;
			}
		}
		else
			llFallingTimeCount -= llFrameTime;


		// clean filled row
		if (bFilledRowFlag)
		{
			if (nTimeFilledRow < 0)
			{
				nTimeFilledRow = 1000000000;

				for (int i = nFieldHeight; i > 0; i--)
				{
					if (pField[i * nFieldWidth + 1] == 8)
					{
						char* dest = &pField[i * nFieldWidth];
						for (int j = i; j > 0; j--)
						{
							memmove(dest, &pField[(j - 1) * nFieldWidth], nFieldWidth);
							dest = &pField[(j - 1) * nFieldWidth];
						}
					}
				}
			}
			else
				nTimeFilledRow -= llFrameTime;
		}

		// Draw ============================

		for (int x = 0; x < nFieldWidth; x++)
			for (int y = 0; y < nFieldHeight; y++)
				pScreen[(y + nFieldPosY) * nWindowWidht + x + nFieldPosX] = L" ABCDEFG=#"[pField[y * nFieldWidth + x]];


		// Draw figure

		for (int y = 0; y < 4; y++)
			for (int x = 0; x < 4; x++)
				if (figures[nCurFigure][Rotate(x, y, nRotation)] != L'.')
				pScreen[(nCurPosY + y) * nWindowWidht + nCurPosX + x] = nCurFigure + 65;

		// Draw game info ===================

		swprintf(&pScreen[nFieldPosY * nWindowWidht + nFieldWidth + nFieldPosX + 2], 
				L"FPS: %d  Frame_Time: %0.1f ms  FPS_LOCK: %s",
					nFramesPerSecond, fAvarangeFrameTime, bFrameLock ? L"YES": L"NO");

		swprintf(&pScreen[(nFieldPosY + 2) * nWindowWidht + nFieldWidth + nFieldPosX + 2],
			L"Score: %d", nTotalScore);

		if (llTimeShowKeyInput > 0)
		{
			llTimeShowKeyInput -= llFrameTime;
			for (int i = 0; i < 4; i++)
				if (bKey[i])
					bKeyTrace[i] = bKey[i];
		}
		else
		{
			llTimeShowKeyInput = 1000000000;
			for (int i = 0; i < 4; i++)
				bKeyTrace[i] = false;
		}

		swprintf(&pScreen[(nFieldPosY + 5) * nWindowWidht + nFieldWidth + nFieldPosX + 2], L"Pressed keys: %s %s %s %s",
			bKeyTrace[0] ? L"<-" : L"  ", bKeyTrace[1] ? L"down" : L"    ", bKeyTrace[2] ? L"->" : L"  ", bKeyTrace[3] ? L"space" : L"     ");


		WriteConsoleOutputCharacter(hConsole, pScreen, nWindowHeight * nWindowWidht, {0, 0}, &dwBytesWitten);

		// Frames ==========================

		llFrameTime = my::timer::passed_time(); // time passed after last call
		llFrameTimeSum += llFrameTime;
		nFrames++;

		if (bFrameLock && llFrameTime < 16700000) // 16.7 ms
		{
			int nDif = 16700000 - llFrameTime;
			llFrameTime = 16700000;
			llFrameTimeSum += nDif;
			std::this_thread::sleep_for(std::chrono::nanoseconds(nDif));
		}

		if (llFrameTimeSum >= 1e9) // one sec
		{
			nFramesPerSecond = nFrames;
			fAvarangeFrameTime = (float)llFrameTimeSum / nFramesPerSecond / 1000000;
			nFrames = 0;
			llFrameTimeSum = 0;
		}

	}

	WriteConsoleOutputCharacter(hConsole, pScreen, nWindowHeight* nWindowWidht, { 0, 0 }, & dwBytesWitten);

	Sleep(1000);

	return 0;
}