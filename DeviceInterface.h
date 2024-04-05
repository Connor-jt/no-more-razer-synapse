#pragma once
#include "Devices/BlackWidow.h"
#include "Devices/TartarusV2.h"
#include "Devices/Goliathus.h"

namespace RazerDeviceManagement {
	using namespace razer_blackwidow;
	using namespace razer_tartarusv2;
	using namespace razer_goliathus;
	

	static enum supported_devices {
		golaithus = 0,
		blackwidow = 1,
		tartarusv2 = 2,
	};
	static void instaniate_device(){

	}
	
}


