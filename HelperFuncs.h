
#pragma once
#include <corecrt_math_defines.h>
#include <corecrt_math.h>

static float clamp1(float d) {
	const float t = d < 0.0f ? 0.0f : d;
	return t > 1.0f ? 1.0f : t;
}

float DegToRad(float degrees) {
    return degrees * (M_PI / 180);
}
float FloatAdditionClamp(float value, float change) {
    value = fmod(value + change, 360.0);
    if (value < 0.0f) value += 360.0;
    return value;
}