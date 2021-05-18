
#include "view.hpp"
#include "variable.hpp"



void move_camera()
{
	// Handle camera fly-through
	float dt = timer.update();
	// scene.camera.position_camera += user.speed*0.1f*dt*scene.camera.front();
	// if(user.keyboard_state.up)
	// 	scene.camera.manipulator_rotate_roll_pitch_yaw(0,-0.5f*dt,0);
	// if(user.keyboard_state.right)
	// 	scene.camera.manipulator_rotate_roll_pitch_yaw(0.7f*dt,0,0);
	// if(user.keyboard_state.left)
	// 	scene.camera.manipulator_rotate_roll_pitch_yaw(-0.7f*dt,0,0);
}

// Store keyboard state
// left-right / up-down key
void keyboard_callback(GLFWwindow* , int key, int , int action, int )
{
	if(key == GLFW_KEY_UP){
		if(action == GLFW_PRESS) user.keyboard_state.up = true;
		if(action == GLFW_RELEASE) user.keyboard_state.up = false;
	}

	if(key == GLFW_KEY_LEFT){
		if(action == GLFW_PRESS) user.keyboard_state.left = true;
		if(action == GLFW_RELEASE) user.keyboard_state.left = false;
	}

	if(key == GLFW_KEY_RIGHT){
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