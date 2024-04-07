#pragma once
#include "DeviceTemplate.h"
namespace razer_blackwidow {
	using namespace RazerDevice;
	// razer logo is ZONE1
	const char row1[] = { NONE,ESC,NONE,F1,F2,F3,F4,F5,F6,F7,F8,F9,F10,F11,F12,PRTSC,SCRLK,BREAK };
	const char row2[] = { NONE,TILDE,_1,_2,_3,_4,_5,_6,_7,_8,_9,_0,MINUS,EQUALS,BACKSPACE,INS,HOME,PUP,NUM_LK,NUM_FSLASH,NUM_ASTRICKS,NUM_MINUS };
	const char row3[] = { NONE,TAB,Q,W,E,R,T,Y,U,I,O,P,LBRACKET,RBRACKET,BSLASH,DEL,END,PDOWN,NUM_7,NUM_8,NUM_9,NUM_PLUS };
	const char row4[] = { NONE,CAPS,A,S,D,F,G,H,J,K,L,SEMI,APOS,NONE,ENTER,NONE,NONE,NONE,NUM_4,NUM_5,NUM_6 };
	const char row5[] = { NONE,LSHIFT,NONE,Z,X,C,V,B,N,M,COMMA,PERIOD,FSLASH,NONE,RSHIFT,NONE,UP,NONE,NUM_1,NUM_2,NUM_3,NUM_ENTER };
	const char row6[] = { NONE,LCTRL,WINDOWS,LALT,NONE,NONE,SPACE,NONE,NONE,NONE,RALT,ZONE1,FN,MENU,RCTRL,LEFT,DOWN,RIGHT,NONE,NUM_0,NUM_DEL };
	static keyboard_row rows[6] = {keyboard_row{sizeof(row1),row1},
								   keyboard_row{sizeof(row2),row2},
								   keyboard_row{sizeof(row3),row3},
								   keyboard_row{sizeof(row4),row4},
								   keyboard_row{sizeof(row5),row5},
								   keyboard_row{sizeof(row6),row6}};
	const float device_width = 1.0f;
	const float height_multi = 0.3134986712457479f;
	const float pos_row0[] = { NAN,NAN, 0.0000,1.0000,    NAN,   NAN, 0.0921,1.0000, 0.1386,1.0000, 0.1843,1.0000, 0.2304,1.0000, 0.2999,1.0000, 0.3460,1.0000, 0.3924,1.0000, 0.4385,1.0000, 0.5076,1.0000, 0.5537,1.0000, 0.6002,1.0000, 0.6463,1.0000, 0.7079,1.0000, 0.7540,1.0000, 0.8005,1.0000 };
	const float pos_row1[] = { NAN,NAN, 0.0000,0.8029, 0.0461,0.8029, 0.0922,0.8029, 0.1385,0.8029, 0.1845,0.8029, 0.2307,0.8029, 0.2768,0.8029, 0.3229,0.8029, 0.3692,0.8029, 0.4152,0.8029, 0.4616,0.8029, 0.5079,0.8029, 0.5539,0.8029, 0.6231,0.8029, 0.7079,0.8029, 0.7540,0.8029, 0.8005,0.8029, 0.8617,0.8029, 0.9078,0.8029, 0.9539,0.8029, 1.0000,0.8029 };
	const float pos_row2[] = { NAN,NAN, 0.0117,0.6557, 0.0693,0.6557, 0.1153,0.6557, 0.1616,0.6557, 0.2076,0.6557, 0.2536,0.6557, 0.2996,0.6557, 0.3459,0.6557, 0.3919,0.6557, 0.4382,0.6557, 0.4841,0.6557, 0.5305,0.6557, 0.5764,0.6557, 0.6347,0.6557, 0.7079,0.6557, 0.7540,0.6557, 0.8005,0.6557, 0.8617,0.6574, 0.9078,0.6574, 0.9539,0.6574, 1.0000,0.5838 };
	const float pos_row3[] = { NAN,NAN, 0.0177,0.5104, 0.0813,0.5104, 0.1270,0.5104, 0.1735,0.5104, 0.2192,0.5104, 0.2654,0.5104, 0.3113,0.5104, 0.3578,0.5104, 0.4035,0.5104, 0.4496,0.5104, 0.4961,0.5104, 0.5418,0.5104,    NAN,   NAN, 0.6172,0.5104,    NAN,   NAN,    NAN,   NAN,    NAN,   NAN, 0.8617,0.5104, 0.9078,0.5104, 0.9539,0.5104 };
	const float pos_row4[] = { NAN,NAN, 0.0292,0.3617,    NAN,   NAN, 0.1040,0.3617, 0.1502,0.3617, 0.1959,0.3617, 0.2422,0.3617, 0.2882,0.3617, 0.3345,0.3617, 0.3808,0.3617, 0.4270,0.3617, 0.4731,0.3617, 0.5193,0.3617,    NAN,   NAN, 0.6057,0.3617,    NAN,   NAN, 0.7540,0.3617,    NAN,   NAN, 0.8617,0.3634, 0.9078,0.3634, 0.9539,0.3634, 1.0000,0.2904 };
	const float pos_row5[] = { NAN,NAN, 0.0114,0.2156, 0.0691,0.2156, 0.1266,0.2156,    NAN,   NAN,    NAN,   NAN, 0.3001,0.2156,    NAN,   NAN,    NAN,   NAN,    NAN,   NAN, 0.4730,0.2156, 0.5000,0.0000, 0.5306,0.2156, 0.5766,0.2156, 0.6344,0.2156, 0.7079,0.2156, 0.7540,0.2156, 0.8005,0.2156,    NAN,   NAN, 0.8853,0.2156, 0.9539,0.2149 };
	static const float* pos_rows[6] = { pos_row0,pos_row1,pos_row2,pos_row3,pos_row4,pos_row5 };


