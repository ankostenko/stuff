#pragma once

#include <cassert>
#include <memory.h>
#include <malloc.h>
#include <cstddef>
#include "math.h"

namespace my {

	/**
		Usfull only for primitive types
		because memcopy copy ref's ptr's
		objects and destructor finish it
		result: memory Error
	*/

	template <typename T>
	class vectorFast
	{
		T* arr{ nullptr };
		size_t _size{ 0 };
		size_t _capacity{ 0  };

		void grow(size_t const& new_capacity = 0, bool unspecific = true) { // false if i want concretó size
			assert(new_capacity > 0);
			if (new_capacity < _size && new_capacity >= 0) {
				_size = new_capacity;
				return;
			}

			size_t new_cap = 0;
			if (unspecific)
				new_cap = MAX(_capacity * 2 + 1, new_capacity);
			else
				new_cap = new_capacity;

			assert(_capacity < new_cap);
			if (arr) {
				T* new_arr = new T[new_cap];
				memmove(new_arr, arr, sizeof(T) * _size);
				delete[] arr;
				arr = new_arr;
			}
			else {
				arr = new T[new_cap];
			}
			_capacity = new_cap;
		}

	public:

		vectorFast() = default;

		vectorFast(T* arr, size_t size): _size(size), _capacity(size) {
			this->arr = (T*)malloc(sizeof(T) * size);
			memmove(this->arr, arr, size * sizeof(T));
		}

		vectorFast(size_t const& capacity) {
			grow(capacity, false);
		}

		vectorFast(size_t const& size, T elem) {
			_size = size;
			grow(_size);
			fill(elem);
		}

		vectorFast(vectorFast&& other) {
			arr = other.arr;
			_size = other._size;
			_capacity = other._capacity;
			other.arr = nullptr;
			other._capacity = 0;
			other._size= 0;
		}

		vectorFast& operator = (vectorFast&& other) {
			arr = other.arr;
			_size = other._size;
			_capacity = other._capacity;
			other.arr = nullptr;
			other._capacity = 0;
			other._size = 0;
			return* this;
		}

		vectorFast(vectorFast const& other) {
			_size = other._size;
			_capacity = other._capacity;
			arr = (T*)malloc(sizeof(T) * _capacity);
			memmove(arr, other.arr, sizeof(T) * _size);
		}

		~vectorFast() {
			delete[] arr;
		}

		vectorFast& operator = (vectorFast const& other) {
			if (this == &other || other.arr == nullptr)
				return *this;
			_size = other._size;
			_capacity = other._capacity;
			delete[] arr;
			arr = (T*)malloc(sizeof(T) * _capacity);
			memmove(arr, other.arr, sizeof(T) * _size);
			return *this;
		}

		void operator << (T const& elem) {  // use it instead push_back if u watch anime
			push_back(elem);
		}

		void push_back(T elem) {
			if (_capacity <= _size)
				grow(_size + 1);
			arr[_size++] = elem;
		}

		void remove(size_t const& pos) {
			assert(pos >= 0 && pos < _size);
			memmove(arr + pos - 1, arr + pos, sizeof(T) * (_size - pos));
			_size -= 1;
		}

		void resize(size_t new_size, T elem) {
			grow(new_size);
			if (new_size > _size)
				fill(elem, _size, new_size - 1);
			else
				fill(elem, 0, new_size - 1);

			_size = new_size;
		}

		void reserve(size_t const& new_capacity) {
			grow(new_capacity);
		}

		void clear() {
			_size = 0;
		}

		T& operator [] (size_t const& i) {
			assert(i >= 0 && i < _size);
			return arr[i];
		}

		T& At(size_t const& i) {
			return operator[](i);
		}

		void insert(int place, T elem) {
			assert(place >= 0 && place <= _size);
			if (++_size > _capacity)
				grow(_size);

			memmove(arr + place + 1, arr + place, sizeof(T) * (_size - place));
			arr[place] = elem;
		}

		void fill(T elem,size_t start = -1, size_t end = -1) {
			if (start == -1 && end == -1) {
				for (int i = 0; i < _size; i++) {
					arr[i] = elem;
				}
				return;
			}

			assert(start >= 0 && start < _capacity && end >= 0 && end < _capacity && start <= end);
			for (size_t i = start; i < end + 1; i++) {
				arr[i] = elem;
			}
		}

		T& front() {
			return At(0);
		}

		T& back() {
			return At(_size - 1);
		}

		T* begin() {
			return arr;
		}

		T* end() {
			return arr + _size - 1;
		}

		size_t size() {
			return _size;
		}

		size_t capacity() {
			return _capacity;
		}

		bool empty() {
			return _size == 0 ? true : false;
		}

		// stack

		T pop() {
			return At(--_size);
		}

		T& peak() {
			return At(_size - 1);
		}

		void push(T const& elem) {
			push_back(elem);
		}

	};
}