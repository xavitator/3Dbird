#pragma once

#include "vcl/vcl.hpp"
#include "variable.hpp"


vcl::vec3 evaluate_terrain(float u, float v, perlin_noise_parameters const& parameters);
vcl::mesh create_terrain(perlin_noise_parameters const& parameters);
std::vector<vcl::vec3> generate_positions_on_terrain(int N, perlin_noise_parameters const& parameters);

