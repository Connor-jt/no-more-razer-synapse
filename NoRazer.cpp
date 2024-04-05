
#include "RazerIO.h"
#include "DeviceInterface.h"

int main(){
    
    cout << "\nScanning for devices...";

    auto var = RazerIO::try_load_devices();

    char breakpoint_test = 0;
    //float current = 0.0f;
    //for (;;) {
    //    // basic pattern to make colors increment and reset
    //    for (int row = 0; row < created_device->row_count; row++) {
    //        int cycle = 0;
    //        for (int col = 0; col < created_device->keys[row].size; col++) {
    //            if (created_device->keys[row].ptr[col] == NONE)
    //                continue;
    //            // set RGB floats??
    //            auto rgb_val = created_device->GetKey(row, col);
    //            //rgb_val.R += 0.034 * ((col+1.0f) / 4.0f);
    //            //if (rgb_val.R >= 1.0f) rgb_val.R = 0.0f;
    //            rgb_val.R = 0.0f;
    //            rgb_val.G = 0.0f;
    //            rgb_val.B = 0.0f;
    //            if (cycle == 0)
    //                rgb_val.R = 1.0f;
    //            else if (cycle == 1)
    //                rgb_val.G = 1.0f;
    //            else rgb_val.B = 1.0f;


    //            created_device->SetKey(row, col, rgb_val);

    //            cycle++;
    //            if (cycle >= 3) cycle = 0;
    //        }

    //        DWORD bytes_returned = 0;
    //        razer_rgb_data* debug_test = &created_device->data_buffers[row];
    //        //SendDataToDevice(driver_handle, 0x88883014, 0, 0, 0, 0, &bytes_returned, 0);
    //        SendDataToDevice(driver_handle, 0x88883140, 0, 0, 0, 0, &bytes_returned, 0);
    //        SendDataToDevice(driver_handle, 0x88883010, &created_device->data_buffers[row], sizeof(razer_rgb_data), 0, 0, &bytes_returned, 0);
    //        Sleep(50);
    //    }
    //    current += 0.075;



}


