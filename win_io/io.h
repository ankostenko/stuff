#pragma once
#include <Windows.h>
#include <tchar.h>
#include <stdio.h>


// Data d;
// d = open_file("file.txt");
// 
// // out data to console
// for (int i = 0; i < d.size; i++)
//	printf("%c", d[i])
//
// write_file("file1.txt", d);



struct Data
{
	char* data{NULL};
	size_t size{0};

	char& operator [] (size_t i)
	{
		return data[i];
	}
};

inline Data open_file(char const* file_name)
{
	Data data;

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
		return Data();
	}

	data.size = GetFileSize(hFile, NULL);
	DWORD nBytesRead;
	BOOL bResult;
	char* data = new char[data.size];

	bResult = ReadFile(hFile, data.data, data.size, &nBytesRead, NULL);

	if (!bResult)
	{
		printf("Read failed");
		return Data();
	}

	return data;
}

// size in bytes
inline void write_file(char const* file_name, Data data)
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
		data.data,                // ????? ??????
		data.size,				// ????? ?????? ??? ??????
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