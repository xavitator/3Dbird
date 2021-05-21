
#include "view.hpp"
#include "variable.hpp"


void init_camera(){
	scene.camera.distance_to_center = 2.5f;
	scene.camera.look_at({4,3,2}, {0,0,0}, {0,0,1});
}

void move_camera()
{
	scene.camera.center_of_rotation = get_pos_bird();
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