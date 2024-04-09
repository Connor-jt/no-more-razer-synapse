#pragma once
#include "../Devices/DeviceTemplate.h"
#include "DebugWave.h"
#include "FlatCycle.h"
#include "Pong.h"
#include "Spelling.h"
#include "Wave.h"

namespace RazerEffects {
	class RazerEffect {
    protected:
		RazerDevice::razer_device* target_device = nullptr;
        RazerEffect() {};
    public:
		virtual void run() { throw new std::exception("unimplemented!!"); }
	};

}