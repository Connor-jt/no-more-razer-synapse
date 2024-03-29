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
extern "C"
{
#include <hidsdi.h> // broken???
}


HANDLE LoadDevice(LPCWSTR device_identifier){
    HANDLE device_handle = CreateFileW(device_identifier, 0, 3, (LPSECURITY_ATTRIBUTES)0x0, 3, 0x40000000, (HANDLE)0x0);
   
    if (device_handle != (HANDLE)-1) {
        HIDP_CAPS caps = {};
        PHIDP_PREPARSED_DATA data = nullptr;
        // linkage error, thanks john microsoft
        //if (HidD_GetPreparsedData(device_handle, &data)) {
        //    HidP_GetCaps(data, &caps);
        //    HidD_FreePreparsedData(data);
        //}
    }
    return device_handle;
}

 
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
        
        
        // 'Overlapped I/O operation is in progress'

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
        std::cout << "\nsuccess";
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













int main()
{
    printf("hello world");

    // /////// // -------------------------------------------------------------
    // TEST 1 //
    // ///// //
    //SetupDiGetClassDevs();
    //SetupDiEnumDeviceInfo();
    //SetupDiGetDeviceRegistryProperty(); // SPDRP_LOCATION_INFORMATION 
    //CM_Get_Device_ID();
    // ----------------------------------------------------------------------

    // /////// // ----------------------------------------------------------------------
    // TEST 2 //
    // ///// //
    // Step 1: Get a device information set (e.g., for USB devices)
    //HDEVINFO hDevInfo = SetupDiGetClassDevs(&GUID_DEVINTERFACE_HID, NULL, NULL, DIGCF_PRESENT | DIGCF_DEVICEINTERFACE);
    //if (hDevInfo == INVALID_HANDLE_VALUE) {
    //    // Handle error
    //    return 1;}
    //// Step 2: Enumerate interfaces and get the desired interface data
    //SP_DEVICE_INTERFACE_DATA interfaceData = {};
    //// ... (enumerate interfaces and populate interfaceData)

    //// Step 3: Determine required buffer size
    //DWORD requiredSize = 0;
    //SetupDiGetDeviceInterfaceDetail(hDevInfo, &interfaceData, NULL, 0, &requiredSize, NULL);

    //// Step 4: Allocate memory and retrieve device interface detail
    //SP_DEVICE_INTERFACE_DETAIL_DATA* detailData = (SP_DEVICE_INTERFACE_DETAIL_DATA*)malloc(requiredSize);
    //detailData->cbSize = sizeof(SP_DEVICE_INTERFACE_DETAIL_DATA);
    //if (!SetupDiGetDeviceInterfaceDetail(hDevInfo, &interfaceData, detailData, requiredSize, NULL, NULL)) {
    //    // Handle error
    //    free(detailData);
    //    SetupDiDestroyDeviceInfoList(hDevInfo);
    //    return 1;
    //}

    //// Step 5: Access the device path (detailData->DevicePath)
    //// Clean up
    //free(detailData);
    //SetupDiDestroyDeviceInfoList(hDevInfo);

    // //////////////////////////// //
    // PART 2 OF THE 2ND TEST !!!! //
    // ////////////////////////// //
   
    // HID\VID_1532&PID_0241&MI_01&COL01\7&31628AA2&0&0000
    //const wchar_t* devicePath = L"\\\\?\\hid#vid_1532&pid_0241&mi_01&col01#7&31628aa2&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}\\kbd";
    //// Step 1: Open a handle to the USB device

    //HANDLE device_handle = CreateFileW(devicePath, 0, 3, (LPSECURITY_ATTRIBUTES)0x0, 3, 0x40000000, (HANDLE)0x0);
    ////HANDLE hDevice = CreateFile(devicePath, 0, 3, NULL, OPEN_EXISTING, 0x40000000, NULL);
    //if (device_handle == INVALID_HANDLE_VALUE) {
    //    std::cerr << "Error opening device handle. Error code: " << GetLastError() << std::endl;
    //    return 1;}

    //// Step 2: Issue IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION
    //USB_NODE_CONNECTION_INFORMATION_EX_V2 connectionInfo = {};
    //DWORD bytesReturned = 0;
    //BOOL success = DeviceIoControl(device_handle, IOCTL_USB_GET_DESCRIPTOR_FROM_NODE_CONNECTION,
    //    &connectionInfo, sizeof(connectionInfo),
    //    &connectionInfo, sizeof(connectionInfo),
    //    &bytesReturned, NULL);

    //if (!success) {
    //    std::cerr << "Error retrieving descriptor. Error code: " << GetLastError() << std::endl;
    //    CloseHandle(device_handle);
    //    return 1;}
    //// Step 3: Access the device descriptor
    ///*USB_DEVICE_DESCRIPTOR deviceDescriptor = connectionInfo.DeviceDescriptor;
    //std::cout << "USB Device Descriptor:" << std::endl;
    //std::cout << "Vendor ID: 0x" << std::hex << deviceDescriptor.idVendor << std::endl;
    //std::cout << "Product ID: 0x" << std::hex << deviceDescriptor.idProduct << std::endl;*/
    //// Add other relevant fields as needed
    //CloseHandle(device_handle);
    // -------------------------------------------------------------------------------------

    // /////// // -------------------------------------------------------------------------
    // TEST 3 // 
    // ///// //
    // 
    auto device_test = LoadDevice(L"\\\\?\\hid#vid_1532&pid_0241&mi_01&col01#7&31628aa2&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}\\kbd");
    // iterate all things
    DWORD bytes_returned = 0;
    int succeeded = 0;
    int start = 0x88883000;
    int max = 0x512;
    for (int i = start; i < max+start; i++) {
        if (SendDataToDevice(device_test, i, 0, 0, 0, 0, &bytes_returned, 0)) {
            succeeded++;
            //std::cout << "\n failure on: " << (start+i);
        }
    }
    // report failure count
    std::cout << "\n" << succeeded << "/" << max << " attempts succeeded";


    contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_0241&mi_00#7&89cfb0b&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}\\kbd")); // keyboard
    contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_0241&mi_02#7&1e326e64&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}")); // keyboard
    contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_0241&mi_01&col01#7&31628aa2&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}\\kbd")); // keyboard
    contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_0241&mi_01&col02#7&31628aa2&0&0001#{4d1e55b2-f16f-11cf-88cb-001111000030}")); // keyboard
    contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_0241&mi_01&col03#7&31628aa2&0&0002#{4d1e55b2-f16f-11cf-88cb-001111000030}")); // keyboard
    contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_0241&mi_01&col04#7&31628aa2&0&0003#{4d1e55b2-f16f-11cf-88cb-001111000030}")); // keyboard
    contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_0241&mi_01&col05#7&31628aa2&0&0004#{4d1e55b2-f16f-11cf-88cb-001111000030}")); // keyboard
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



