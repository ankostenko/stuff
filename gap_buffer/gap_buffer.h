#pragma once

#include <memory.h>

class GapBuffer {
	char* data;
	int data_size;
	int data_capacity;
	int gap_start_position;
	int gap_end_position;
	int gap_size;


	void DataGrow(int new_size) {
		int new_capacity = data_capacity * 2 + 1;
		data_capacity = std::_Max_value(new_capacity, new_size);
		if (data) {
			char* intermidiante = new char[data_capacity];
			memmove(intermidiante, data, data_size);
			delete[] data;
			data = intermidiante;
		}
		else {
			data = new char[data_capacity];
		}
	}

	void GapBufGrow(int new_gap_size) {

		int new_data_size = data_size + new_gap_size;
		if (data_capacity < new_data_size)
			DataGrow(new_data_size);

		memmove(data + gap_end_position + new_gap_size, data + gap_end_position, data_size - gap_end_position);

		data_size = new_data_size;
		gap_end_position += new_gap_size;
		gap_size += new_gap_size;

	}

	void MoveGapLeft() {
		data[gap_end_position - 1] = data[gap_start_position - 1];
		gap_end_position--;
		gap_start_position--;
	}

	void MoveGapRight() {
		data[gap_start_position] = data[gap_end_position];
		gap_end_position++;
		gap_start_position++;
	}

public:

	GapBuffer(char* data, int data_size = 0) {
		if (data) {
			this->data_size = strlen(data) + 1; // We need to get '\0' too
			data_capacity = std::_Max_value(this->data_size, 100);
			DataGrow(this->data_size);
			memmove(this->data, data, this->data_size);
		}
		else {
			data_capacity = 0;
			DataGrow(data_size);
		}
		this->gap_start_position = 0;
		this->gap_end_position = 0;
		GapBufGrow(20);
	}

	~GapBuffer() {
		delete[] this->data;
	}

	void SetGapPososition(int position) {
		if (gap_start_position < position) {
			while (gap_start_position != position)
				MoveGapRight();
		}
		else {
			while (gap_start_position != position)
				MoveGapLeft();
		}
		return;
	}

	void InsertCharacter(int position, char ch) {
		SetGapPososition(position);

		if (gap_size == 1) {
			GapBufGrow(20);
		}

		data[gap_start_position++] = ch;
		gap_size--;
	}

	void InsertString(int position, char* str) {
		int i = 0;
		SetGapPososition(position);

		while (str[i] != '\0') {

			if (gap_size == 1) {
				GapBufGrow(20);
			}

			data[gap_start_position++] = str[i++];
			gap_size--;
		}
	}

	int GetSize() {
		return data_size - gap_size - 1; // witout '\0'
	}

	char operator[] (int inx) const {
		if (inx >= gap_start_position)
			return data[inx + gap_size];
		else
			return data[inx];
	}
};