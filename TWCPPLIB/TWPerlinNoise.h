#pragma once
#include "TW3DTypes.h"

// Perlin noise [-1; 1]
class TWPerlinNoise {
public:
	TWPerlinNoise(TWT::uint seed = std::default_random_engine::default_seed);

	template <class URNG>
	explicit TWPerlinNoise(URNG& urng) {
		reseed(urng);
	}

	void reseed(TWT::uint seed);

	template <class URNG>
	void reseed(URNG& urng) {
		for (size_t i = 0; i < 256; ++i) {
			p[i] = i;
		}

		std::shuffle(std::begin(p), std::begin(p) + 256, urng);

		for (size_t i = 0; i < 256; ++i) {
			p[256 + i] = p[i];
		}
	}

	double noise(TWT::vec3d xyz) const;
	double octaveNoise(TWT::vec3d xyz, int octaves) const;

private:
	static double Fade(double t) noexcept;
	static double Lerp(double t, double a, double b) noexcept;
	static double Grad(int hash, double x, double y, double z) noexcept;

	int p[512];
};