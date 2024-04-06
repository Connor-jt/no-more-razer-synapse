#pragma once
#include "DeviceTemplate.h"
namespace razer_goliathus {
	using namespace RazerDevice;
	const char row1[] = { ZONE1 };
	static keyboard_row rows[1] = { keyboard_row{sizeof(row1),row1},};

	class device_goliathus :public razer_device {
	public:
		device_goliathus() {
			row_count = 1;
			keys = rows;
			device_packet_size = 0x08;
			last_key_index = 0x0;
			init();
		}
		key_position GetKeyPosition(KeyID key) override {
			switch (key) {
			case ZONE1:		return key_position{ 0, 0 };
			default:		return key_position{-1,-1 };
			}
		}
	};
}