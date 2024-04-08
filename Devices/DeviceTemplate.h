#pragma once
#include <exception>
#include "../HelperFuncs.cpp"
namespace RazerDevice {
static enum KeyID : unsigned char {
	NONE = 0,
	SPACE = ' ',
	APOS = '\'',
	COMMA = ',',
	MINUS = '-',
	PERIOD = '.',
	FSLASH = '/',
	_0 = '0',
	_1 = '1',
	_2 = '2',
	_3 = '3',
	_4 = '4',
	_5 = '5',
	_6 = '6',
	_7 = '7',
	_8 = '8',
	_9 = '9',
	SEMI = ';',
	EQUALS = '=',
	LBRACKET = '[',
	BSLASH = '\\',
	RBRACKET = ']',
	A = 'a',
	B = 'b',
	C = 'c',
	D = 'd',
	E = 'e',
	F = 'f',
	G = 'g',
	H = 'h',
	I = 'i',
	J = 'j',
	K = 'k',
	L = 'l',
	M = 'm',
	N = 'n',
	O = 'o',
	P = 'p',
	Q = 'q',
	R = 'r',
	S = 's',
	T = 't',
	U = 'u',
	V = 'v',
	W = 'w',
	X = 'x',
	Y = 'y',
	Z = 'z',
	TILDE = '~',
	ESC,
	F1,
	F2,
	F3,
	F4,
	F5,
	F6,
	F7,
	F8,
	F9,
	F10,
	F11,
	F12,
	BACKSPACE,
	TAB,
	CAPS,
	LSHIFT,
	LCTRL,
	WINDOWS,
	LALT,
	RSHIFT,
	RALT,
	FN,
	MENU,
	RCTRL,
	PRTSC,
	SCRLK,
	BREAK,
	INS,
	HOME,
	PUP,
	DEL,
	END,
	PDOWN,
	UP,
	DOWN,
	LEFT,
	RIGHT,
	NUM_LK,
	NUM_FSLASH,
	NUM_ASTRICKS,
	NUM_MINUS,
	NUM_PLUS,
	NUM_ENTER,
	NUM_DEL,
	NUM_0,
	NUM_1,
	NUM_2,
	NUM_3,
	NUM_4,
	NUM_5,
	NUM_6,
	NUM_7,
	NUM_8,
	NUM_9,
	ENTER,
	WHEEL, // keypad mouse wheel
	ZONE1, // mouse mat zone // keyboard logo

};


struct keyboard_row { int size; const char* ptr; };
struct key_position { int row; int col; };
struct key_offset   { float x; float y; };
struct RGB_float { float R; float G; float B; };
#pragma pack(push, 1)
struct razer_rgb_data {
private:
	struct RGB {
		unsigned char R;
		unsigned char G;
		unsigned char B;};
	unsigned char  unk0x00;		// 00
	unsigned char  unk0x01;		// 00
	unsigned char  unk0x02;		// 1f
	// below is checksum'd
	unsigned char  unk0x03;		// 00
	unsigned char  unk0x04;		// 00
	unsigned char  unk0x05;		// 00
	unsigned char  packet_size;	// ??
	unsigned char  unk0x07;		// 0F
	unsigned char  packet_type; // ?? (03 for lighting input, 02 for disable lighting)
	// sub structure (packet)
	unsigned short unk0x09;		// 00 00
	unsigned char  row_index;	// ??
	unsigned char  unk0x0C;		// 00
	unsigned char  last_key_index; // ??
	RGB            zones[25];
	// footer packet data
	unsigned char  checksum;	// ??
	unsigned char  unk0x5A;		// 00
public:
	void init(int index, int col_count, char device_packet_size) { // just to set the mem
		// signature
		unk0x00 = 0;
		unk0x01 = 0;
		unk0x02 = 31;
		// packet header
		unk0x03 = 0;
		unk0x04 = 0;
		unk0x05 = 0;
		packet_size = device_packet_size;
		unk0x07 = 0x0F; 
		packet_type = 0x03;
		// packet
		unk0x09 = 0;
		row_index = index;
		unk0x0C = 0;
		last_key_index = col_count;
		for (int i = 0; i < 25; i++) zones[i] = RGB{ 0,0,0 };
		CalcChecksum();
		unk0x5A = 0;
	}
	void pack_RGB(RGB_float value, int col) {
		if (col < 0 || col > last_key_index) throw std::exception("bad column index!!");
		// check out data from checksum // just do a whole hash before submitting data!!
		//checksum ^= zones[col].R ^ zones[col].G ^ zones[col].B;
		zones[col].R = roundf(clamp1(value.R) * 255.0f);
		zones[col].G = roundf(clamp1(value.G) * 255.0f);
		zones[col].B = roundf(clamp1(value.B) * 255.0f);
		// check in new data
		//checksum ^= zones[col].R ^ zones[col].G ^ zones[col].B;
	}
	void CalcChecksum() {
		checksum = 0;
		unsigned char* data_bytes = (unsigned char*)(this);
		for (int index = 3; index < sizeof(razer_rgb_data) - 2; index++)
			checksum ^= data_bytes[index];
	}
};
#pragma pack(pop)
class razer_device {
private:
	void VerifyKeyPos(int row, int col) {
		if (row < 0 || row >= row_count) throw std::exception("bad row index!!");
		if (col < 0 || col >= keys[row].size) throw std::exception("bad column index!!");
	}
	void GetRowClosestKeys(int& left, int& right, float target, int row) {
		for (int i = 0; i < keys[row].size; i++) {
			auto col_pos = GetKeyOffset(row, i); // assuming that each row has at least 1 column
			right = i;
			if (!isnan(col_pos.x) && col_pos.x >= target) {
				left = i - 1; // reverts to -1 if  i == 0
				break;
	}}}
	void PaintToKey(int row, int col, float origin_x, float origin_y, RGB_float value, bool additive) {
		if (row == -1 || col == -1) return;
		auto key_pos = GetKeyOffset(row, col);
		if (isnan(key_pos.x)) return;
		float distance = std::hypotf(abs(key_pos.x - origin_x), abs(key_pos.y - origin_y));
		const float max_distance = 0.05f; // world units??? TODO: do something better with this
		float clampped_dist = max_distance - distance;
		if (clampped_dist <= 0.0f) return; // dont need to paint then
		float color_strength = clampped_dist / max_distance;
		SetKey(row, col, RGB_float{ value.R * color_strength,value.G * color_strength,value.B * color_strength }, additive);
	}
protected:
	char device_packet_size = 0;
	char last_key_index = 0;
public:
	void init() {	// ONLY CALL ONCE, NOT LOCKED after first call
		if (keys == nullptr || key_virtual_positions == nullptr || row_count == 0 || device_packet_size == 0) throw std::exception("inheritance failure!!");

		// malloc data buffers by amount of rows
		data_buffers = new razer_rgb_data[row_count];
		for (int i = 0; i < row_count; i++)
			data_buffers[i].init(i, last_key_index, device_packet_size);
		
		// malloc rgb array and then each entry (as well as set them to empty values, because new doesn't do that here for some reason)
		RGB_values = new RGB_float*[row_count];
		for (int i = 0; i < row_count; i++) {
			RGB_values[i] = new RGB_float[keys[i].size];
			for (int j = 0; j < keys[i].size; j++) {
				RGB_values[i][j].R = 0.0f;
				RGB_values[i][j].G = 0.0f;
				RGB_values[i][j].B = 0.0f;
			}
		}
	}
	~razer_device() {
		delete[](data_buffers);
		for (int i = 0; i < row_count; i++) delete[](RGB_values[i]);
		delete[](RGB_values);
	}

