#pragma once

#include "vcl/vcl.hpp"
#include "variable.hpp"
#include "ring.hpp"
#include <random>
#include <iostream>
#include <iomanip>
#include <algorithm>

using std::cout;
using std::endl;
using std::setprecision;


vcl::vec3 evaluate_terrain(float u, float v, perlin_noise_parameters const& parameters);
vcl::mesh create_terrain(perlin_noise_parameters const& parameters);
vcl::mesh create_ocean(perlin_noise_parameters const& parameters);
std::vector<vcl::vec3> generate_positions_on_terrain(perlin_noise_parameters const& parameters);
std::vector<vcl::vec3> generate_positions_ile();
std::vector<vcl::vec3> generate_positions_clouds();
void generate_positions_ring();
std::vector<vcl::vec3> generate_positions_ships();
perlin_noise_parameters generate_alea_ile();
std::vector<float> generate_rotation(int N);
vcl::mesh create_wall();
void update_ocean(vcl::mesh& ocean, vcl::mesh_drawable& ocean_visual, perlin_noise_parameters const& parameterse);
float ocean_height(float a, float b, int N, perlin_noise_parameters const& parameters);

vcl::vec3 cloud_deplacement(vcl::vec3 position_initiale);

void generate_terrain();

mesh_drawable ile_g(perlin_noise_parameters par2);

mesh create_ocean_infini();

std::vector<vec3> generate_positions_ile_inf(int N);