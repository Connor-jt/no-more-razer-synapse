#pragma once

namespace RazerEffects {
    class DebugWaveEffect :public RazerEffect {
    public:
        DebugWaveEffect(RazerDevice::razer_device* target) { target_device = target; }
        // temporary effect thing
        int curr_starting_cycle = 2;
        void run() override {
            // basic pattern to make colors increment and reset
            for (int row = 0; row < target_device->row_count; row++) {
                int cycle = curr_starting_cycle;
                for (int col = 0; col < target_device->keys[row].size; col++) {
                    if (target_device->keys[row].ptr[col] == RazerDevice::KeyID::NONE) {
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
                    new_val.R *= 0.1;
                    new_val.G *= 0.1;
                    new_val.B *= 0.1;
                    target_device->SetKey(row, col, new_val, false);
                }
            }
            // progress animation loop
            curr_starting_cycle++;
            if (curr_starting_cycle >= 6)
                curr_starting_cycle = 0;
        }
    };
}