
#include "RazerIO.h"
#include "DeviceInterface.h"

/*// //// TODO //// ////
1. function for when device un/plugged
2. figure out how to disable custom colorings for all devices (only mouse pad is working)
3. add in theme generator objects that can be attached to any device
*/

struct loaded_device {
    RazerIO::discovered_device device_details;
    RazerDevice::razer_device* device_data;
};

static string sample_text = "Hello world! amongus imposter 100 burger king foot letuce 15 dollar fortnite card";
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
    
    // temporary effect thing
    int curr_starting_cycle = 2;
    // temp word effect thing
    int character_index = 0;
    int max_character_ticks = 15;
    int character_ticks = max_character_ticks;

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
                    // temp darkening
                    if (curr_device.device_details.pid == RazerDeviceManagement::BlackWidow) {
                        new_val.R *= 0.1;
                        new_val.G *= 0.1;
                        new_val.B *= 0.1;
                    }
                    curr_device.device_data->SetKey(row, col, new_val);
                }
            }


            // do word spelling pattern for keyboard
            if (curr_device.device_details.pid == RazerDeviceManagement::BlackWidow) {
                float current_key_color = (float)character_ticks / (float)max_character_ticks;
                float next_key_color = 1.0f - current_key_color;

                auto keypos = curr_device.device_data->GetKeyPosition((RazerDevice::KeyID)sample_text[character_index]);
                if (keypos.row != -1 && keypos.col != -1)
                    curr_device.device_data->SetKey(keypos.row, keypos.col, RazerDevice::RGB_float{ current_key_color,current_key_color,current_key_color });

                int next_char_index = character_index+1;
                if (next_char_index >= sample_text.size()) next_char_index = 0;

                keypos = curr_device.device_data->GetKeyPosition((RazerDevice::KeyID)sample_text[next_char_index]);
                if (keypos.row != -1 && keypos.col != -1)
                    curr_device.device_data->SetKey(keypos.row, keypos.col, RazerDevice::RGB_float{ next_key_color,next_key_color,next_key_color });

                character_ticks--; 
                if (character_ticks == 0) {
                    character_ticks = max_character_ticks;
                    character_index = next_char_index;
                }
            }

            // then push changes
            for (int row = 0; row < curr_device.device_data->row_count; row++) {
                DWORD bytes_returned = 0;
                RazerIO::SendDataToDevice(curr_device.device_details.driver_handle, 0x88883140, 0, 0, 0, 0, &bytes_returned, 0); // no idea what this one actually does, but razer calls it super frequently
                RazerIO::SendDataToDevice(curr_device.device_details.driver_handle, 0x88883010, &curr_device.device_data->data_buffers[row], sizeof(RazerDevice::razer_rgb_data), 0, 0, &bytes_returned, 0);
            }

            Sleep(50);
        }
        
        // reset animation loop
        curr_starting_cycle++;
        if (curr_starting_cycle >= 6)
            curr_starting_cycle = 0;
    }

    // UNFINISHED !!!!
    for (int i = 0; i < devices.size(); i++) {
        auto curr_device = devices[i];
        DWORD bytes_returned = 0;
        switch (curr_device.device_details.pid) {
        case RazerDeviceManagement::TartarusV2:
        case RazerDeviceManagement::BlackWidow:
        case RazerDeviceManagement::GoliathusE: {
            char doody[] = { 0x00,0x00,0x1F,0x00,0x00,0x00,0x06,0x0F,0x02,0x01,0x00,0x03,0x00,0x28,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x21,0x00 };
            RazerIO::SendDataToDevice(curr_device.device_details.driver_handle, 0x88883010, doody, sizeof(doody), 0, 0, &bytes_returned, 0);
            wcout << L"\nUnloaded device: " << curr_device.device_details.name;
            break;}
        }
    }

}


