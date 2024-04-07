#pragma once
#include "DeviceTemplate.h"
namespace razer_tartarusv2 {
	using namespace RazerDevice;
	const char row1[] = { _1,_2,_3,_4,_5 };
	const char row2[] = { TAB,Q,W,E,R };
	const char row3[] = { CAPS,A,S,D,F };
	const char row4[] = { LSHIFT,Z,X,C,WHEEL,SPACE };
	static keyboard_row rows[4] = { keyboard_row{sizeof(row1),row1},
								   keyboard_row{sizeof(row2),row2},
								   keyboard_row{sizeof(row3),row3},
								   keyboard_row{sizeof(row4),row4} };
	const float device_width = 1.0f;
	const float height_multi = 1.1471215788745488f;
	const float pos_row0[] = { 0.0000,0.9629, 0.1801,1.0000, 0.3677,1.0000, 0.5553,1.0000, 0.7504,1.0000 };
	const float pos_row1[] = { 0.0075,0.8059, 0.1876,0.8452, 0.3717,0.8452, 0.5553,0.8452, 0.7479,0.8452 };
	const float pos_row2[] = { 0.0162,0.6515, 0.1969,0.6893, 0.3772,0.6893, 0.5567,0.6893, 0.7377,0.6893 };
	const float pos_row3[] = { 0.0287,0.4915, 0.2059,0.5333, 0.3789,0.5333, 0.5540,0.5333, 0.7333,0.4679, 1.0000,0.0000 };
	static const float* pos_rows[4] = { pos_row0,pos_row1,pos_row2,pos_row3 };

	class device_tartarusv2 :public razer_device {
	public:
		device_tartarusv2() {
			row_count = 4;
			keys = rows;
			width = device_width;
			height_ratio = height_multi;
			key_virtual_positions = pos_rows;
			device_packet_size = 0x17;
			last_key_index = 0x5;
			init();
		}
		key_position GetKeyPosition(KeyID key) override {
			switch (key) {
			case _1:		return key_position{ 0, 0 };
			case _2:		return key_position{ 0, 1 };
			case _3:		return key_position{ 0, 2 };
			case _4:		return key_position{ 0, 3 };
			case _5:		return key_position{ 0, 4 };
			case TAB:		return key_position{ 1, 0 };
			case Q:			return key_position{ 1, 1 };
			case W:			return key_position{ 1, 2 };
			case E:			return key_position{ 1, 3 };
			case R:			return key_position{ 1, 4 };
			case CAPS:		return key_position{ 2, 0 };
			case A:			return key_position{ 2, 1 };
			case S:			return key_position{ 2, 2 };
			case D:			return key_position{ 2, 3 };
			case F:			return key_position{ 2, 4 };
			case LSHIFT:	return key_position{ 3, 0 };
			case Z:			return key_position{ 3, 1 };
			case X:			return key_position{ 3, 2 };
			case C:			return key_position{ 3, 3 };
			case WHEEL:		return key_position{ 3, 4 };
			case SPACE:		return key_position{ 3, 5 };
			default:		return key_position{-1,-1 };
			}
		}
	};
}
