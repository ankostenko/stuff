#pragma once
#include <cassert>
#include <memory.h>

#define MAX(a, b) (a > b ? a : b)

// simple implemintation of dynamic array

template <typename T>
class vector
{
private:
	using type = T;
	using pinter = T*;
	size_t elem__size = sizeof(type);
	
	type* buffer{nullptr};
	size_t _size{0};
	size_t _capacity{0};

	void grow(size_t new_size)
	{
		size_t new_capacity = _capacity * 2 + 1;
		new_capacity = MAX(new_size, new_capacity);
		assert(new_capacity > _capacity);
		type* new_buffer = static_cast<type*>(::operator new(new_capacity * elem__size));
		if (buffer)
		{
			memmove(new_buffer, buffer, elem__size * _size);
			::operator delete(buffer);
		}
		buffer = new_buffer;
		assert(buffer);
		_capacity = new_capacity;
	}


public:

	inline vector() = default;

	inline vector(size_t size, type const& value = type{}) : _capacity(_size)
	{
		grow(_capacity);
		for (int i = 0; i < size; i++)
			push_back(value);
	}

	inline ~vector()
	{
		for (int i = 0; i < _size; i++)
			buffer[i].~type();
		::operator delete(buffer);
	}

	inline void push_back(type const& value)
	{
		size_t new__size = _size + 1;
		if (new__size > _capacity)
			grow(new__size);
		new(buffer + _size) type(value);
		_size = new__size;
	}

	inline type& operator [](size_t i)
	{
		assert(i >= 0 && i < size);
		return buffer[i];
	}
	
	inline void erise(int i)
	{
		assert(i >= 0 && i < size);
		buffer[i].~type();
		memmove(buffer + i, buffer + i + 1, elem__size * (_size - i));
	}

	inline void reserve(size_t reserve_size)
	{
		grow(reserve_size);
	}

	inline int size(void) { return _size; }
	inline int capacity(void) { return _capacity; }

	inline type* begin() { return buffer; }
	inline type* end() { return buffer + _size; }
};