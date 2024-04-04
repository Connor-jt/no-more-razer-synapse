#include <exception>
#include "HelperFuncs.cpp"

static enum KeyID : unsigned char {
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
		unsigned char B;
	};
	unsigned short unk0x00;
	unsigned short unk0x02;
	unsigned short unk0x04;
	unsigned char  unk0x06[3];
	unsigned short unk0x09;
	unsigned short row_index;
	unsigned short key_count;
	unsigned short unk0x0F;
	RGB            keys[24];
	unsigned short unk0x59;
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
		if (col < 0 || col >= key_count) throw std::exception("bad column index!!");
		keys[col].R = roundf(clamp1(value.R) * 255.0f);
		keys[col].G = roundf(clamp1(value.G) * 255.0f);
		keys[col].B = roundf(clamp1(value.B) * 255.0f);
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
	void init() {
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
	// INHERITABLES //
	int row_count = 0; // overwritten
	keyboard_row* keys = nullptr; // overwritten
	virtual key_position GetKeyPosition(KeyID key) { return key_position{ -1,-1 }; }
};

namespace razer_blackwidow {
	const char row1[] = {ESC,F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12,PRTSC,SCRLK,BREAK};
	const char row2[] = {TILDE,_1,_2,_3,_4,_5,_6,_7,_8,_9,_0,MINUS,EQUALS,BACKSPACE,INS,HOME,PUP,NUM_LK,NUM_FSLASH,NUM_ASTRICKS,NUM_MINUS};
	const char row3[] = {TAB,Q,W,E,R,T,Y,U,I,O,P,LBRACKET,RBRACKET,BSLASH,DEL,END,PDOWN,NUM_7,NUM_8,NUM_9,NUM_PLUS};
	const char row4[] = {CAPS,A,S,D,F,G,H,J,K,L,SEMI,APOS,ENTER,NUM_4,NUM_5,NUM_6};
	const char row5[] = {LSHIFT,Z,X,C,V,B,N,M,COMMA,PERIOD,FSLASH,RSHIFT,UP,NUM_1,NUM_2,NUM_3,NUM_ENTER};
	const char row6[] = {LCTRL,WINDOWS,LALT,SPACE,RALT,FN,MENU,RCTRL,LEFT,DOWN,RIGHT,NUM_0,NUM_DEL};
	static keyboard_row rows[6] = {keyboard_row{sizeof(row1),row1},
								   keyboard_row{sizeof(row2),row2},
								   keyboard_row{sizeof(row3),row3},
								   keyboard_row{sizeof(row4),row4},
								   keyboard_row{sizeof(row5),row5},
								   keyboard_row{sizeof(row6),row6}};
	
