#pragma once
#include <stdio.h>
#include <memory.h>
#include <stdint.h>
#include <math.h>

// cycle bit shift
#define S(x, n) (((x) << n) | ((x) >> (32 - n)))

static char* copy(char* original, uint32_t size, uint32_t* out_new_size)
{
	int rem = size % 64;
	uint32_t new_size = size;

	// for adding bits
	if (rem > 56)
		new_size += 120 - rem;
	else if (rem == 56)
		new_size += 64;
	else
		new_size += 56 - rem;

	// for data size
	new_size += 8;

	// memory allocation
	char* data = new char[new_size];
	memmove(data, original, size);

	for (int i = 0; i < new_size - size; i++)
	{
		if (i)
			data[size + i] = 0x00;
		else
			data[size + i] = 0x80; // 1 and zeros for first time
	}


	// write low bits first then height
	uint64_t bits_size = size * 8;
	char* low_heigh_dwords = (char*)(&(bits_size));

	data[new_size - 8] = low_heigh_dwords[4];
	data[new_size - 7] = low_heigh_dwords[5];
	data[new_size - 6] = low_heigh_dwords[6];
	data[new_size - 5] = low_heigh_dwords[7];
	data[new_size - 4] = low_heigh_dwords[0];
	data[new_size - 3] = low_heigh_dwords[1];
	data[new_size - 2] = low_heigh_dwords[2];
	data[new_size - 1] = low_heigh_dwords[3];

	*out_new_size = new_size;

	return data;
}


#define F(b, c, d) ((b & c) | ((~b) & d))
#define G(b, c, d) ((b & d) | (c & (~d)))
#define H(b, c, d) (b ^ c ^ d)
#define I(b, c, d) (c ^ (b | (~d)))

static void round1(uint32_t* a, uint32_t b, uint32_t c, uint32_t d, uint32_t X, uint32_t s, uint32_t T)
{
	*a = b + S((*a + F(b, c, d) + X + T), s);
}

static void round2(uint32_t* a, uint32_t b, uint32_t c, uint32_t d, uint32_t X, uint32_t s, uint32_t T)
{
	*a = b + S((*a + G(b, c, d) + X + T), s);
}

static void round3(uint32_t* a, uint32_t b, uint32_t c, uint32_t d, uint32_t X, uint32_t s, uint32_t T)
{
	*a = b + S((*a + H(b, c, d) + X + T), s);
}

static void round4(uint32_t* a, uint32_t b, uint32_t c, uint32_t d, uint32_t X, uint32_t s, uint32_t T)
{
	*a = b + S((*a + I(b, c, d) + X + T), s);
}


