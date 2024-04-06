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

	class device_tartarusv2 :public razer_device {
	public:
		device_tartarusv2() {
			row_count = 4;
			keys = rows;
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
