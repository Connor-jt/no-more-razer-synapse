#pragma once
#include "../EffectsTemplate.h"
#include <corecrt_math_defines.h>

namespace RazerEffects {
    class WaveEffect :public RazerEffect {
    public:
        WaveEffect(RazerDevice::razer_device* target) {
            target_device = target;
        }

        float effect_x = 0.38f;
        float effect_y = 0.12f;
        float angle = -45.0f;

        float DegToRad(float degrees) {
            return degrees * (M_PI / 180);
        }

        void run() override {
            angle += 4.9f;
            for (int row = 0; row < target_device->row_count; row++) {
                for (int col = 0; col < target_device->keys[row].size; col++) {
                    if (!IsAcceptedKey((RazerDevice::KeyID)target_device->keys[row].ptr[col]))
                        continue;
                    auto pos = target_device->GetKeyOffset(row, col);

                    // logic courtsey of my cutie lavrent
                    float A, B, C;
                    // if phi == 90 or phi == 270 then A=1 B=0 C=-Ex
                    if (angle == 90 || angle == 270) {
                        A = 1;
                        B = 0;
                        C = -effect_x;
                    }else {
                        A = tan(DegToRad(angle));
                        B = -1;
                        C = (-tan(DegToRad(angle)) * effect_x) + effect_y;
                    }

                    // set up interpolation stuff

                    float intersect_x = ( ((B* pos.x)/A) - (C/B) - pos.y) / ( (A/B)+(B/A) );
                    float intersect_y = -(1/B) * (A * intersect_x + C);

                    const float max_dist = 0.12f;
                    float dist = sqrt(pow(effect_x-intersect_x, 2.0f) + pow(effect_y-intersect_y, 2.0f));

                    float vis_percentage = max_dist - dist;
                    if (vis_percentage < 0.0f) vis_percentage = 0.0f;

                    RGB_float result = {1.0f* vis_percentage, 1.0f* vis_percentage, 0.0f};

                    target_device->SetKey(row, col, result, false);
                   
                }
            }
        }
    };
}