
#include "RazerIO.h"
#include "DeviceInterface.h"
#include "Effects/EffectsTemplate.h"

/*// //// TODO //// ////
1. function for when device un/plugged
2. do smooth wave effect
3. fix bounce effect
4. do spectrum cycling effect
5. fix cleaning up data when shutting down device
6. fix RGB packet IO functions making it super slow
7. remove check in/out system and replace with original checksum before posting
8. get proper device measurements
*/

class LoadedDevice {
public:
    RazerIO::discovered_device device_details;
    RazerDevice::razer_device* device_data;
    vector<RazerEffects::RazerEffect*> effects = {};
    void DoEffects() {
        for (int i = 0; i < effects.size(); i++)
            effects[i]->run();
    }
    void PostData() {
        DWORD bytes_returned = 0;
        for (int row = 0; row < device_data->row_count; row++) {
            RazerIO::SendDataToDevice(device_details.driver_handle, 0x88883140, 0, 0, 0, 0, &bytes_returned, 0); // no idea what this one actually does, but razer calls it super frequently
            RazerIO::SendDataToDevice(device_details.driver_handle, 0x88883010, &device_data->data_buffers[row], sizeof(RazerDevice::razer_rgb_data), 0, 0, &bytes_returned, 0);
        }
    }
    void DisableLighting() {
        DWORD bytes_returned = 0;
        // keep an explicit list of things that this byte sequence is confirmed to be compatible with
        switch (device_details.pid) {
        case RazerDeviceManagement::TartarusV2:
        case RazerDeviceManagement::BlackWidow:
        case RazerDeviceManagement::GoliathusE:
            char lights_off_packet[] = { 0x00,0x00,0x1F,0x00,0x00,0x00,0x06,0x0F,0x02,0x01,0x00,0x03,0x00,0x28,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x21,0x00 };
            RazerIO::SendDataToDevice(device_details.driver_handle, 0x88883140, 0, 0, 0, 0, &bytes_returned, 0);
            RazerIO::SendDataToDevice(device_details.driver_handle, 0x88883010, lights_off_packet, sizeof(lights_off_packet), 0, 0, &bytes_returned, 0);
            break;
        }
    }
    ~LoadedDevice() {
        DisableLighting();
        delete device_data;
        for (int i = 0; i < effects.size(); i++) delete effects[i];
        wcout << L"\nUnloaded device: " << device_details.name;
    }
};

class DeviceManager {
    vector<LoadedDevice*> devices = {};
    void RefreshDevices() {

    }
};

int main(){
    
    cout << "\nScanning for devices...";
    // find devices and generate data objects
    auto found_devices = RazerIO::try_load_devices();
    for (int i = 0; i < found_devices.size(); i++){
        auto dev_data_object = RazerDeviceManagement::instaniate_device(found_devices[i].pid);
        if (dev_data_object == (RazerDevice::razer_device*)-1) {
            wcout << L"\nUnsupported device, disregarding: " << found_devices[i].name;
            continue;}
        vector<RazerEffects::RazerEffect*> effects = {};
        effects.push_back(new RazerEffects::DebugWaveEffect{ dev_data_object });
        if (found_devices[i].pid == RazerDeviceManagement::BlackWidow)
            effects.push_back(new RazerEffects::BounceEffect{ dev_data_object });
        devices.push_back(new LoadedDevice{ found_devices[i], dev_data_object, effects });
    }
    
    for (;;) {
        for (int device_index = 0; device_index < devices.size(); device_index++) {
            auto curr_device = devices[device_index];
            curr_device->DoEffects();
            curr_device->PostData();
            Sleep(50);
        }
    }

    // call all of the device destructors
    for (int i = 0; i < devices.size(); i++) delete devices[i];
    devices.clear();
    return 1;
}


