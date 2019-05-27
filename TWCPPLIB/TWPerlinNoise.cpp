#include "pch.h"
#include "TWPerlinNoise.h"

TWPerlinNoise::TWPerlinNoise(TWT::uint seed) {
	reseed(seed);
}

void TWPerlinNoise::reseed(TWT::uint seed) {
	for (size_t i = 0; i < 256; ++i) {
		p[i] = i;
	}

	std::shuffle(std::begin(p), std::begin(p) + 256, std::default_random_engine(seed));

	for (size_t i = 0; i < 256; ++i) {
		p[256 + i] = p[i];
	}
}

double TWPerlinNoise::noise(TWT::vec3d xyz) const {
	const int X = static_cast<int>(std::floor(xyz.x)) & 255;
	const int Y = static_cast<int>(std::floor(xyz.y)) & 255;
	const int Z = static_cast<int>(std::floor(xyz.z)) & 255;

	xyz -= glm::floor(xyz);

	const double u = Fade(xyz.x);
	const double v = Fade(xyz.y);
	const double w = Fade(xyz.z);

	const int A = p[X] + Y, AA = p[A] + Z, AB = p[A + 1] + Z;
	const int B = p[X + 1] + Y, BA = p[B] + Z, BB = p[B + 1] + Z;

	return Lerp(w, Lerp(v, Lerp(u, Grad(p[AA], xyz.x, xyz.y, xyz.z),
		Grad(p[BA], xyz.x - 1, xyz.y, xyz.z)),
		Lerp(u, Grad(p[AB], xyz.x, xyz.y - 1, xyz.z),
		Grad(p[BB], xyz.x - 1, xyz.y - 1, xyz.z))),
		Lerp(v, Lerp(u, Grad(p[AA + 1], xyz.x, xyz.y, xyz.z - 1),
		Grad(p[BA + 1], xyz.x - 1, xyz.y, xyz.z - 1)),
		Lerp(u, Grad(p[AB + 1], xyz.x, xyz.y - 1, xyz.z - 1),
		Grad(p[BB + 1], xyz.x - 1, xyz.y - 1, xyz.z - 1))));
}

double TWPerlinNoise::octaveNoise(TWT::vec3d xyz, int octaves) const {
	double result = 0.0;
	double amp = 1.0;

	for (int i = 0; i < octaves; ++i) {
		result += noise(xyz) * amp;
		xyz *= 2.0;
		amp *= 0.5;
	}

	return result;
}

double TWPerlinNoise::Fade(double t) noexcept {
	return t * t * t * (t * (t * 6 - 15) + 10);
}

double TWPerlinNoise::Lerp(double t, double a, double b) noexcept {
	return a + t * (b - a);
}

double TWPerlinNoise::Grad(int hash, double x, double y, double z) noexcept {
	const std::int32_t h = hash & 15;
	const double u = h < 8 ? x : y;
	const double v = h < 4 ? y : h == 12 || h == 14 ? x : z;
	return ((h & 1) == 0 ? u : -u) + ((h & 2) == 0 ? v : -v);
}