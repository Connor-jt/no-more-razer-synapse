#pragma once
#include "../EffectsTemplate.h"

namespace RazerEffects {
    class WaveEffect :public RazerEffect {
    public:
        WaveEffect(RazerDevice::razer_device* target) {
            target_device = target;
        }

        float effect_x = 0.38f;
        float effect_y = 0.12f;
        float angle = -45.0f;
        float thickness = 0.11;

        bool symmetrical = false;
        bool loop_gradients = true;
        float current_grad_offset = 0.0f;

        struct gradient {
            float offset;
            RGB_float color;
        };
        vector<gradient> gradients = { gradient{0.0f, 1.0f, 0.0f, 0.0f}, gradient{0.25f, 0.0f, 1.0f, 0.0f}, gradient{0.5f, 1.0f, 0.0f, 0.0f} };


        void run() override {
            // custom: rotate gradient
            angle = FloatAdditionClamp(angle, 4.9f);
            if (!gradients.size()) return; // dont do anything if we dont have any colors to set
            
            float furthest_offset = gradients[gradients.size() - 1].offset;
            // custom: shift gradient
            current_grad_offset += 0.06f;
            if (current_grad_offset > furthest_offset)
                current_grad_offset -= furthest_offset;
            else if (current_grad_offset < 0.0f)
                current_grad_offset += furthest_offset;

            for (int row = 0; row < target_device->row_count; row++) {
                for (int col = 0; col < target_device->keys[row].size; col++) {
                    if (!IsAcceptedKey((RazerDevice::KeyID)target_device->keys[row].ptr[col]))
                        continue;
                    auto pos = target_device->GetKeyOffset(row, col);

                    float target_direction = DegToRad(angle);
                    // logic courtesy of my cutie lavrent
                    float A, B, C;
                    if (angle == 90 || angle == 270) {
                        A = 1;
                        B = 0;
                        C = -effect_x;
                    }else {
                        A = tan(target_direction);
                        B = -1;
                        C = (-tan(target_direction) * effect_x) + effect_y;
                    }
                    float intersect_x = (((B * pos.x) / A) - (C / B) - pos.y) / ((A / B) + (B / A));
                    float intersect_y = -(1 / B) * (A * intersect_x + C);

                    // set up interpolation stuff
                    float gradient_offset = sqrt(pow(effect_x-intersect_x, 2.0f) + pow(effect_y-intersect_y, 2.0f));
                    if (!symmetrical) {
                        // get the smallest angle between the points and check if its less than 90 degrees, if it is, then its on the forward side
                        float direction = atan2(pos.y-effect_y, pos.x-effect_x) + M_PI;
                        float angle_between = abs(direction - target_direction);
                        float smallest_angle = (angle_between > M_PI) ? (M_PI*2) - angle_between : angle_between;
                        if (smallest_angle > M_PI/2)
                            gradient_offset *= -1;
                    }
                    gradient_offset += current_grad_offset;
                    if (loop_gradients) {
                        gradient_offset = fmod(gradient_offset, furthest_offset);
                        if (gradient_offset < 0.0f) gradient_offset += furthest_offset;
                    }

                    // find base gradient, and then next gradient
                    gradient base_gradient = gradients[0]; // this is so anything less than lowest offset will only be given this one gradient (do not interpolate)
                    gradient next_gradient = {0.0f, NAN, NAN, NAN};
                    for (int i = 0; i < gradients.size(); i++) {
                        // iterate through list until current entry is beyond our current offset
                        if (gradient_offset < gradients[i].offset) {
                            if (i > 0) next_gradient = gradients[i]; // if the first entry was too high of an offset, then do NOT write to next gradient
                            break;
                        } else base_gradient = gradients[i];
                    }
                    RGB_float result;
                    if (isnan(next_gradient.color.R)) {
                        // no interpolation needed
                        result = base_gradient.color;
                    }else { // interpolate gradients
                        float distance_between_gradients = next_gradient.offset - base_gradient.offset;
                        float our_distance_inbetween = gradient_offset - base_gradient.offset;
                        float next_grad_percent = our_distance_inbetween / distance_between_gradients;
                        float base_grad_percent = 1.0f - next_grad_percent;

                        result = {(base_gradient.color.R*base_grad_percent)+(next_gradient.color.R*next_grad_percent),
                                  (base_gradient.color.G*base_grad_percent)+(next_gradient.color.G*next_grad_percent),
                                  (base_gradient.color.B*base_grad_percent)+(next_gradient.color.B*next_grad_percent)};
                    }
                    // if the gradient has a line thickness, then darken pieces further away from the line
                    if (!isnan(thickness)) {
                        float distance_from_line = sqrt(pow(pos.x - intersect_x, 2.0f) + pow(pos.y - intersect_y, 2.0f));
                        float visibility = thickness - distance_from_line;
                        if (visibility < 0.0f) visibility = 0.0f;
                        result.R *= visibility;
                        result.G *= visibility;
                        result.B *= visibility;
                    }


                    target_device->SetKey(row, col, result, false);
                   
                }
            }
        }
    };
}