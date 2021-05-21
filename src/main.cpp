#include "vcl/vcl.hpp"
#include <iostream>

#include "view.hpp"
#include "terrain.hpp"
#include "tree.hpp"
#include "bird.hpp"
#include "variable.hpp"

using namespace vcl;

void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
void window_size_callback(GLFWwindow* window, int width, int height);
void opengl_uniform(GLuint shader, scene_environment const& current_scene);

void initialize_data();
void display_scene();
void display_interface();

int main(int, char* argv[])
{
	std::cout << "Run " << argv[0] << std::endl;

	int const width = 1280, height = 1024;
	GLFWwindow* window = create_window(width, height);
	window_size_callback(window, width, height);
	std::cout << opengl_info_display() << std::endl;;

	imgui_init(window);

	glfwSetCursorPosCallback(window, mouse_move_callback);
	glfwSetKeyCallback(window, keyboard_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported())
    	glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	
	std::cout<<"Initialize data ..."<<std::endl;
	initialize_data();


	std::cout<<"Start animation loop ..."<<std::endl;
	user.fps_record.start();
	glEnable(GL_DEPTH_TEST);
	while (!glfwWindowShouldClose(window))
	{
		scene.light = scene.camera.position();
		user.fps_record.update();
		
		glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);
		glClear(GL_DEPTH_BUFFER_BIT);
		imgui_create_frame();
		if(user.fps_record.event) {
			std::string const title = "VCL Display - "+str(user.fps_record.fps)+" fps";
			glfwSetWindowTitle(window, title.c_str());
		}

		ImGui::Begin("GUI",NULL,ImGuiWindowFlags_AlwaysAutoResize);
		//user.cursor_on_gui = ImGui::GetIO().WantCaptureMouse;

		if(user.gui.display_frame) draw(user.global_frame, scene);

		display_interface();
		display_scene();

		ImGui::End();
		imgui_render_frame(window);
		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	imgui_cleanup();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}



void initialize_data()
{
	GLuint const shader_mesh = opengl_create_shader_program(opengl_shader_preset("mesh_vertex"), opengl_shader_preset("mesh_fragment"));
	GLuint const shader_uniform_color = opengl_create_shader_program(opengl_shader_preset("single_color_vertex"), opengl_shader_preset("single_color_fragment"));
	GLuint const texture_white = opengl_texture_to_gpu(image_raw{1,1,image_color_type::rgba,{255,255,255,255}});
	mesh_drawable::default_shader = shader_mesh;
	mesh_drawable::default_texture = texture_white;
	curve_drawable::default_shader = shader_uniform_color;
	segments_drawable::default_shader = shader_uniform_color;
	
	user.global_frame = mesh_drawable(mesh_primitive_frame());
	user.gui.display_frame = false;
	init_camera();

    // Create visual terrain surface
	terrain = create_terrain(parameters);
	terrain_visual = mesh_drawable(terrain);
    terrain_visual.shading.color = {0.6f,0.85f,0.5f};
    terrain_visual.shading.phong.specular = 0.0f; // non-specular terrain material

	image_raw const im2 = image_load_png("assets/texture_grass.png");

	// Send this image to the GPU, and get its identifier texture_image_id
	GLuint const texture_image_id2 = opengl_texture_to_gpu(im2,
		GL_MIRRORED_REPEAT /**GL_TEXTURE_WRAP_S*/,
		GL_MIRRORED_REPEAT /**GL_TEXTURE_WRAP_T*/);

	terrain_visual.texture = texture_image_id2;
	


	feuille = mesh_drawable(create_cone(1.0f, 2.0f, 2.0f));
	feuille.shading.color = { 0.0f, 1.0f, 0.0f };

	tree = mesh_drawable(create_tree());
	

	tree_position = generate_positions_on_terrain(100, parameters);

	create_bird();
}


void display_scene()
{
	draw(terrain_visual, scene);
	for (int i = 0; i < 100; i++) {
		tree.transform.translate = tree_position[i];
		draw(tree, scene);
	}
	move_bird();
	move_camera();
}


void display_interface()
{
	ImGui::Checkbox("Frame", &user.gui.display_frame);

	
}

void window_size_callback(GLFWwindow* , int width, int height)
{
	glViewport(0, 0, width, height);
	float const aspect = width / static_cast<float>(height);
	scene.projection = projection_perspective(50.0f*pi/180.0f, aspect, 0.1f, 100.0f);
}


void mouse_move_callback(GLFWwindow* window, double xpos, double ypos)
{
	vec2 const  p1 = glfw_get_mouse_cursor(window);
	vec2 const& p0 = user.mouse_prev;
	glfw_state state = glfw_current_state(window);

	if(state.key_ctrl)
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
	else
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

	if(! state.key_ctrl){
		scene.camera.manipulator_rotate_trackball(p0, p1);
	}
	user.mouse_prev = p1;
	//glfwSetCursorPos(window, 0, 0);
}

void opengl_uniform(GLuint shader, scene_environment const& current_scene)
{
	opengl_uniform(shader, "projection", current_scene.projection);
	opengl_uniform(shader, "view", current_scene.camera.matrix_view());
	opengl_uniform(shader, "light", current_scene.light, false);
}



