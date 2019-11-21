#pragma once
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>


inline char* open_file(char const* file_name , int& size)
{
	HANDLE hFile;

	hFile = CreateFile(file_name,
		GENERIC_READ,
		0,
		NULL,
		OPEN_EXISTING,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		_tprintf(TEXT("hFile is NULL\n"));
		_tprintf(TEXT("Target file is %s\n"),
			file_name);
		return  NULL;
	}

	size = GetFileSize(hFile, NULL);
	DWORD nBytesRead;
	BOOL bResult;
	char* data = new char[size];

	bResult = ReadFile(hFile, data, size, &nBytesRead, NULL);

	if (!bResult)
	{
		printf("Read failed");
		return NULL;
	}

	return data;
}

// size in bytes
inline void write_file(char const* file_name, char* data, int size)
{
	HANDLE hFile;

	hFile = CreateFile(file_name,
		GENERIC_WRITE,
		0,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);

	if (hFile == INVALID_HANDLE_VALUE)
	{
		_tprintf(TEXT("hFile is NULL\n"));
		_tprintf(TEXT("Target file is %s\n"),
			file_name);
		return;
	}

	//*out_size = GetFileSize(hFile, NULL);
	DWORD nBytesWrited;
	BOOL bResult;
	//char* data = new char[*out_size];


	bResult = WriteFile(
		hFile,                    // ?????????? ?????
		data,                // ????? ??????
		size,				// ????? ?????? ??? ??????
		&nBytesWrited,		// ????? ?????????? ??????
		NULL				// ??????????? ?????
	);
	//bResult = ReadFile(hFile, data, *out_size, &nBytesRead, NULL);

	if (!bResult)
	{
		printf("Write failed");
		return;
	}
}