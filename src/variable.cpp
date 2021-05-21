#include "variable.hpp"

timer_basic timer;

user_interaction_parameters user;
scene_environment scene;

mesh terrain;
mesh ocean_m;

mesh_drawable terrain_visual;
mesh_drawable ocean;
mesh_drawable tree;
mesh_drawable feuille;
mesh_drawable wall;
mesh_drawable caillou;
mesh_drawable cloud1;
mesh_drawable cloud2;
mesh_drawable cloud3;
mesh_drawable cloud4;
mesh_drawable ship;
mesh_drawable ring;


std::vector<vcl::vec3> tree_position;
perlin_noise_parameters parameters;
mesh_drawable billboard_grass;
mesh_drawable billboard_bird;
std::vector<mesh_drawable> liste_iles;
std::vector<vcl::vec3> ile_position;
std::vector<std::vector<vcl::vec3>> liste_tree_position;
std::vector<float> ile_orientation;
std::vector<vcl::vec3> cloud_position;
std::vector<float> cloud_orientation;
std::vector<vcl::vec3> ship_position;
std::vector<float> ship_orientation;

std::vector<vcl::vec3> ring_position;
std::vector<float> ring_orientation;

std::vector<perlin_noise_parameters> liste_noise_ile;

int taille_terrain = 100;
int nb_iles = 6;
int nb_arbres = 20;
int nb_cloud = 40;
int nb_ship = 20;
int nb_ring = 20; 