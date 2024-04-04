
static float clamp1(float d) {
	const float t = d < 0.0f ? 0.0f : d;
	return t > 1.0f ? 1.0f : t;
}