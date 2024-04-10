#pragma once
#include "../EffectsTemplate.h"

namespace RazerEffects {
    class StaticEffect :public RazerEffect {
    public:
        StaticEffect(RazerDevice::razer_device* target, RGB_float target_color) {
            target_device = target;
            color = target_color;
        }
        RGB_float color;
        void run() override {
            for (int row = 0; row < target_device->row_count; row++) {
                for (int col = 0; col < target_device->keys[row].size; col++) {
                    if (IsAcceptedKey((RazerDevice::KeyID)target_device->keys[row].ptr[col])) {
                        target_device->SetKey(row, col, color, false);
                    }
                }
            }
        }
    };
}