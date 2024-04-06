
#include "RazerIO.h"
#include "DeviceInterface.h"


struct loaded_device {
    RazerIO::discovered_device device_details;
    RazerDevice::razer_device* device_data;
};


int main(){
    
    cout << "\nScanning for devices...";
    // find devices and generate data objects
    vector<loaded_device> devices = {};
    auto found_devices = RazerIO::try_load_devices();
    for (int i = 0; i < found_devices.size(); i++){
        auto dev_data_object = RazerDeviceManagement::instaniate_device(found_devices[i].pid);
        if (dev_data_object == (RazerDevice::razer_device*)-1) {
            wcout << L"\nUnsupported device, disregarding: " << found_devices[i].name;
            continue;}
        devices.push_back(loaded_device{ found_devices[i], dev_data_object });
    }
    
    int curr_starting_cycle = 0;
    for (;;) {
        for (int device_index = 0; device_index < devices.size(); device_index++) {
            auto curr_device = devices[device_index];
            // basic pattern to make colors increment and reset
            for (int row = 0; row < curr_device.device_data->row_count; row++) {
                int cycle = curr_starting_cycle;
                for (int col = 0; col < curr_device.device_data->keys[row].size; col++) {
                    if (curr_device.device_data->keys[row].ptr[col] == RazerDevice::KeyID::NONE) {
                        //cycle++;
                        //if (cycle == 6) cycle = 0;
                        continue;
                    }

                    RazerDevice::RGB_float new_val = { 0.0f,0.0f,0.0f };
                    cycle++;
                    switch (cycle) {
                    case 1:
                        new_val.R = 1.0f;
                        break;
                    case 2:
                        new_val.R = 1.0f;
                        new_val.G = 1.0f;
                        break;
                    case 3:
                        new_val.G = 1.0f;
                        break;
                    case 4:
                        new_val.G = 1.0f;
                        new_val.B = 1.0f;
                        break;
                    case 5:
                        new_val.B = 1.0f;
                        break;
                    case 6:
                        new_val.B = 1.0f;
                        new_val.R = 1.0f;
                        cycle = 0;
                        break;
                    }
                    curr_device.device_data->SetKey(row, col, new_val);
                }

                DWORD bytes_returned = 0;
                auto debug_test = &curr_device.device_data->data_buffers[row];
                RazerIO::SendDataToDevice(curr_device.device_details.driver_handle, 0x88883140, 0, 0, 0, 0, &bytes_returned, 0); // no idea what this one actually does, but razer calls it super frequently
                curr_device.device_data->SetChecksum(row); // TODO: since this is a simple checksum, we could actually just xor in&out any changes as we make them, opposed to recaculating
                RazerIO::SendDataToDevice(curr_device.device_details.driver_handle, 0x88883010, &curr_device.device_data->data_buffers[row], sizeof(RazerDevice::razer_rgb_data), 0, 0, &bytes_returned, 0);
                Sleep(50);
            }
        }
        // reset animation loop
        curr_starting_cycle++;
        if (curr_starting_cycle >= 6)
            curr_starting_cycle = 0;
    }

}


