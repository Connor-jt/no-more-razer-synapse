#pragma once

#include <windows.h>
#include <initguid.h>
#include <usbioctl.h>
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

namespace RazerIO {

    bool SendDataToDevice(HANDLE device, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, DWORD* bytes_returned, LPOVERLAPPED lpOverlapped) {

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
                result.push_back(source.substr(section_chars_begin_index, index - section_chars_begin_index));
                section_chars_begin_index = index + 1;
                section_index++;
            }
        }
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
        wstring path_descriptor = L"";
        wstring device_ids = L"";
        wstring hardware_ids = L"";
        // iterate through device path string and map each section
        {   int descriptor_index = 0;
        int last_descriptor_index = 0;
        for (int index = 0;; index++) {
            wchar_t current_char = device_path[index];
            if (current_char == '#') {
                switch (descriptor_index) {
                case 0: path_descriptor = wstring(device_path + last_descriptor_index, index - last_descriptor_index); break;
                case 1: device_ids = wstring(device_path + last_descriptor_index, index - last_descriptor_index); break;
                case 2: hardware_ids = wstring(device_path + last_descriptor_index, index - last_descriptor_index); break;
                case 3: result.class_guid = wstring(device_path + last_descriptor_index, index - last_descriptor_index); break;
                }
                last_descriptor_index = index + 1;
                descriptor_index++;
            }
            else if (current_char == '\0') break;
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
            case 3: result.hwid4 = stoi(physical_ids[i], nullptr, 16); break;
            }
        }}
        return result;
    }


    DEFINE_GUID(GUID_DEVINTERFACE_RAZER, 0xE3BE005DL, 0xD130, 0x4910, 0x88, 0xff, 0x09, 0xae, 0x02, 0xf6, 0x80, 0xe9);
    DEFINE_DEVPROPKEY(DEVPKEY_Device_ContainerId, 0x8c7ed206, 0x3f8a, 0x4827, 0xb3, 0xab, 0xae, 0x9e, 0x1f, 0xae, 0xfc, 0x6c, 2);     // DEVPROP_TYPE_GUID
    DEFINE_DEVPROPKEY(DEVPKEY_Device_BusReportedDeviceDesc, 0x540b947e, 0x8b40, 0x45bc, 0xa8, 0xa2, 0x6a, 0x0b, 0x89, 0x4c, 0xbd, 0xa2, 4);     // DEVPROP_TYPE_STRING
    wstring HID_get_name_from_parent(int target_vid, int target_pid) {
        HDEVINFO hDevInfo = SetupDiGetClassDevsW(&GUID_DEVINTERFACE_USB_DEVICE, 0, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
        if (hDevInfo == INVALID_HANDLE_VALUE) return wstring(L"ERROR: ENUMERATION FAILED");
        for (int device_index = 0;; device_index++) {
            SP_DEVICE_INTERFACE_DATA interfaceData = {};
            interfaceData.cbSize = sizeof(SP_DEVICE_INTERFACE_DATA);
            if (!SetupDiEnumDeviceInterfaces(hDevInfo, 0, &GUID_DEVINTERFACE_USB_DEVICE, device_index, &interfaceData))
                break;
            SP_DEVINFO_DATA devinfo_data = {};
            devinfo_data.cbSize = sizeof(SP_DEVINFO_DATA);
            if (!SetupDiEnumDeviceInfo(hDevInfo, device_index, &devinfo_data))
                continue;
            char __interface_detail_data[0x20e] = { 0 };
            PSP_DEVICE_INTERFACE_DETAIL_DATA_W interface_detail_data = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)&__interface_detail_data;
            interface_detail_data->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
            if (!SetupDiGetDeviceInterfaceDetailW(hDevInfo, &interfaceData, interface_detail_data, 0x20e, 0, 0))
                continue;
            // we're looking for a deivce that has a matching vid & pid
            auto interface_path_data = digest_device_path(interface_detail_data->DevicePath);
            if (interface_path_data.vid != target_vid && interface_path_data.vid != target_pid)
                continue;
            BYTE property_buffer[0x200] = { 0 };
            DEVPROPTYPE property_type = 0;
            DWORD required_size = 0;
            if (!SetupDiGetDevicePropertyW(hDevInfo, &devinfo_data, &DEVPKEY_Device_BusReportedDeviceDesc, &property_type, property_buffer, 0x200, &required_size, 0))
                return wstring(L"ERROR: PROPERTY GET FAILED");
            return wstring((LPWSTR)property_buffer);
        }
        return wstring(L"ERROR: NO MATCHING DEVICE");
    }
    HANDLE HID_find_device_driver(int target_vid, int target_pid) {
        HDEVINFO hDevInfo = SetupDiGetClassDevsW(&GUID_DEVINTERFACE_RAZER, 0, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
        if (hDevInfo == INVALID_HANDLE_VALUE)
            return (HANDLE)-1;
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


    struct discovered_device {
        int pid;
        wstring name;
        HANDLE driver_handle;
    };
    vector<discovered_device> try_load_devices() {
        vector<discovered_device> results = {};
        HDEVINFO hDevInfo = SetupDiGetClassDevsW(&GUID_DEVINTERFACE_HID, 0, 0, DIGCF_DEVICEINTERFACE | DIGCF_PRESENT);
        if (hDevInfo == INVALID_HANDLE_VALUE)
            return results;
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
            char __interface_detail_data[0x20e] = { 0 }; // number provided by razer via razer synapse 3 decompiled code
            PSP_DEVICE_INTERFACE_DETAIL_DATA_W interface_detail_data = (PSP_DEVICE_INTERFACE_DETAIL_DATA_W)&__interface_detail_data;
            interface_detail_data->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA); // razer says the answer is 6, although that is unlikely
            if (!SetupDiGetDeviceInterfaceDetailW(hDevInfo, &interfaceData, interface_detail_data, 0x20e, 0, 0))
                continue;
            // check for razer vendor id, skip any that aren't razer
            auto interface_path_data = digest_device_path(interface_detail_data->DevicePath);
            if (interface_path_data.vid != 0x1532 && interface_path_data.vid != 0x6e8) continue;

            wstring device_name = HID_get_name_from_parent(interface_path_data.vid, interface_path_data.pid);
            HANDLE driver_handle = HID_find_device_driver(interface_path_data.vid, interface_path_data.vid);
            if (driver_handle == (HANDLE)-1) {
                wcout << L"\nFound device without driver: " << device_name;
                continue;}

            wcout << L"\nFound device with driver: " << device_name;
            results.push_back(discovered_device{ interface_path_data.pid, device_name, driver_handle });
        }
        return results;
    }
}