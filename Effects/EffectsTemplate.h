#pragma once
#include "../Devices/DeviceTemplate.h"

namespace RazerEffects {
	class RazerEffect {
    protected:
		RazerDevice::razer_device* target_device = nullptr;
        RazerEffect() {};
    public:
		virtual void run() { throw new std::exception("unimplemented!!"); }
	};
	class SpectrumEffect:public RazerEffect {
    public:
	};
	class WaveEffect:public RazerEffect {
    public:
	};
    class DebugWaveEffect:public RazerEffect {
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
	class BounceEffect:public RazerEffect {
    public:
        BounceEffect(RazerDevice::razer_device* target) { target_device = target; }
        // temp moving color thing
        float curr_x = 0.0f;
        float curr_y = 0.14f;
        enum pong_direction {
            top_left,
            top_right,
            bot_left,
            bot_right
        }; pong_direction direction = top_right;
        void run() override {
            // collision with left side
            if (curr_x <= 0.0f) { 
                if (direction == top_left)
                    direction = top_right;
                else if (direction == bot_left)
                    direction = bot_right;
            // collision with right side
            }else if (curr_x >= target_device->width) { 
                if (direction == top_right)
                    direction = top_left;
                else if (direction == bot_right)
                    direction = bot_left;
            // collision with bottom
            }else if (curr_y <= 0.0f) { 
                if (direction == bot_left)
                    direction = top_left;
                else if (direction == bot_right)
                    direction = top_right;
            // collision with top
            }else if (curr_y >= target_device->width*target_device->height_ratio) { 
                if (direction == top_left)
                    direction = bot_left;
                else if (direction == top_right)
                    direction = bot_right;
            }
            const float speed = 0.02f;
            switch (direction) {
            case top_left:
                curr_x -= speed;
                curr_y += speed;
                break;
            case top_right:
                curr_x += speed;
                curr_y += speed;
                break;
            case bot_left:
                curr_x -= speed;
                curr_y -= speed;
                break;
            case bot_right:
                curr_x += speed;
                curr_y -= speed;
                break;
            }

            target_device->PaintAt(curr_x, curr_y, RazerDevice::RGB_float{ 1.0f, 1.0f, 1.0f }, true);
        }
	};
	class SpellingEffect:public RazerEffect {
    public:
        SpellingEffect(RazerDevice::razer_device* target) {target_device = target;}
        // temp word effect thing
        int character_index = 0;
        int max_character_ticks = 15;
        int character_ticks = max_character_ticks; 
        void run() override {
            // TODO: make static or something 
            string sample_text = "Hello world! amongus imposter 100 burger king foot letuce 15 dollar fortnite card";
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