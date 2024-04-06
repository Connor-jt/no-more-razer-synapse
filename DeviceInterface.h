#pragma once
#include "Devices/BlackWidow.h"
#include "Devices/TartarusV2.h"
#include "Devices/Goliathus.h"

namespace RazerDeviceManagement {
	using namespace razer_blackwidow;
	using namespace razer_tartarusv2;
	using namespace razer_goliathus;
	enum deviceID {
		BlackWidow = 0x241,
		TartarusV2 = 0x22b,
		GoliathusE = 0xc02,
	};

	static razer_device* instaniate_device(int pid){
		switch (pid) {
		case BlackWidow: return new razer_blackwidow::device_blackwidow();
		case TartarusV2: return new razer_tartarusv2::device_tartarusv2();
        case GoliathusE: return new razer_goliathus::device_goliathus();
		default: return (razer_device*)-1;
		}
	}
	
}


