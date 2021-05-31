
#include "view.hpp"
#include "variable.hpp"
#include "terrain.hpp"
#include "hitbox.hpp"
#include <algorithm>

double approx = 0.0001;

/**
 * @brief Initialisation de la caméra
 * 
 */
void init_camera(){
	scene.camera.distance_to_center = 2.5f;
	vec3 pos_cam = {3, 3, rho_theta_phi[0]};
	scene.camera.look_at(pos_cam, {0,0,0}, {0,0,1});
}

/**
 * @brief Bouger la caméra sur la position de l'oiseau
 * 
 */
void move_camera_center()
{
	scene.camera.center_of_rotation = get_pos_bird();
}

/**
 * @brief Rendre val proche de 0 sans être égal (éviter une normalisation de vecteur non possible à cause d'un élément valant 0)
 * 
 * @param val Valeur à vérifier
 */
void change_angle_to_non_zero(double &val){
	if (std::abs(val) < approx){
		if(val < 0)
			val += - approx;
		else 
			val += approx;
	}
}

/**
 * @brief Bouger la caméra sachant qu'il y a un mouvement de souris entre la position p0 et la position p1 sur le plan de la fenetre
 * 
 * @param p0 position d'origine
 * @param p1 position d'arrivée
 */
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

/**
 * @brief Resize de la fenetre
 * 
 * @param n_width nouvelle largeur
 * @param n_height nouvelle hauteur
 */
void window_size_callback(GLFWwindow* , int n_width, int n_height)
{
	width = n_width;
	height = n_height;
	glViewport(0, 0, width, height);
	float const aspect = width / static_cast<float>(height);
	scene.projection = projection_perspective(50.0f*pi/180.0f, aspect, 0.1f, 100.0f);
}

/**
 * @brief Fonction appelée lorsque l'utilisateur bouge la souris
 * 
 * @param window fenetre où est la souris
 * @param xpos xpos de la souris
 * @param ypos ypos de la souris
 */
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
	vec2 const  p1 = vec2(xpos, ypos);
	vec2 const& p0 = user.mouse_prev;
	glfw_state state = glfw_current_state(window);

	if(state.key_ctrl || user.dead){
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	}
	else
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if(! state.key_ctrl && ! user.dead){
		move_camera_rotation(p0, p1);
	}
	user.mouse_prev = p1;
}

/**
 * @brief Réaction en fonction des touches sur lesquelles appuie le joueur
 * 
 * @param key id de la touche enfoncée
 * @param action action sur la touche
 */
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