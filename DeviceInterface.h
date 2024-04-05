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
		case 0x1542: return new razer_blackwidow::device_blackwidow();
		case 0x0001: return new razer_tartarusv2::device_tartarusv2();
		//case 0x0002: return new razer_goliathus::device_blackwidow();
		default: return (razer_device*)-1;
		}
	}
	
}


