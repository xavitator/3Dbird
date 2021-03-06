#include "variable.hpp"

timer_basic timer;
timer_event_periodic period_1 = timer_event_periodic(1);
timer_event_periodic period_10 = timer_event_periodic(10);


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
mesh_drawable ocean_inf;

hierarchy_mesh_drawable hierarchy_bird;
vec3 orientation_bird = {0,1,0};
vec3 pos_without_oscill;

std::vector<vcl::vec3> tree_position;
perlin_noise_parameters parameters;
mesh_drawable billboard_grass;
mesh_drawable billboard_bird;
std::vector<mesh_drawable> liste_iles;
std::vector<vcl::vec3> ile_position;
std::vector<vcl::vec3> ile_position_inf;
std::vector<std::vector<vcl::vec3>> liste_tree_position;
std::vector<float> ile_orientation;
std::vector<vcl::vec3> cloud_position;
std::vector<float> cloud_orientation;
std::vector<vcl::vec3> ship_position;
std::vector<float> ship_orientation;

std::vector<Ring *> ring_objects;

std::vector<perlin_noise_parameters> liste_noise_ile;

int taille_terrain = 100;
int nb_iles = 5;
int nb_arbres = 20;
int nb_cloud = 40;
int nb_ship = 20;
float fact_rot_cam = 0.005f;
float rot_facteur_bird = 0.05f;
vec3 rho_theta_phi = {5.0, 1.0, 1.0};
float initial_speed = 1.05f;
int ceiling_height = 11;
int nb_ring = 100;
float v_maree = 0.1f;
int limite_essai = 50;
int width = 1280, height = 1024;
vec3 color_bird = {1.0f, 1.0f, 1.0f};

float omega= 0.0f;
float theta=0.0f;
float vitesse = 0.0f;