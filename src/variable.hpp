#pragma once

#include <iostream>
#include <list>
#include "vcl/vcl.hpp"

using namespace vcl;

struct gui_parameters {
	bool display_frame = true;
    bool display_surface = true;
	bool display_wireframe = false;
	bool add_sphere = true;
};

struct keyboard_state_parameters{
	bool left  = false; 
	bool right = false; 
	bool up    = false;
	bool down  = false;
	bool ctrl  = false;
};


struct perlin_noise_parameters
{
	float persistency = 0.28f;
	float frequency_gain = 2.5f;
	int octave = 8;
	float terrain_height = 0.104f;
};

struct user_interaction_parameters {
	vec2 mouse_prev;
	timer_fps fps_record;
	mesh_drawable global_frame;
	gui_parameters gui;
	bool cursor_on_gui;
    bool display_frame = true;
	keyboard_state_parameters keyboard_state;
	float speed = 0.001f;
};

struct shadow_map_parameters 
{
	GLuint texture;
	int width;
	int height;
	GLuint fbo;
	GLuint shader;
};

struct scene_environment
{
	camera_around_center camera;
	mat4 projection;
	camera_spherical_coordinates light;
	shadow_map_parameters depth_map;
};

enum Collision {
    ILE = 1,
    TREE_ROCK,
    OCEAN,
    SHIP,
    CLOUD,
    BORDER,
	RING,
    CEILING
};

extern mesh terrain;
extern mesh_drawable terrain_visual;
extern mesh_drawable tree;
extern mesh_drawable feuille;
extern std::vector<vcl::vec3> tree_position;
extern perlin_noise_parameters parameters;

extern timer_basic timer;

extern user_interaction_parameters user;
extern scene_environment scene;

extern mesh ocean_m;

extern mesh_drawable terrain_visual;
extern mesh_drawable ocean;
extern mesh_drawable tree;
extern mesh_drawable feuille;
extern mesh_drawable wall;
extern mesh_drawable caillou;
extern mesh_drawable cloud1;
extern mesh_drawable cloud2;
extern mesh_drawable cloud3;
extern mesh_drawable cloud4;
extern mesh_drawable ship;
extern mesh_drawable ring;

extern hierarchy_mesh_drawable hierarchy_bird;
extern vec3 orientation_bird;
extern vec3 pos_without_oscill;

extern std::vector<vcl::vec3> tree_position;
extern perlin_noise_parameters parameters;
extern mesh_drawable billboard_grass;
extern mesh_drawable billboard_bird;
extern std::vector<mesh_drawable> liste_iles;
extern std::vector<vcl::vec3> ile_position;
extern std::vector<std::vector<vcl::vec3>> liste_tree_position;
extern std::vector<float> ile_orientation;
extern std::vector<vcl::vec3> cloud_position;
extern std::vector<float> cloud_orientation;
extern std::vector<vcl::vec3> ship_position;
extern std::vector<float> ship_orientation;

extern std::vector<vcl::vec3> ring_position;
extern std::vector<float> ring_orientation;

extern std::vector<perlin_noise_parameters> liste_noise_ile;

extern int taille_terrain;
extern int nb_iles;
extern int nb_arbres;
extern int nb_cloud;
extern int nb_ship;
extern int nb_ring;
extern float v_maree;
extern float fact_rot_cam;
extern float rot_facteur_bird;
extern vec3 rho_theta_phi;
extern float initial_speed;
extern float ceiling_height;
extern int width;
extern int height;

int compare_vec(vec3 vec_1, vec3 vec_2);