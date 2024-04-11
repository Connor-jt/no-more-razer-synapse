
#include "RazerIO.h"
#include "DeviceInterface.h"

/*// //// TODO //// ////
1. function for when device un/plugged
2. finalize wave effect (adding in extra values and exposing them on creation)
3. fix bounce effect??
4. threading for multiple devices (otherwise the effects become very slow as the data passing functions likely perform a lot of waits and such)
5. get proper device measurements
6. use ownership percentage instead of simple additive boolean, for adding/setting RGB colors, this would be important for having anything underneath the wave effect
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

        auto wave_effect = new RazerEffects::CycleEffect(curr_device->device_data);
        curr_device->effects.push_back(wave_effect);
        //auto wave_effect = new RazerEffects::WaveEffect(curr_device->device_data);
        //curr_device->effects.push_back(wave_effect);
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
            //Sleep(20); // bit of rest time for the cpu // enough is provided by the post data call i believe
        }
    }

    device_manager.CloseAllDevices();
    return 1;
}