	class device_blackwidow:public razer_device {
	public:
		device_blackwidow() {
			row_count = 6;
			keys = rows;
			init();
		}
		key_position GetKeyPosition(KeyID key) override {
			switch (key) {
			case ESC:			return key_position{ 0, 0 };
			case F1:			return key_position{ 0, 1 };
			case F2:			return key_position{ 0, 2 };
			case F3:			return key_position{ 0, 3 };
			case F4:			return key_position{ 0, 4 };
			case F5:			return key_position{ 0, 5 };
			case F6:			return key_position{ 0, 6 };
			case F7:			return key_position{ 0, 7 };
			case F8:			return key_position{ 0, 8 };
			case F9:			return key_position{ 0, 9 };
			case F10:			return key_position{ 0,10 };
			case F11:			return key_position{ 0,11 };
			case F12:			return key_position{ 0,12 };
			case PRTSC:			return key_position{ 0,13 };
			case SCRLK:			return key_position{ 0,14 };
			case BREAK:			return key_position{ 0,15 };
			case TILDE:			return key_position{ 1, 0 };
			case _1:			return key_position{ 1, 1 };
			case _2:			return key_position{ 1, 2 };
			case _3:			return key_position{ 1, 3 };
			case _4:			return key_position{ 1, 4 };
			case _5:			return key_position{ 1, 5 };
			case _6:			return key_position{ 1, 6 };
			case _7:			return key_position{ 1, 7 };
			case _8:			return key_position{ 1, 8 };
			case _9:			return key_position{ 1, 9 };
			case _0:			return key_position{ 1,10 };
			case MINUS:			return key_position{ 1,11 };
			case EQUALS:		return key_position{ 1,12 };
			case BACKSPACE:		return key_position{ 1,13 };
			case INS:			return key_position{ 1,14 };
			case HOME:			return key_position{ 1,15 };
			case PUP:			return key_position{ 1,16 };
			case NUM_LK:		return key_position{ 1,17 };
			case NUM_FSLASH:	return key_position{ 1,18 };
			case NUM_ASTRICKS:	return key_position{ 1,19 };
			case NUM_MINUS:		return key_position{ 1,20 };
			case TAB:			return key_position{ 2, 0 };
			case Q:				return key_position{ 2, 1 };
			case W:				return key_position{ 2, 2 };
			case E:				return key_position{ 2, 3 };
			case R:				return key_position{ 2, 4 };
			case T:				return key_position{ 2, 5 };
			case Y:				return key_position{ 2, 6 };
			case U:				return key_position{ 2, 7 };
			case I:				return key_position{ 2, 8 };
			case O:				return key_position{ 2, 9 };
			case P:				return key_position{ 2,10 };
			case LBRACKET:		return key_position{ 2,11 };
			case RBRACKET:		return key_position{ 2,12 };
			case BSLASH:		return key_position{ 2,13 };
			case DEL:			return key_position{ 2,14 };
			case END:			return key_position{ 2,15 };
			case PDOWN:			return key_position{ 2,16 };
			case NUM_7:			return key_position{ 2,17 };
			case NUM_8:			return key_position{ 2,18 };
			case NUM_9:			return key_position{ 2,19 };
			case NUM_PLUS:		return key_position{ 2,20 };
			case CAPS:			return key_position{ 3, 0 };
			case A:				return key_position{ 3, 1 };
			case S:				return key_position{ 3, 2 };
			case D:				return key_position{ 3, 3 };
			case F:				return key_position{ 3, 4 };
			case G:				return key_position{ 3, 5 };
			case H:				return key_position{ 3, 6 };
			case J:				return key_position{ 3, 7 };
			case K:				return key_position{ 3, 8 };
			case L:				return key_position{ 3, 9 };
			case SEMI:			return key_position{ 3,10 };
			case APOS:			return key_position{ 3,11 };
			case ENTER:			return key_position{ 3,12 };
			case NUM_4:			return key_position{ 3,13 };
			case NUM_5:			return key_position{ 3,14 };
			case NUM_6:			return key_position{ 3,15 };
			case LSHIFT:		return key_position{ 4, 0 };
			case Z:				return key_position{ 4, 1 };
			case X:				return key_position{ 4, 2 };
			case C:				return key_position{ 4, 3 };
			case V:				return key_position{ 4, 4 };
			case B:				return key_position{ 4, 5 };
			case N:				return key_position{ 4, 6 };
			case M:				return key_position{ 4, 7 };
			case COMMA:			return key_position{ 4, 8 };
			case PERIOD:		return key_position{ 4, 9 };
			case FSLASH:		return key_position{ 4,10 };
			case RSHIFT:		return key_position{ 4,11 };
			case UP:			return key_position{ 4,12 };
			case NUM_1:			return key_position{ 4,13 };
			case NUM_2:			return key_position{ 4,14 };
			case NUM_3:			return key_position{ 4,15 };
			case NUM_ENTER:		return key_position{ 4,16 };
			case LCTRL:			return key_position{ 5, 0 };
			case WINDOWS:		return key_position{ 5, 1 };
			case LALT:			return key_position{ 5, 2 };
			case SPACE:			return key_position{ 5, 3 };
			case RALT:			return key_position{ 5, 4 };
			case FN:			return key_position{ 5, 5 };
			case MENU:			return key_position{ 5, 6 };
			case RCTRL:			return key_position{ 5, 7 };
			case LEFT:			return key_position{ 5, 8 };
			case DOWN:			return key_position{ 5, 9 };
			case RIGHT:			return key_position{ 5,10 };
			case NUM_0:			return key_position{ 5,11 };
			case NUM_DEL:		return key_position{ 5,12 };
			} return key_position{ -1,-1 };
		}
	};
}