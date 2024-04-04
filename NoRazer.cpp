// NoRazer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//typedef const USHORT USAGE;
//typedef const char* LPCSTR;
#include <windows.h>
#include <initguid.h>
#include <usbioctl.h>
//#pragma comment (lib, "Hidclass.lib")
#include <usbiodef.h>
#include <ntddstor.h>
#include <Hidclass.h>

#include <iostream>

#include <winusb.h>
#pragma comment (lib, "Setupapi.lib")
#include <setupapi.h>
#include <cfgmgr32.h>



#include <errhandlingapi.h>
#include <FileApi.h>
#include <ioapiset.h>
#include <synchapi.h>

#pragma comment (lib, "hid.lib")
#include <hidsdi.h>
#include <hidpi.h> 


#include <vector>
#include <string>
using namespace std;


#pragma pack(push, 1)
struct razer_rgb_data {
    struct RGB {
        unsigned char R;
        unsigned char B;
        unsigned char G;};
    unsigned short unk0x00      =  0;
    unsigned short unk0x02      = 31;
    unsigned short unk0x04      =  0;
    unsigned char  unk0x06[3]   = {0x47,0x0F,0x03};
    unsigned short unk0x09      =  0;
    unsigned short column_index =  0; // USER SET
    unsigned short key_count    =  0; // USER SET
    unsigned short unk0x0F      =  0;
    RGB            keys[24];
    unsigned short unk0x59      =  0; // seems kinda like a checksum of sorts
};
#pragma pack(pop)

bool SendDataToDevice(HANDLE device, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, DWORD* bytes_returned, LPOVERLAPPED lpOverlapped){

    BOOL device_output;
    DWORD lpBytesReturned;


    HANDLE thread_event = 0x0;
    if (lpOverlapped == (LPOVERLAPPED)0x0) {
        thread_event = CreateEventW((LPSECURITY_ATTRIBUTES)0x0, 1, 0, (LPCWSTR)0x0);
        if (thread_event == 0x0) return false; // failed to create thread??
        
        ResetEvent(thread_event);
        // lpOverlapped = 0x0; // not sure how this is supposed to work? maybe the decompiler pulled some bad code
    }


    lpBytesReturned = 0;
    device_output = DeviceIoControl(device, dwIoControlCode, lpInBuffer, nInBufferSize, lpOutBuffer, nOutBufferSize, &lpBytesReturned, lpOverlapped);
    if (!device_output) {
        DWORD last_error = GetLastError();
        if (last_error == 0xea) // 'More data is available'
            goto LAB_1003a94b;
        
        if (last_error != 0x3e5) {
            if (last_error == 1)
                return false;
            else return true;
        }
        
        
        // error is 'Overlapped I/O operation is in progress'
        // seems like this is all unimportant & could just be skipped !!!
        if (!WaitForSingleObject(thread_event, 2000)) {
            BOOL overlap_result = GetOverlappedResult(device, lpOverlapped, &lpBytesReturned, 0);
            if (overlap_result != 0)
                goto LAB_1003a94b;
        }else {
            // exception L"CHIDDriver::DeviceControl : TimeOut\n";
            CancelIoEx(device, lpOverlapped);
            GetOverlappedResult(device, lpOverlapped, &lpBytesReturned, 1);
            // why dont we return 
        }
    }
    else {
    LAB_1003a94b:
        if (bytes_returned != (DWORD*)0x0)
            *bytes_returned = lpBytesReturned;
    }
    return true;
}






