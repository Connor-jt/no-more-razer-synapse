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
	WHEEL,
};


struct keyboard_row { int size; const char* ptr; };
struct key_position { int row; int col; };
struct RGB_float { float R; float G; float B; };
#pragma pack(push, 1)
struct razer_rgb_data {
private:
	struct RGB {
		unsigned char R;
		unsigned char G;
		unsigned char B;};
	unsigned short unk0x00;
	unsigned short unk0x02;
	unsigned short unk0x04;
	unsigned char  unk0x06[3];
	unsigned short unk0x09;
	unsigned char  row_index;
	unsigned char  unk0x0C;
	unsigned char  last_key_index;
	RGB            keys[25];
	// not included in checksuming process
	unsigned char  checksum; 
	unsigned char  unk0x5A;
public:
	void init(int index, int col_count) { // just to set the mem
		unk0x00 = 0;
		unk0x02 = 31;
		unk0x04 = 0;
		unk0x06[0] = 0x47; unk0x06[1] = 0x0F; unk0x06[2] = 0x03;
		unk0x09 = 0;
		row_index = index;
		key_count = col_count;
		unk0x0F = 0;
		for (int i = 0; i < 24; i++) keys[i] = RGB{ 0,0,0 };
		unk0x59 = 0;
	}
	void pack_RGB(RGB_float value, int col) {
		if (col < 0 || col > last_key_index) throw std::exception("bad column index!!");
		keys[col].R = roundf(clamp1(value.R) * 255.0f);
		keys[col].G = roundf(clamp1(value.G) * 255.0f);
		keys[col].B = roundf(clamp1(value.B) * 255.0f);
	}
	void set_checksum() {
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
public:
	void init() {	// ONLY CALL ONCE, NOT LOCKED after first call
		if (keys == nullptr || row_count == 0) throw std::exception("inheritance failure!!");

		// malloc data buffers by amount of rows
		data_buffers = new razer_rgb_data[row_count];
		for (int i = 0; i < row_count; i++)
			data_buffers[i].init(i, keys[i].size);
		
		// malloc rgb array and then each entry (as well as set )
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

	RGB_float** RGB_values; 
	razer_rgb_data* data_buffers;
	void SetKey(int row, int col, RGB_float value) {
		VerifyKeyPos(row, col);
		RGB_values[row][col] = value;
		data_buffers[row].pack_RGB(value, col);
	}
	RGB_float GetKey(int row, int col) {
		VerifyKeyPos(row, col);
		return RGB_values[row][col];
	}
	void SetChecksum(int row) { // called before sending if required by device
		VerifyKeyPos(row, 0);
		data_buffers[row].set_checksum();
	}
	// INHERITABLES //
	int row_count = 0; // overwritten
	keyboard_row* keys = nullptr; // overwritten
	virtual key_position GetKeyPosition(KeyID key) { return key_position{ -1,-1 }; }
};
}