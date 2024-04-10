#pragma once
#include "../Devices/DeviceTemplate.h"
using namespace RazerDevice;
namespace RazerEffects {
	class RazerEffect {
	private:
		unsigned char key_filters[32] = {0};
		bool FilterExcludes = true; // true means it ignores these keys, false means it ignores anything other than these keys
    protected:
		razer_device* target_device = nullptr;
        RazerEffect() {};
		bool IsAcceptedKey(KeyID id) {
			if (id == NONE) return false;
			int index = (unsigned char)id >> 3;
			unsigned char bit_mask = 1 << ((unsigned char)id & 0b111);
			bool test1 = (key_filters[index] & bit_mask);
			return test1 ^ FilterExcludes;
		}
    public:
		virtual void run() { throw new std::exception("unimplemented!!"); }
		void SetFilter(KeyID id, bool state) {
			int index = (unsigned char)id >> 3;
			unsigned char bit_mask = 1 << ((unsigned char)id & 0b111);
			if (state) key_filters[index] |= bit_mask;
			else	   key_filters[index] &= ~bit_mask;
		}
		void SetFilters(KeyID* ids, int id_count, bool state) {
			for (int i = 0; i < id_count; i++)
				SetFilter(ids[i], state);
		}
		void SetFilterState(bool new_state) {
			FilterExcludes = new_state;
		}
	};

}