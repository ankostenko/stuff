#pragma once

#include <iostream>
#include <memory.h>
#include <ctype.h>
#include <cstring>
#include <cassert>
#include "hash.h"

namespace my {

	class string : public IHash
	{
		char* str{new char('\0')};
		size_t _length{1};   // _length include '\0'

		string(char* str, size_t len) {
			_length = len;
			this->str = new char[_length];
			memmove(this->str, str, _length);
		}

	public:

		string() {}

		string(char const* str) {
			_length = strlen(str) + 1;
			this->str = new char[_length];
			memmove(this->str, str, _length);
		}

		string(string const& other) {
			_length = other._length;
			delete[] str;
			str = new char[_length];
			memmove(str, other.str, _length);
		}

		string(string&& other) {
			_length = other._length;
			str = other.str;
			other._length = 0;
			other.str = nullptr;
		}

		~string() {
			delete[] str;
		}
		
		size_t size() const{
			return _length - 1;
		}

		size_t length() const {
			return _length;
		}

		string& operator = (string const& other) {
			_length = other._length;
			delete[] str;
			str = new char[_length];
			memmove(this->str, other.str, _length);
			return *this;
		}

		string& operator = (string&& other) {
			_length = other._length;
			delete[] str;
			str = other.str;
			other._length = 0;
			other.str = nullptr;
			return *this;
		}

		string& operator += (string const& other) {
			size_t new__length = _length + other.size();
			char* new_str = new char[new__length];
			memmove(new_str, str, size());
			memmove(new_str + size(), other.str, other._length);
			delete[] str;
			str = new_str;
			_length = new__length;
			return *this;
		}

		string& operator += (char const* other) {
			size_t otherlen = strlen(other) + 1;
			size_t new__length = size() + otherlen;
			char* new_str = new char[new__length];
			memmove(new_str, str, size());
			memmove(new_str + size(), other, otherlen);
			delete[] str;
			str = new_str;
			_length = new__length;
			return *this;
		}

		string& operator += (char const ch) {
			insert(ch, size());
			return *this;
		}

		string& operator + (string const& other) const{
			char* new_str = new char[_length + other.size()];
			memmove(new_str, str, size());
			memmove(new_str + size(), other.str, other._length);
			return  *(new string(new_str, _length + other.size()));
		}

		string& operator * (size_t const& factor) const{
			char* new_str = new char[size() * factor + 1];
			for (int i = 0; i < factor; i++)
				memmove(new_str + (size() * i), str, size());
			
			new_str[size() * factor] = '\0';
			return *(new string(new_str, size() * factor + 1));
		}

		char& operator [] (int const& i) const{
			assert(i >= 0 || i < _length);
			return str[i];
		}

		char& At(int const& i) {
			return operator[](i);
		}

		char& last() {
			return str[_length - 1];
		}

		void insert(char const& ch, int const& i) {
			if (i < 0 || i > _length)
				return;

			size_t new_len = _length + 1;
			char* new_str = new char[new_len];

			memmove(new_str, str, i);
			new_str[i] = ch;
			memmove(new_str + i + 1, str + i, _length - i);

			delete[] str;
			str = new_str;
			++_length;
		}

		char* getCString() {
			return str;
		}

		int getHash() {
			int hash = 0;
			for (int i = 0; i < _length; i++) {
				int inx = i % 5;
				switch (inx) {
				case 0: {
					hash += (128 - str[i]) * 111;
				}
				case 1: {
					hash += (128 - str[i]) * 17;
					break;
				}
				case 2: {
					hash += (128 - str[i]) * 11;
					break;

				}
				case 3: {
					hash += (128 - str[i]) * 13;
					break;

				}
				case 4: {
					hash += (128 - str[i]) * 7;
					break;

				}
				}
			}
			return hash % 100000;
		}

		bool operator == (string const& other) const{
			if (_length != other._length)
				return false;

			for (int i = 0; i < _length; i++)
				if (str[i] != other[i])
					return false;

			return true;
		}

		bool operator < (string const& other) const {
			if (_length < other._length)
				return true;

			if (_length > other._length)
				return false;

			for (int i = 0; i < _length; i++) {
				if (str[i] < other[i])
					return true;

				if (str[i] > other[i])
					return false;
			}

			return false;
		}

		friend std::ostream& operator << (std::ostream& out, string const& c) {
			out << c.str;
			return out;
		}
	};
}
