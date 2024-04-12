#pragma once
#include "../EffectsTemplate.h"

namespace RazerEffects {
    class BounceEffect :public RazerEffect {
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
            }
            else if (curr_x >= target_device->width) {
                if (direction == top_right)
                    direction = top_left;
                else if (direction == bot_right)
                    direction = bot_left;
                // collision with bottom
            }
            else if (curr_y <= 0.0f) {
                if (direction == bot_left)
                    direction = top_left;
                else if (direction == bot_right)
                    direction = top_right;
                // collision with top
            }
            else if (curr_y >= target_device->width * target_device->height_ratio) {
                if (direction == top_left)
                    direction = bot_left;
                else if (direction == top_right)
                    direction = bot_right;
            }
            const float speed = 0.01f;
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
}