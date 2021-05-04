#pragma once

#include "vcl/vcl.hpp"

struct perlin_noise_parameters
{
	float persistency = 0.35f;
	float frequency_gain = 200.00f;
	int octave = 6;
	float terrain_height = 0.4f;
};


vcl::vec3 evaluate_terrain(float u, float v, perlin_noise_parameters const& parameters);
vcl::mesh create_terrain(perlin_noise_parameters const& parameters);
std::vector<vcl::vec3> generate_positions_on_terrain(int N, perlin_noise_parameters const& parameters);

