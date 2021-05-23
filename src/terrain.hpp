#pragma once

#include "vcl/vcl.hpp"
#include "variable.hpp"
#include <random>
#include <iostream>
#include <iomanip>
#include <algorithm>

using std::cout;
using std::endl;
using std::setprecision;


vcl::vec3 evaluate_terrain(float u, float v, perlin_noise_parameters const& parameters);
vcl::mesh create_terrain(perlin_noise_parameters const& parameters);
vcl::mesh create_ocean(perlin_noise_parameters const& parameters, int taille);
std::vector<vcl::vec3> generate_positions_on_terrain(int N, perlin_noise_parameters const& parameters);
std::vector<vcl::vec3> generate_positions_ile(int N, int taille);
std::vector<vcl::vec3> generate_positions_clouds(int N, int taille);
std::vector<vcl::vec3> generate_positions_ring(int N, int taille);
std::vector<vcl::vec3> generate_positions_ships(int N, int taille, std::vector<vcl::vec3> position_ile);
perlin_noise_parameters generate_alea_ile();
std::vector<float> generate_rotation(int N);
vcl::mesh create_wall(int taille);
void update_ocean(vcl::mesh& ocean, vcl::mesh_drawable& ocean_visual, perlin_noise_parameters const& parameters);
float ocean_height(float a, float b, int N, perlin_noise_parameters const& parameters, float v_maree);

vcl::vec3 cloud_deplacement(vcl::vec3 position_initiale, int taille);

void generate_terrain();

mesh_drawable ile_g(perlin_noise_parameters par2);