inline uint32_t* md5(char* in_data, uint32_t size)
{
	uint32_t new_size;
	uint32_t* data = (uint32_t*)copy(in_data, size, &new_size);
	new_size = new_size / 4;

	uint32_t A = 0x01234567;
	uint32_t B = 0x89ABCDEF;
	uint32_t C = 0xFEDCBA98;
	uint32_t D = 0x76543210;

	uint32_t T[64];
	for (int i = 0; i < 64; i++)
	{
		T[i] = floor(pow(2, 32) * fabs(sin(i + 1)));
	}

	uint32_t X[16];
	uint32_t AA;
	uint32_t BB;
	uint32_t CC;
	uint32_t DD;


	for (int i = 0; i < new_size / 16; i++)
	{
		for (int j = 0; j < 16; j++)
		{
			X[j] = data[j + (16 * i)];
		}

		// save privies values
		AA = A;
		BB = B;
		CC = C;
		DD = D;

		// round 1
		round1(&A, B, C, D, X[0], 7, T[0]);
		round1(&D, A, B, C, X[1], 12, T[1]);
		round1(&C, D, A, B, X[2], 17, T[2]);
		round1(&B, C, D, A, X[3], 22, T[3]);

		round1(&A, B, C, D, X[4], 7, T[4]);
		round1(&D, A, B, C, X[5], 12, T[5]);
		round1(&C, D, A, B, X[6], 17, T[6]);
		round1(&B, C, D, A, X[7], 22, T[7]);

		round1(&A, B, C, D, X[8], 7, T[8]);
		round1(&D, A, B, C, X[9], 12, T[9]);
		round1(&C, D, A, B, X[10], 17, T[10]);
		round1(&B, C, D, A, X[11], 22, T[11]);

		round1(&A, B, C, D, X[12], 7, T[12]);
		round1(&D, A, B, C, X[13], 12, T[13]);
		round1(&C, D, A, B, X[14], 17, T[14]);
		round1(&B, C, D, A, X[15], 22, T[15]);


		// round 2
		round2(&A, B, C, D, X[1], 5, T[16]);
		round2(&D, A, B, C, X[6], 9, T[17]);
		round2(&C, D, A, B, X[11], 14, T[18]);
		round2(&B, C, D, A, X[0], 20, T[19]);

		round2(&A, B, C, D, X[5], 5, T[20]);
		round2(&D, A, B, C, X[10], 9, T[21]);
		round2(&C, D, A, B, X[15], 14, T[22]);
		round2(&B, C, D, A, X[4], 20, T[23]);

		round2(&A, B, C, D, X[9], 5, T[24]);
		round2(&D, A, B, C, X[14], 9, T[25]);
		round2(&C, D, A, B, X[3], 14, T[26]);
		round2(&B, C, D, A, X[8], 20, T[27]);

		round2(&A, B, C, D, X[13], 5, T[28]);
		round2(&D, A, B, C, X[2], 9, T[29]);
		round2(&C, D, A, B, X[7], 14, T[30]);
		round2(&B, C, D, A, X[12], 20, T[31]);


		// round 3
		round3(&A, B, C, D, X[5], 4, T[32]);
		round3(&D, A, B, C, X[8], 11, T[33]);
		round3(&C, D, A, B, X[11], 16, T[34]);
		round3(&B, C, D, A, X[14], 23, T[35]);

		round3(&A, B, C, D, X[1], 4, T[36]);
		round3(&D, A, B, C, X[4], 11, T[37]);
		round3(&C, D, A, B, X[7], 16, T[38]);
		round3(&B, C, D, A, X[10], 23, T[39]);

		round3(&A, B, C, D, X[13], 4, T[40]);
		round3(&D, A, B, C, X[0], 11, T[41]);
		round3(&C, D, A, B, X[3], 16, T[42]);
		round3(&B, C, D, A, X[6], 23, T[43]);

		round3(&A, B, C, D, X[9], 4, T[44]);
		round3(&D, A, B, C, X[12], 11, T[45]);
		round3(&C, D, A, B, X[15], 16, T[46]);
		round3(&B, C, D, A, X[2], 23, T[47]);

		// round 4
		round4(&A, B, C, D, X[0], 6, T[48]);
		round4(&D, A, B, C, X[7], 10, T[49]);
		round4(&C, D, A, B, X[14], 15, T[50]);
		round4(&B, C, D, A, X[5], 21, T[51]);

		round4(&A, B, C, D, X[12], 6, T[52]);
		round4(&D, A, B, C, X[3], 10, T[53]);
		round4(&C, D, A, B, X[10], 15, T[54]);
		round4(&B, C, D, A, X[1], 21, T[55]);

		round4(&A, B, C, D, X[8], 6, T[56]);
		round4(&D, A, B, C, X[15], 10, T[57]);
		round4(&C, D, A, B, X[6], 15, T[58]);
		round4(&B, C, D, A, X[13], 21, T[59]);

		round4(&A, B, C, D, X[4], 6, T[60]);
		round4(&D, A, B, C, X[11], 10, T[61]);
		round4(&C, D, A, B, X[2], 15, T[62]);
		round4(&B, C, D, A, X[9], 21, T[63]);

		A += AA;
		B += BB;
		C += CC;
		D += DD;
	}

	uint32_t* res = new uint32_t[4];

	for (int k = 0; k < 4; k++)
		res[k] = 0x00000000;

	res[0] |= (A << 24) & 0xff000000;
	res[0] |= (A << 8) & 0x00ff0000;
	res[0] |= (A >> 8) & 0x0000ff00;
	res[0] |= (A >> 16) & 0x000000ff;

	res[1] |= (B << 24) & 0xff000000;
	res[1] |= (B << 8) & 0x00ff0000;
	res[1] |= (B >> 8) & 0x0000ff00;
	res[1] |= (B >> 16) & 0x000000ff;

	res[2] |= (C << 24) & 0xff000000;
	res[2] |= (C << 8) & 0x00ff0000;
	res[2] |= (C >> 8) & 0x0000ff00;
	res[2] |= (C >> 16) & 0x000000ff;

	res[3] |= (D << 24) & 0xff000000;
	res[3] |= (D << 8) & 0x00ff0000;
	res[3] |= (D >> 8) & 0x0000ff00;
	res[3] |= (D >> 16) & 0x000000ff;

	delete[] data;
	return res;
}