vector<wstring> separate_device_descriptor(const wstring source) {
    vector<wstring> result = {};
    int section_index = 0;
    int section_chars_begin_index = 0;
    for (int index = 0; index < source.size(); index++) {
        wchar_t current_char = source[index];
        if (current_char == '&') {
            result.push_back(source.substr(section_chars_begin_index, index-section_chars_begin_index));
            section_chars_begin_index = index + 1;
            section_index++;
    }}
    return result;
}
struct device_path_data {
    int vid, pid, mi = 0;
    vector<int> col = {};
    int hwid1, hwid2, hwid3, hwid4 = 0;
    wstring class_guid;
};
device_path_data digest_device_path(LPCWSTR device_path) {
    device_path_data result = {};
    // im not a fan of the excessive substringing to get all this going, but it keeps it concise
    wstring path_descriptor  = L"";
    wstring device_ids = L"";
    wstring hardware_ids = L"";
    // iterate through device path string and map each section
    {   int descriptor_index = 0;
        int last_descriptor_index = 0;
        for (int index = 0;;index++) {
            wchar_t current_char = device_path[index];
            if (current_char == '#') {
                switch (descriptor_index) {
                case 0: path_descriptor   = wstring(device_path+last_descriptor_index, index-last_descriptor_index); break;
                case 1: device_ids        = wstring(device_path+last_descriptor_index, index-last_descriptor_index); break;
                case 2: hardware_ids      = wstring(device_path+last_descriptor_index, index-last_descriptor_index); break;
                case 3: result.class_guid = wstring(device_path+last_descriptor_index, index-last_descriptor_index); break;}
                last_descriptor_index = index + 1;
                descriptor_index++;
            } else if (current_char == '\0') break;
    }}
    // iterate through device ids and map the data
    {   vector<wstring> dev_ids = separate_device_descriptor(device_ids);
        for (int i = 0; i < dev_ids.size(); i++) {
            wstring curr_string = dev_ids[i];
            if (result.vid == 0 && curr_string.starts_with(L"vid_"))
                result.vid = stoi(curr_string.substr(4), nullptr, 16);
            else if (result.pid == 0 && curr_string.starts_with(L"pid_"))
                result.pid = stoi(curr_string.substr(4), nullptr, 16);
            else if (result.mi == 0 && curr_string.starts_with(L"mi_"))
                result.mi = stoi(curr_string.substr(3), nullptr, 16);
            else if (curr_string.starts_with(L"col"))
                result.col.push_back(stoi(curr_string.substr(3), nullptr, 16));
    }}
    // iterate through hardware ids and map them
    {   vector<wstring> physical_ids = separate_device_descriptor(hardware_ids);
        for (int i = 0; i < physical_ids.size(); i++) {
            switch (i) {
            case 0: result.hwid1 = stoi(physical_ids[i], nullptr, 16); break;
            case 1: result.hwid2 = stoi(physical_ids[i], nullptr, 16); break;
            case 2: result.hwid3 = stoi(physical_ids[i], nullptr, 16); break;
            case 3: result.hwid4 = stoi(physical_ids[i], nullptr, 16); break;}
    }}
    return result;
}


DEFINE_GUID(GUID_DEVINTERFACE_RAZER, 0xE3BE005DL, 0xD130, 0x4910, 0x88, 0xff, 0x09, 0xae, 0x02, 0xf6, 0x80, 0xe9);
DEFINE_DEVPROPKEY(DEVPKEY_Device_ContainerId, 0x8c7ed206, 0x3f8a, 0x4827, 0xb3, 0xab, 0xae, 0x9e, 0x1f, 0xae, 0xfc, 0x6c, 2);     // DEVPROP_TYPE_GUID
DEFINE_DEVPROPKEY(DEVPKEY_Device_BusReportedDeviceDesc, 0x540b947e, 0x8b40, 0x45bc, 0xa8, 0xa2, 0x6a, 0x0b, 0x89, 0x4c, 0xbd, 0xa2, 4);     // DEVPROP_TYPE_STRING
int find_parent_usb_device(int target_vid, int target_pid, PBYTE property_buffer) {
    HDEVINFO hDevInfo = SetupDiGetClassDevsW(&GUID_DEVINTERFACE_USB_DEVICE, 0, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE)
        return GetLastError();

    int device_index = 0;
    while (device_index++, true) {
        SP_DEVICE_INTERFACE_DATA interfaceData = {};
        interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
        if (!SetupDiEnumDeviceInterfaces(hDevInfo, 0, &GUID_DEVINTERFACE_USB_DEVICE, device_index, &interfaceData))
            break;
        SP_DEVINFO_DATA devinfo_data = {};
        devinfo_data.cbSize = sizeof(SP_DEVINFO_DATA);
        if (!SetupDiEnumDeviceInfo(hDevInfo, device_index, &devinfo_data))
            continue;
        char __interface_detail_data[0x20e] = {0};
        PSP_DEVICE_INTERFACE_DETAIL_DATA_W interface_detail_data = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)&__interface_detail_data;
        interface_detail_data->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        if (!SetupDiGetDeviceInterfaceDetailW(hDevInfo, &interfaceData, interface_detail_data, 0x20e, 0, 0))
            continue;

        // we're looking for a deivce that has a matching vid & pid
        auto interface_path_data = digest_device_path(interface_detail_data->DevicePath);
        if (interface_path_data.vid != target_vid && interface_path_data.vid != target_pid)
            continue;

        DEVPROPTYPE property_type = 0;
        DWORD required_size = 0;
        if (!SetupDiGetDevicePropertyW(hDevInfo, &devinfo_data, &DEVPKEY_Device_BusReportedDeviceDesc, &property_type, property_buffer, 0x200, &required_size, 0))
        {
            // GET DETAILS; RETURN THEM;
        }

        return 0; // success
    }
    return 621; // not an actual error, its just so we can track whats going on
}
HANDLE find_device_driver(int target_vid, int target_pid) {
    HDEVINFO hDevInfo = SetupDiGetClassDevsW(&GUID_DEVINTERFACE_RAZER, 0, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE)
        return (HANDLE) - 1;
    // iterate through drivers to find matching one
    for (int driver_index = 0;; driver_index++) {
        SP_DEVICE_INTERFACE_DATA interfaceData = {};
        interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
        if (!SetupDiEnumDeviceInterfaces(hDevInfo, 0, &GUID_DEVINTERFACE_RAZER, driver_index, &interfaceData))
            break;
        SP_DEVINFO_DATA devinfo_data = {};
        devinfo_data.cbSize = sizeof(SP_DEVINFO_DATA);
        if (!SetupDiEnumDeviceInfo(hDevInfo, driver_index, &devinfo_data))
            continue;
        char __interface_detail_data[0x20e] = { 0 };
        PSP_DEVICE_INTERFACE_DETAIL_DATA_W interface_detail_data = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)&__interface_detail_data;
        interface_detail_data->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
        if (!SetupDiGetDeviceInterfaceDetailW(hDevInfo, &interfaceData, interface_detail_data, 0x20e, 0, 0))
            continue;
        // then test if this driver matches our razer device
        auto interface_path_data = digest_device_path(interface_detail_data->DevicePath);
        if (interface_path_data.vid != target_vid && interface_path_data.vid != target_pid)
            continue;
        // map device and pass handle back
        return CreateFileW(interface_detail_data->DevicePath, 0, 3, (LPSECURITY_ATTRIBUTES)0x0, 3, 0x40000000, (HANDLE)0x0);
    }
    return (HANDLE)-1;
}


