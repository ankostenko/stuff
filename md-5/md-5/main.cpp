#include <stdio.h>
#include <string.h>
#include "file.h"
#include "md5.h"


int main(int argc, char* argv[])
{
	char* data = NULL;

	//if (argc)
		//data = open_file(argv[0]);

	int size;
	
	data = open_file("1.png", &size);

	uint32_t* res = nullptr;

	if (data)
		res = md5(data, size);


	printf("%x %x %x %x", res[0], res[1], res[2], res[3]);
	

	char str[] = "abc";
	res = md5(str, sizeof(str));
	printf("\n%x %x %x %x", res[0], res[1], res[2], res[3]);


	delete[] res;
	delete[] data;

	return 0;
}