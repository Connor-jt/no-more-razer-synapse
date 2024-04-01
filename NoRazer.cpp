// NoRazer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//typedef const USHORT USAGE;
//typedef const char* LPCSTR;
#include <windows.h>
#include <usbioctl.h>
//#pragma comment (lib, "Hidclass.lib")
#include <initguid.h>
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

 
bool SendDataToDevice(HANDLE device, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, DWORD* bytes_returned, LPOVERLAPPED lpOverlapped){

    BOOL device_output;
    DWORD DVar2;
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
        DVar2 = GetLastError();
        if (DVar2 == 0xea) // 'More data is available'
            goto LAB_1003a94b;
        
        if (DVar2 != 0x3e5) {
            if (DVar2 == 1)
                return false;
            else return true;
        }
        
        
        // error is 'Overlapped I/O operation is in progress'

        // seems like this is all unimportant & could just be skipped !!!
        DVar2 = WaitForSingleObject(thread_event, 2000);
        if (DVar2 == 0) {
            BOOL overlap_result = GetOverlappedResult(device, lpOverlapped, &lpBytesReturned, 0);
            if (overlap_result != 0)
                goto LAB_1003a94b;
        }
        else {
            // exception L"CHIDDriver::DeviceControl : TimeOut\n";
            CancelIoEx(device, lpOverlapped);
            GetOverlappedResult(device, lpOverlapped, &lpBytesReturned, 1);
            // why dont we return 
        }
    }
    else {
    LAB_1003a94b:
        cout << "\nsuccess";
        if (bytes_returned != (DWORD*)0x0)
            *bytes_returned = lpBytesReturned;
    }
    return true;
}

void contact_device(HANDLE handle) {
    if (handle == 0 || handle == (HANDLE)-1)
        return;

    char data[] = { 0x00, 0x00, 0x1F, 0x00, 0x00, 0x00, 0x47, 0x0F, 0x03, 0x00, 0x00, 0x02, 0x00, 0x15, 0x00, 0x00, 0x00, 0x00, 0xE1, 0x00, 0x64, 0x00, 0x00, 0x0A, 0x00, 0x01, 0x00, 0xC9, 0x00, 0x00, 0xC0, 0x00, 0x00, 0xB8, 0x00, 0x00, 0xB0, 0x00, 0x00, 0xA7, 0x00, 0x00, 0x9F, 0x00, 0x00, 0x97, 0x00, 0x00, 0x8F, 0x00, 0x00, 0x86, 0x00, 0x00, 0x7E, 0x00, 0x00, 0x76, 0x00, 0x00, 0x65, 0x00, 0x00, 0x5D, 0x00, 0x00, 0x55, 0x00, 0x00, 0x4C, 0x00, 0x00, 0x44, 0x00, 0x00, 0x3C, 0x00, 0x00, 0x33, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0x00 };
    
    DWORD bytes_returned = 0;
    SendDataToDevice(handle, 0x88883140, 0, 0, 0, 0, &bytes_returned, 0);
    SendDataToDevice(handle, 0x88883010, &data, sizeof(data), 0, 0, &bytes_returned, 0);
    SendDataToDevice(handle, 0x88883140, 0, 0, 0, 0, &bytes_returned, 0);
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



int try_load_devices(){
    HDEVINFO hDevInfo = SetupDiGetClassDevsW(&GUID_DEVINTERFACE_HID, 0, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
    if (hDevInfo == INVALID_HANDLE_VALUE) 
        return GetLastError();

    // iterate vector indexes until index failure
    int device_index = 0;
    while (device_index++, true) {

        SP_DEVICE_INTERFACE_DATA interfaceData = {};
        interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
        if (!SetupDiEnumDeviceInterfaces(hDevInfo, 0, &GUID_DEVINTERFACE_HID, device_index, &interfaceData))
            break; // this means we've reached the end of the vector

        SP_DEVINFO_DATA devinfo_data = {};
        devinfo_data.cbSize = sizeof(SP_DEVINFO_DATA);
        if (!SetupDiEnumDeviceInfo(hDevInfo, device_index, &devinfo_data))
        { cout << "\nSDEDI: " << GetLastError(); continue; }

        char __interface_detail_data[0x20e] = {0}; // number provided by razer via razer synapse 3 decompiled code
        PSP_DEVICE_INTERFACE_DETAIL_DATA_W interface_detail_data = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)&__interface_detail_data;
        interface_detail_data->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA); // razer says the answer is 6, although that is unlikely
        if (!SetupDiGetDeviceInterfaceDetailW(hDevInfo, &interfaceData, interface_detail_data, 0x20e, 0, 0))
        { cout << "\nSDGDIDW: " << GetLastError(); continue; }

        // check for razer vendor id, skip any that aren't razer
        auto interface_path_data = digest_device_path(interface_detail_data->DevicePath);
        if (interface_path_data.vid != 0x1532 && interface_path_data.vid != 0x6e8) continue; 
        

        HANDLE device_handle = CreateFileW(interface_detail_data->DevicePath, 0, 3, (LPSECURITY_ATTRIBUTES)0x0, 3, 0x40000000, (HANDLE)0x0);
        if (device_handle == (HANDLE)-1)
        { cout << "\nCFW: " << GetLastError(); continue; }

        HIDP_CAPS caps = {};
        PHIDP_PREPARSED_DATA data = nullptr;
        if (HidD_GetPreparsedData(device_handle, &data)) {
            HidP_GetCaps(data, &caps);
            HidD_FreePreparsedData(data);
        }



        DWORD bytes_returned = 0;
        if (SendDataToDevice(device_handle, 0x88883140, 0, 0, 0, 0, &bytes_returned, 0))
        {

            char breakpoint_test = 0;
        }
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
    cout << "reponse code: " << try_load_devices() << "\n";
    cout << "ending process...\n";
}


