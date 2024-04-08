#pragma once
#include "Devices/BlackWidow.h"
#include "Devices/TartarusV2.h"
#include "Devices/Goliathus.h"
#include "./Effects/EffectsTemplate.h"

namespace DeviceManagement {
	using namespace razer_blackwidow;
	using namespace razer_tartarusv2;
	using namespace razer_goliathus;
	enum deviceID:int {
		BlackWidow = 0x241,
		TartarusV2 = 0x22b,
		GoliathusE = 0xc02,
	};

	static razer_device* instaniate_device(int pid){
		switch (pid) {
		case BlackWidow: return new razer_blackwidow::device_blackwidow();
		case TartarusV2: return new razer_tartarusv2::device_tartarusv2();
        case GoliathusE: return new razer_goliathus::device_goliathus();
		default: return (razer_device*)-1;
		}
	}
	
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
                // perform checksum
                device_data->data_buffers[row].CalcChecksum();
                if (RazerIO::SendDataToDevice(device_details.driver_handle, 0x88883140, 0, 0, 0, 0, &bytes_returned, 0)); // no idea what this one actually does, but razer calls it super frequently
                    RazerIO::SendDataToDevice(device_details.driver_handle, 0x88883010, &device_data->data_buffers[row], sizeof(RazerDevice::razer_rgb_data), 0, 0, &bytes_returned, 0);
                Sleep(10); // needs a break inbetween packets? the devices seem to run very slowly when rows are sent one after another like this
            }
        }
        void DisableLighting() { // TODO, make await for if send data to device fails
            DWORD bytes_returned = 0;
            // keep an explicit list of things that this byte sequence is confirmed to be compatible with
            switch (device_details.pid) {
            //case TartarusV2: // causes potentially adverse effects; my tartarusV2 seems to automatically turn of lighting when interfaced with razer synapse now??
            case BlackWidow:
            case GoliathusE:
                char lights_off_packet[] = { 0x00,0x00,0x1F,0x00,0x00,0x00,0x06,0x0F,0x02,0x01,0x00,0x03,0x00,0x28,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x21,0x00 };
                RazerIO::SendDataToDevice(device_details.driver_handle, 0x88883140, 0, 0, 0, 0, &bytes_returned, 0);
                RazerIO::SendDataToDevice(device_details.driver_handle, 0x88883010, lights_off_packet, sizeof(lights_off_packet), 0, 0, &bytes_returned, 0);
                break;
            }
        }
        ~LoadedDevice() { // turning off lighting is optional as this can be called after the device stops responding
            delete device_data;
            for (int i = 0; i < effects.size(); i++) delete effects[i];
            CloseHandle(device_details.driver_handle);
            CloseHandle(device_details.hid_alivecheck_handle);
            wcout << L"\nUnloaded device: " << device_details.name;
        }
    };

    class DeviceManager {
    private:
        void UnregisterDevice(int index) {
            delete devices[index];
            devices.erase(devices.begin() + index);
        }
        void UnloadDevice(int index) {
            auto curr_device = devices[index];
            // test if the device is still active, if so disable lighting
            if (RazerIO::HandleAliveCheck(curr_device->device_details.hid_alivecheck_handle))
                curr_device->DisableLighting();
            UnregisterDevice(index);
        }
    public:
        vector<LoadedDevice*> devices = {};
        void CloseAllDevices() {
            for (int i = 0; i < devices.size(); i++){
                UnloadDevice(i);
                i--;
            }
        }
        vector<LoadedDevice*> RefreshDevices() {
            cout << "\nScanning for devices...";
            // recheck all devices to see if they're still communicating?? close them if not
            for (int i = 0; i < devices.size(); i++) {
                if (!RazerIO::HandleAliveCheck(devices[i]->device_details.hid_alivecheck_handle)) {
                    UnregisterDevice(i);
                    i--;
                }
            }
            // get a list of currently loaded pid's
            vector<int> active_pids = {};
            for (int i = 0; i < devices.size(); i++)
                active_pids.push_back(devices[i]->device_details.pid);
            // then get out device list and append any valid entries
            vector<LoadedDevice*> output = {};
            auto found_devices = RazerIO::try_load_devices(active_pids);
            for (int i = 0; i < found_devices.size(); i++) {
                auto dev_data_object = instaniate_device(found_devices[i].pid);
                if (dev_data_object == (RazerDevice::razer_device*)-1) {
                    wcout << L"\nUnsupported device, disregarding: " << found_devices[i].name;
                    continue;
                }
                auto mapped_device = new LoadedDevice{ found_devices[i], dev_data_object };
                devices.push_back(mapped_device);
                output.push_back(mapped_device);
            }
            return output;
        }
        LoadedDevice* GetDeviceByPid(int pid) {
            for (int i = 0; i < devices.size(); i++)
                if (devices[i]->device_details.pid == pid)
                    return devices[i];
        }
    };
}


