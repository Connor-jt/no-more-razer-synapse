
#include "RazerIO.h"
#include "DeviceInterface.h"

/*// //// TODO //// ////
1. function for when device un/plugged
2. gradient thing for wave effect (split out as separate thing??)
3. fix bounce effect
4. do spectrum cycling effect
5. threading for multiple devices
6. get proper device measurements
*/

static DeviceManagement::DeviceManager device_manager = {};
int main(){
    
    RazerDevice::KeyID keys_to_color[4] = {W,A,S,D};

    // find devices and give them themes
    auto found_devices = device_manager.RefreshDevices();
    for (int i = 0; i < found_devices.size(); i++) {
        auto curr_device = found_devices[i];

        //auto wave_effect = new RazerEffects::DebugWaveEffect(curr_device->device_data);
        //curr_device->effects.push_back(wave_effect);

        auto wave_effect = new RazerEffects::WaveEffect(curr_device->device_data);
        curr_device->effects.push_back(wave_effect);
        //auto static_effect = new RazerEffects::StaticEffect(curr_device->device_data, RGB_float{1.0f, 0.0f, 0.0f});
        //static_effect->SetFilters(keys_to_color, 4, true);
        //static_effect->SetFilterState(false);
        //curr_device->effects.push_back(static_effect);
        //if (curr_device->device_details.pid == DeviceManagement::BlackWidow)
        //    curr_device->effects.push_back(new RazerEffects::BounceEffect{ curr_device->device_data });
    }
    
    for (int tick_index = 0; tick_index < 1000; tick_index++) {
        for (int device_index = 0; device_index < device_manager.devices.size(); device_index++) {
            auto curr_device = device_manager.devices[device_index];
            if (curr_device->effects.size()) { // skip devices that we haven't set up any effects for
                curr_device->DoEffects();
                curr_device->PostData();
            }
            //Sleep(20); // bit of rest time for the cpu
        }
    }

    device_manager.CloseAllDevices();
    return 1;
}


