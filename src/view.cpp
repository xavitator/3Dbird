
#include "view.hpp"
#include "variable.hpp"

vec3 pos_cam = {4,3,2};

void init_camera(){
	scene.camera.distance_to_center = 2.5f;
	scene.camera.look_at(pos_cam, {0,0,0}, {0,0,1});
}

void move_camera_center()
{
	scene.camera.center_of_rotation = get_pos_bird();
}

void move_camera_rotation(vec2 p0, vec2 p1){
	float fact_rot = 0.005f;

	vec3 pos_bird = get_pos_bird();
	vec3 pos_cam_relat = pos_cam - pos_bird;

	double rho = scene.camera.distance_to_center;
	vec2 diff_pos = fact_rot * (p1 - p0);
	double theta = std::acos(pos_cam_relat[2] / rho) + diff_pos[1];
	double phi = std::atan(pos_cam_relat[1] / pos_cam_relat[0]) + diff_pos[0];

	vec3 pos_centered = {rho * std::sin(theta) * std::cos(phi), rho * std::sin(theta) * std::sin(phi), rho * std::cos(theta)};

	scene.camera.look_at(pos_centered + get_pos_bird(), get_pos_bird(), {0,0,1});
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