
#include "view.hpp"
#include "variable.hpp"
#include "terrain.hpp"
#include <algorithm>

double approx = 0.0001;

void init_camera(){
	scene.camera.distance_to_center = 2.5f;
	vec3 pos_cam = {3, 3, rho_theta_phi[0]};
	scene.camera.look_at(pos_cam, {0,0,0}, {0,0,1});
}

void move_camera_center()
{
	scene.camera.center_of_rotation = get_pos_bird();
}

void change_angle_to_non_zero(double &val){
	if (std::abs(val) < approx){
		if(val < 0)
			val += - approx;
		else 
			val += approx;
	}
}

int on_ile_cam(float x, float y) {
	for (int k = 0; k < ile_position.size(); k++) {
		if (vcl::abs(x - ile_position[k][0]) < 10 && vcl::abs(y - ile_position[k][1]) < 10) {
			return k;
		}
	}
	return -1;
}

float height_ile(float x, float y) {
	int k = on_ile_cam(x,y);
	if(k < 0) return -1;
	float u = ((float)x - ile_position[k][0]) / 20 + 0.5f;
	float v = ((float)y - ile_position[k][1]) / 20 + 0.5f;
	//std::cout << u << ";" << v << endl;
	vec3 a = evaluate_terrain(u, v, liste_noise_ile[k]);
	return a[2];
}

float height_ocean(float x, float y){
	float z = ocean_height(x+taille_terrain/2, y + taille_terrain / 2, taille_terrain, parameters);
	return z;
}

void move_camera_rotation(vec2 p0, vec2 p1){

	vec3 pos_bird = get_pos_bird();
	vec2 diff_pos = fact_rot_cam * (p0 - p1);

	double rho = rho_theta_phi[0];
	double theta = rho_theta_phi[1] + diff_pos[1];
	double phi = rho_theta_phi[2] + diff_pos[0];

	change_angle_to_non_zero(theta);
	change_angle_to_non_zero(phi);	

	vec3 pos_centered = {rho * std::sin(theta) * std::cos(phi), rho * std::sin(theta) * std::sin(phi), rho * std::cos(theta)};

	rho_theta_phi = {rho, theta, phi};

	vec3 pos_cam = pos_centered + pos_bird;

	float x = pos_cam[0];

	x = std::min(std::max(x, - taille_terrain/2.0f), taille_terrain / 2.0f);

	float y = pos_cam[1];

	y = std::min(std::max(y, - taille_terrain/2.0f), taille_terrain / 2.0f);

	float z_terrain = std::max(height_ile(x,y), height_ocean(x,y));

	float z = std::max(pos_cam[2], z_terrain);

	pos_cam = {x,y,z};

	scene.camera.look_at(pos_cam, pos_bird, {0,0,rho});
}

// Store keyboard state
// left-right / up-down key
void keyboard_callback(GLFWwindow* , int key, int , int action, int )
{
	if(key == GLFW_KEY_UP || key == GLFW_KEY_Z || key == GLFW_KEY_W){
		if(action == GLFW_PRESS) user.keyboard_state.up = true;
		if(action == GLFW_RELEASE) user.keyboard_state.up = false;
	}

	if(key == GLFW_KEY_DOWN || key == GLFW_KEY_S){
		if(action == GLFW_PRESS) user.keyboard_state.down = true;
		if(action == GLFW_RELEASE) user.keyboard_state.down = false;
	}

	if(key == GLFW_KEY_LEFT || key == GLFW_KEY_A || key == GLFW_KEY_Q){
		if(action == GLFW_PRESS) user.keyboard_state.left = true;
		if(action == GLFW_RELEASE) user.keyboard_state.left = false;
	}

	if(key == GLFW_KEY_RIGHT || key == GLFW_KEY_D){
		if(action == GLFW_PRESS) user.keyboard_state.right = true;
		if(action == GLFW_RELEASE) user.keyboard_state.right = false;
	}
	if(key == GLFW_KEY_LEFT_CONTROL){
		if(action == GLFW_PRESS) user.keyboard_state.ctrl = true;
		if(action == GLFW_RELEASE) user.keyboard_state.ctrl = false;
	}
}


void initialize_data_view()
{
	GLuint const shader_mesh = opengl_create_shader_program(opengl_shader_preset("mesh_vertex"), opengl_shader_preset("mesh_fragment"));
	mesh_drawable::default_shader = shader_mesh;
	mesh_drawable::default_texture = opengl_texture_to_gpu(image_raw{1,1,image_color_type::rgba,{255,255,255,255}});

	user.global_frame = mesh_drawable(mesh_primitive_frame());
	// scene.camera.position_camera = {0.5f, 0.5f, -2.0f};
	// scene.camera.manipulator_rotate_roll_pitch_yaw(0,0,pi/2.0f);
}