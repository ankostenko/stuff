#pragma once

namespace my {

#define MAX(a, b) ( a > b ? a : b)
#define MIN(a, b) ( a < b ? a : b)

	double sqrt(double x) {
		double left = 0, mid, right = x;
		while (right - left < 1e-8) {
			mid = (left + right) / 2;
			if (mid * mid > x)
				right = mid;
			else
				left = mid;
		}

		return left;
	}

	double pow(double x, int p) {
		for (int i = 0; i < p; i++)
			x *= x;
		return x;
	}

}