	class device_blackwidow :public razer_device {
	public:
		device_blackwidow() {
			row_count = 6;
			keys = rows;
			width = device_width;
			height_ratio = height_multi;
			key_virtual_positions = pos_rows;
			device_packet_size = 0x47;
			last_key_index = 0x15;
			init();
		}
		key_position GetKeyPosition(KeyID key) override {
			switch (key) {
			case ESC:			return key_position{ 0, 1 };
			case F1:			return key_position{ 0, 3 };
			case F2:			return key_position{ 0, 4 };
			case F3:			return key_position{ 0, 5 };
			case F4:			return key_position{ 0, 6 };
			case F5:			return key_position{ 0, 7 };
			case F6:			return key_position{ 0, 8 };
			case F7:			return key_position{ 0, 9 };
			case F8:			return key_position{ 0,10 };
			case F9:			return key_position{ 0,11 };
			case F10:			return key_position{ 0,12 };
			case F11:			return key_position{ 0,13 };
			case F12:			return key_position{ 0,14 };
			case PRTSC:			return key_position{ 0,15 };
			case SCRLK:			return key_position{ 0,16 };
			case BREAK:			return key_position{ 0,17 };
			case TILDE:			return key_position{ 1, 1 };
			case _1:			return key_position{ 1, 2 };
			case _2:			return key_position{ 1, 3 };
			case _3:			return key_position{ 1, 4 };
			case _4:			return key_position{ 1, 5 };
			case _5:			return key_position{ 1, 6 };
			case _6:			return key_position{ 1, 7 };
			case _7:			return key_position{ 1, 8 };
			case _8:			return key_position{ 1, 9 };
			case _9:			return key_position{ 1,10 };
			case _0:			return key_position{ 1,11 };
			case MINUS:			return key_position{ 1,12 };
			case EQUALS:		return key_position{ 1,13 };
			case BACKSPACE:		return key_position{ 1,14 };
			case INS:			return key_position{ 1,15 };
			case HOME:			return key_position{ 1,16 };
			case PUP:			return key_position{ 1,17 };
			case NUM_LK:		return key_position{ 1,18 };
			case NUM_FSLASH:	return key_position{ 1,19 };
			case NUM_ASTRICKS:	return key_position{ 1,20 };
			case NUM_MINUS:		return key_position{ 1,21 };
			case TAB:			return key_position{ 2, 1 };
			case Q:				return key_position{ 2, 2 };
			case W:				return key_position{ 2, 3 };
			case E:				return key_position{ 2, 4 };
			case R:				return key_position{ 2, 5 };
			case T:				return key_position{ 2, 6 };
			case Y:				return key_position{ 2, 7 };
			case U:				return key_position{ 2, 8 };
			case I:				return key_position{ 2, 9 };
			case O:				return key_position{ 2,10 };
			case P:				return key_position{ 2,11 };
			case LBRACKET:		return key_position{ 2,12 };
			case RBRACKET:		return key_position{ 2,13 };
			case BSLASH:		return key_position{ 2,14 };
			case DEL:			return key_position{ 2,15 };
			case END:			return key_position{ 2,16 };
			case PDOWN:			return key_position{ 2,17 };
			case NUM_7:			return key_position{ 2,18 };
			case NUM_8:			return key_position{ 2,19 };
			case NUM_9:			return key_position{ 2,20 };
			case NUM_PLUS:		return key_position{ 2,21 };
			case CAPS:			return key_position{ 3, 1 };
			case A:				return key_position{ 3, 2 };
			case S:				return key_position{ 3, 3 };
			case D:				return key_position{ 3, 4 };
			case F:				return key_position{ 3, 5 };
			case G:				return key_position{ 3, 6 };
			case H:				return key_position{ 3, 7 };
			case J:				return key_position{ 3, 8 };
			case K:				return key_position{ 3, 9 };
			case L:				return key_position{ 3,10 };
			case SEMI:			return key_position{ 3,11 };
			case APOS:			return key_position{ 3,12 };
			case ENTER:			return key_position{ 3,14 };
			case NUM_4:			return key_position{ 3,18 };
			case NUM_5:			return key_position{ 3,19 };
			case NUM_6:			return key_position{ 3,20 };
			case LSHIFT:		return key_position{ 4, 1 };
			case Z:				return key_position{ 4, 3 };
			case X:				return key_position{ 4, 4 };
			case C:				return key_position{ 4, 5 };
			case V:				return key_position{ 4, 6 };
			case B:				return key_position{ 4, 7 };
			case N:				return key_position{ 4, 8 };
			case M:				return key_position{ 4, 9 };
			case COMMA:			return key_position{ 4,10 };
			case PERIOD:		return key_position{ 4,11 };
			case FSLASH:		return key_position{ 4,12 };
			case RSHIFT:		return key_position{ 4,14 };
			case UP:			return key_position{ 4,16 };
			case NUM_1:			return key_position{ 4,18 };
			case NUM_2:			return key_position{ 4,19 };
			case NUM_3:			return key_position{ 4,20 };
			case NUM_ENTER:		return key_position{ 4,21 };
			case LCTRL:			return key_position{ 5, 1 };
			case WINDOWS:		return key_position{ 5, 2 };
			case LALT:			return key_position{ 5, 3 };
			case SPACE:			return key_position{ 5, 6 };
			case RALT:			return key_position{ 5,10 };
			case FN:			return key_position{ 5,12 };
			case MENU:			return key_position{ 5,13 };
			case RCTRL:			return key_position{ 5,14 };
			case LEFT:			return key_position{ 5,15 };
			case DOWN:			return key_position{ 5,16 };
			case RIGHT:			return key_position{ 5,17 };
			case NUM_0:			return key_position{ 5,19 };
			case NUM_DEL:		return key_position{ 5,20 };
			default:		    return key_position{-1,-1 };
			} 
		}
	};
}