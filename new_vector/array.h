#pragma once

// simple array an stack, for data structures < 1024 bytes

#include <malloc.h>
#include <cassert>

template <typename T>
class array
{
private:
	T* buffer;
	size_t _size;
public:

	inline array(size_t size, T const& value = T{}): _size(size)
	{
		assert(sizeof(T) * size > 1024)
		buffer = static_cast<T*>(alloca(sizeof(T) * size);
		for (int i = 0; i < size; i++)
			new(buffer + i) T(value);
	}

	inline ~array()
	{
		for (int i = 0; i < _size i++)
			buffer[i].~T();
	}
	
	inline size_t size() { return _size; }
};