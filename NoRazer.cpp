
#include "RazerIO.h"
#include "DeviceInterface.h"

/*// //// TODO //// ////
1. function for when device un/plugged
2. do smooth wave effect
3. fix bounce effect
4. do spectrum cycling effect
5. fix cleaning up data when shutting down device
6. fix RGB packet IO functions making it super slow
7. get proper device measurements
*/

static DeviceManagement::DeviceManager device_manager = {};
int main(){
    
    // find devices and give them themes
    auto found_devices = device_manager.RefreshDevices();
    for (int i = 0; i < found_devices.size(); i++) {
        auto curr_device = found_devices[i];
        curr_device->effects.push_back(new RazerEffects::DebugWaveEffect{ curr_device->device_data });
        if (curr_device->device_details.pid == DeviceManagement::BlackWidow)
            curr_device->effects.push_back(new RazerEffects::BounceEffect{ curr_device->device_data });
    }
    
    for (int tick_index = 0; tick_index < 100; tick_index++) {
        for (int device_index = 0; device_index < device_manager.devices.size(); device_index++) {
            auto curr_device = device_manager.devices[device_index];
            curr_device->DoEffects();
            curr_device->PostData();
            Sleep(20);
        }
    }

    device_manager.CloseAllDevices();
    return 1;
}