int try_load_devices(){
    
    /*HANDLE device_handle = CreateFileW(L"\\\\?\\rzcontrol#vid_1532&pid_0241&mi_00#8&288536a0&0#{e3be005d-d130-4910-88ff-09ae02f680e9}", 0, 3, (LPSECURITY_ATTRIBUTES)0x0, 3, 0x40000000, (HANDLE)0x0);
    if (device_handle == (HANDLE)-1)
    {
        char breakpoint_test = 0;
    }*/
    //return 0;



    HDEVINFO hDevInfo = SetupDiGetClassDevsW(&GUID_DEVINTERFACE_HID, 0, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE) 
        return GetLastError();

    // iterate all razer HID devices
    for (int device_index = 0;; device_index++) {

        SP_DEVICE_INTERFACE_DATA interfaceData = {};
        interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
        if (!SetupDiEnumDeviceInterfaces(hDevInfo, 0, &GUID_DEVINTERFACE_HID, device_index, &interfaceData))
            break; // this means we've reached the end of the list

        SP_DEVINFO_DATA devinfo_data = {};
        devinfo_data.cbSize = sizeof(SP_DEVINFO_DATA);
        if (!SetupDiEnumDeviceInfo(hDevInfo, device_index, &devinfo_data))
            continue;

        char __interface_detail_data[0x20e] = {0}; // number provided by razer via razer synapse 3 decompiled code
        PSP_DEVICE_INTERFACE_DETAIL_DATA_W interface_detail_data = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)&__interface_detail_data;
        interface_detail_data->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA); // razer says the answer is 6, although that is unlikely
        if (!SetupDiGetDeviceInterfaceDetailW(hDevInfo, &interfaceData, interface_detail_data, 0x20e, 0, 0))
            continue;

        // check for razer vendor id, skip any that aren't razer
        auto interface_path_data = digest_device_path(interface_detail_data->DevicePath);
        if (interface_path_data.vid != 0x1532 && interface_path_data.vid != 0x6e8) continue; 

        BYTE parent_property_buffer[0x200] = {0};
        int parent_result = find_parent_usb_device(interface_path_data.vid, interface_path_data.pid, parent_property_buffer);

        wcout << L"\nfound device: " << wstring((LPWSTR)parent_property_buffer);

        HANDLE driver_handle = find_device_driver(interface_path_data.vid, interface_path_data.vid);
        if (driver_handle == (HANDLE)-1)
            continue; // couldn't find driver;


        unsigned char color_index = 0;
        razer_rgb_data rgb_data_obj = {};
        rgb_data_obj.column_index = 0;
        rgb_data_obj.key_count = 21;
        int col_index = 0;
        for (;;) {
            // color in keys
            for (int i = 0; i < rgb_data_obj.key_count; i++) {
                rgb_data_obj.keys[i].R = 0;
                rgb_data_obj.keys[i].G = color_index;
                rgb_data_obj.keys[i].B = 0;
                color_index += 1; // naturally overflows back to 0
            }

            DWORD bytes_returned = 0;
            //SendDataToDevice(driver_handle, 0x88883014, 0, 0, 0, 0, &bytes_returned, 0);
            SendDataToDevice(driver_handle, 0x88883140, 0, 0, 0, 0, &bytes_returned, 0);
            SendDataToDevice(driver_handle, 0x88883010, &rgb_data_obj, sizeof(rgb_data_obj), 0, 0, &bytes_returned, 0);

            Sleep(300);
            rgb_data_obj.column_index++;
            if (rgb_data_obj.column_index > 5) rgb_data_obj.column_index = 0;
        }

        // console log found driver for device: device name
    }
    return 0;

    // //////////////////////////// //
    // PART 2 OF THE 2ND TEST !!!! //
    // ////////////////////////// //
   
    // HID\VID_1532&PID_0241&MI_01&COL01\7&31628AA2&0&0000
    //const wchar_t* devicePath = L"\\\\?\\hid#vid_1532&pid_0241&mi_01&col01#7&31628aa2&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}\\kbd";
    //// Step 1: Open a handle to the USB device

    //HANDLE device_handle = CreateFileW(devicePath, 0, 3, (LPSECURITY_ATTRIBUTES)0x0, 3, 0x40000000, (HANDLE)0x0);
    ////HANDLE hDevice = CreateFile(devicePath, 0, 3, NULL, OPEN_EXISTING, 0x40000000, NULL);
    //if (device_handle == INVALID_HANDLE_VALUE) {
    //    cerr << "Error opening device handle. Error code: " << GetLastError() << endl;
    //    return 1;}

    //// Step 2: Issue IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION
    //USB_NODE_CONNECTION_INFORMATION_EX_V2 connectionInfo = {};
    //DWORD bytesReturned = 0;
    //BOOL success = DeviceIoControl(device_handle, IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION,
    //    &connectionInfo, sizeof(connectionInfo),
    //    &connectionInfo, sizeof(connectionInfo),
    //    &bytesReturned, NULL);

    //if (!success) {
    //    cerr << "Error retrieving descriptor. Error code: " << GetLastError() << endl;
    //    CloseHandle(device_handle);
    //    return 1;}
    //// Step 3: Access the device descriptor
    ///*USB_DEVICE_DESCRIPTOR deviceDescriptor = connectionInfo.DeviceDescriptor;
    //cout << "USB Device Descriptor:" << endl;
    //cout << "Vendor ID: 0x" << hex << deviceDescriptor.idVendor << endl;
    //cout << "Product ID: 0x" << hex << deviceDescriptor.idProduct << endl;*/
    //// Add other relevant fields as needed
    //CloseHandle(device_handle);
    // -------------------------------------------------------------------------------------


    //contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_0241&mi_00#7&89cfb0b&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}\\kbd")); // keyboard
    //contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_0241&mi_02#7&1e326e64&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}")); // keyboard
    //contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_0241&mi_01&col01#7&31628aa2&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}\\kbd")); // keyboard
    //contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_0241&mi_01&col02#7&31628aa2&0&0001#{4d1e55b2-f16f-11cf-88cb-001111000030}")); // keyboard
    //contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_0241&mi_01&col03#7&31628aa2&0&0002#{4d1e55b2-f16f-11cf-88cb-001111000030}")); // keyboard
    //contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_0241&mi_01&col04#7&31628aa2&0&0003#{4d1e55b2-f16f-11cf-88cb-001111000030}")); // keyboard
    //contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_0241&mi_01&col05#7&31628aa2&0&0004#{4d1e55b2-f16f-11cf-88cb-001111000030}")); // keyboard
    /*contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_022b&mi_00#7&3a630f46&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}\\kbd"));
    contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_022b&mi_01&col02#7&2a901eea&0&0001#{4d1e55b2-f16f-11cf-88cb-001111000030}"));
    contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_022b&mi_01&col03#7&2a901eea&0&0002#{4d1e55b2-f16f-11cf-88cb-001111000030}"));
    contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_022b&mi_01&col04#7&2a901eea&0&0003#{4d1e55b2-f16f-11cf-88cb-001111000030}"));
    contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_022b&mi_01&col05#7&2a901eea&0&0004#{4d1e55b2-f16f-11cf-88cb-001111000030}"));
    contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_022b&mi_02#7&6b8e128&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}"));
    contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_022b&mi_00&col02#8&dca0e7a&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}"));
    contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_022b&mi_01&col01#7&2a901eea&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}\\kbd"));
    contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_022b&mi_00&col03&col01#8&3120b189&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}"));
    contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_022b&mi_00&col03&col02#8&3120b189&0&0001#{4d1e55b2-f16f-11cf-88cb-001111000030}"));
    contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_0241&mi_00&col03#8&5ac5cba&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}"));*/
    // ------------------------------------------------------------------------------------


}

int main(){
    printf("hello world\n");
    try_load_devices();
    cout << "\nending process...\n";
}