	RGB_float** RGB_values = nullptr; 
	razer_rgb_data* data_buffers = nullptr;
	void SetKey(int row, int col, RGB_float value, bool additive) {
		VerifyKeyPos(row, col);
		if (additive) {
			auto var = RGB_values[row][col];
			value.R += var.R;
			value.G += var.G;
			value.B += var.B;}
		RGB_values[row][col] = value;
		data_buffers[row].pack_RGB(value, col);
	}
	RGB_float GetKey(int row, int col) {
		VerifyKeyPos(row, col);
		return RGB_values[row][col];
	}
	virtual key_position GetKeyPosition(KeyID key) { return key_position{ -1,-1 }; }
	// variables for device real world position or dimensions??
	key_offset GetKeyOffset(int row, int col) {
		VerifyKeyPos(row, col);
		return key_offset{ key_virtual_positions[row][col*2]*width,key_virtual_positions[row][(col*2)+1]*width*height_ratio };
	}

	void PaintAt(float x, float y, RGB_float value, bool additive) { // y decrements with row index, x increases with column index
		// using a quick neighbour system that works best when all elements in a row have about equal height (so there will be artifacts with some silly zones)
		int lower_row_index = -1; // lowest to bottom
		int upper_row_index = -1;
		for (int i = 0; i < row_count; i++) {
			RazerDevice::key_offset row_pos;
			int col_index = 0;
			do{ row_pos = GetKeyOffset(i, col_index);
				col_index++;
				if (col_index == keys[row_count].size) break; // escape if all cols are empty (impossible)
			} while (isnan(row_pos.x));
			lower_row_index = i;
			if (!isnan(row_pos.x) && row_pos.y <= y) {
				upper_row_index = i-1; // in cases where this is true for index 0, this reverts the value to -1
				break;
		}}

		// then scan the rows
		int lower_left_col_index = -1;
		int lower_right_col_index = -1; // will always find a right, but not a left
		GetRowClosestKeys(lower_left_col_index, lower_right_col_index, x, lower_row_index);
		int upper_left_col_index = -1; // both may not be found
		int upper_right_col_index = -1; 
		if (upper_row_index != -1) GetRowClosestKeys(upper_left_col_index, upper_right_col_index, x, upper_row_index);

		PaintToKey(lower_row_index, lower_left_col_index,  x, y, value, additive);
		PaintToKey(lower_row_index, lower_right_col_index, x, y, value, additive);
		PaintToKey(upper_row_index, upper_left_col_index,  x, y, value, additive);
		PaintToKey(upper_row_index, upper_right_col_index, x, y, value, additive);
	}
	// INHERITABLES //
	int row_count = 0;
	float width = 1.0f;
	float height_ratio = 0.0f;
	keyboard_row* keys = nullptr;
protected:
	const float** key_virtual_positions = nullptr; 
};
}