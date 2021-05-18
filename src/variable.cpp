#include "variable.hpp"

mesh terrain;
mesh_drawable terrain_visual;
mesh_drawable tree;
mesh_drawable feuille;
std::vector<vcl::vec3> tree_position;
perlin_noise_parameters parameters;

timer_basic timer;

user_interaction_parameters user;
scene_environment scene;