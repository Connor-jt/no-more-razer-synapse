#pragma once

#include "../EffectsTemplate.h"

namespace RazerEffects {
	class CycleEffect :public RazerEffect {
    private:
        float cycle_progress = 0.0f;
        int cycle_index = 0;
	public:
        CycleEffect(RazerDevice::razer_device* target, vector<RGB_float> specified_colors = {}, float speed = 0.07f) {
            target_device = target;
            cycle_speed = speed;
            if (specified_colors.size() > 1){ // minimum 2 colors for effect to be valid
                colors.clear();
                for (int i = 0; i < specified_colors.size(); i++)
                    colors.push_back(specified_colors[i]);
            }
        }
        float cycle_speed = 0.07f; // max 1.0f
        vector<RGB_float> colors = { RGB_float{1.0f, 0.0f, 0.0f}, RGB_float{0.0f, 1.0f, 0.0f}, RGB_float{0.0f, 0.0f, 1.0f} };


        void run() override {
            int next_index = cycle_index + 1;
            if (next_index >= colors.size()) next_index = 0;

            cycle_progress += cycle_speed;
            if (cycle_progress >= 1.0f) {
                cycle_progress -= 1.0f;
                cycle_index = next_index;
                next_index++;
                if (next_index >= colors.size()) next_index = 0; // we can probably optimize to get rid of this
            }

            float curr_color_percent = 1.0f - cycle_progress;
            RGB_float result = {(colors[cycle_index].R*curr_color_percent) + (colors[next_index].R*cycle_progress),
                                (colors[cycle_index].G*curr_color_percent) + (colors[next_index].G*cycle_progress),
                                (colors[cycle_index].B*curr_color_percent) + (colors[next_index].B*cycle_progress),};

            for (int row = 0; row < target_device->row_count; row++) {
                for (int col = 0; col < target_device->keys[row].size; col++) {
                    if (IsAcceptedKey((RazerDevice::KeyID)target_device->keys[row].ptr[col]))
                        target_device->SetKey(row, col, result, false);
                }
            }
        }
	};
}