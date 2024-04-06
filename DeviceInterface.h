#pragma once
#include "Devices/BlackWidow.h"
#include "Devices/TartarusV2.h"
#include "Devices/Goliathus.h"

namespace RazerDeviceManagement {
	using namespace razer_blackwidow;
	using namespace razer_tartarusv2;
	using namespace razer_goliathus;
	

	static razer_device* instaniate_device(int pid){
		switch (pid) {
		case 0x241: return new razer_blackwidow::device_blackwidow();
		case 0x22b: return new razer_tartarusv2::device_tartarusv2();
        case 0xc02: return new razer_goliathus::device_goliathus();
		default: return (razer_device*)-1;
		}
	}
	
}


