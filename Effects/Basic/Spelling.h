#pragma once
#include "../EffectsTemplate.h"

namespace RazerEffects {
    class SpellingEffect :public RazerEffect {
    public:
        SpellingEffect(RazerDevice::razer_device* target) { target_device = target; }
        // temp word effect thing
        int character_index = 0;
        int max_character_ticks = 5;
        int character_ticks = max_character_ticks;
        void run() override {
            // TODO: make static or something 
            string sample_text = "Hello world! test message 0123456789";
            float current_key_color = (float)character_ticks / (float)max_character_ticks;
            float next_key_color = 1.0f - current_key_color;

            auto keypos = target_device->GetKeyPosition((RazerDevice::KeyID)sample_text[character_index]);
            if (keypos.row != -1 && keypos.col != -1)
                target_device->SetKey(keypos.row, keypos.col, RazerDevice::RGB_float{ current_key_color,current_key_color,current_key_color }, false);

            int next_char_index = character_index + 1;
            if (next_char_index >= sample_text.size()) next_char_index = 0;

            keypos = target_device->GetKeyPosition((RazerDevice::KeyID)sample_text[next_char_index]);
            if (keypos.row != -1 && keypos.col != -1)
                target_device->SetKey(keypos.row, keypos.col, RazerDevice::RGB_float{ next_key_color,next_key_color,next_key_color }, false);

            character_ticks--;
            if (character_ticks == 0) {
                character_ticks = max_character_ticks;
                character_index = next_char_index;
            }

        }
    };
}