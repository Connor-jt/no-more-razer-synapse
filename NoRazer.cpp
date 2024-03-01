// NoRazer.cpp : This file contains the 'main' function. Program execution begins and ends there.
//
//typedef const USHORT USAGE;
//typedef const char* LPCSTR;
#include <windows.h>
#include <iostream>

#include <winusb.h>
#include <setupapi.h>




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
       /* if (HidD_GetPreparsedData(device_handle, &data)) {
            HidP_GetCaps(data, &caps);
            HidD_FreePreparsedData(data);
        }*/
    }
    return device_handle;
}

 
void SendDataToDevice(HANDLE device, DWORD dwIoControlCode, LPVOID lpInBuffer, DWORD nInBufferSize, LPVOID lpOutBuffer, DWORD nOutBufferSize, DWORD* bytes_returned, LPOVERLAPPED lpOverlapped){

    BOOL device_output;
    DWORD DVar2;
    DWORD lpBytesReturned;


    HANDLE thread_event = 0x0;
    if (lpOverlapped == (LPOVERLAPPED)0x0) {
        thread_event = CreateEventW((LPSECURITY_ATTRIBUTES)0x0, 1, 0, (LPCWSTR)0x0);
        if (thread_event == 0x0) return; // failed to create thread??
        
        ResetEvent(thread_event);
        // lpOverlapped = 0x0; // not sure how this is supposed to work? maybe the decompiler pulled some bad code
    }


    lpBytesReturned = 0;
    device_output = DeviceIoControl(device, dwIoControlCode, lpInBuffer, nInBufferSize, lpOutBuffer, nOutBufferSize, &lpBytesReturned, lpOverlapped);
    if (!device_output) {
        DVar2 = GetLastError();
        if (DVar2 == 0xea) // 'More data is available'
            goto LAB_1003a94b;
        
        if (DVar2 != 0x3e5) return; 
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
        if (bytes_returned != (DWORD*)0x0)
            *bytes_returned = lpBytesReturned;
    }
    return;
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

    contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_0241&mi_00#7&89cfb0b&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}\\kbd")); // keyboard
    contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_0241&mi_01&col01#7&31628aa2&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}\\kbd")); // keyboard
    contact_device(LoadDevice(L"\\\\?\\hid#vid_1532&pid_0241&mi_02#7&1e326e64&0&0000#{4d1e55b2-f16f-11cf-88cb-001111000030}")); // keyboard
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



}



