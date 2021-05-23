#define _USE_MATH_DEFINES

#include "vcl/vcl.hpp"
#include <iostream>

#include "view.hpp"
#include "terrain.hpp"
#include "tree.hpp"
#include "bird.hpp"
#include "variable.hpp"
#include "hitbox.hpp"


#include <random>
#include <math.h>

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
		int a = hit_ois();
		std::cout << a << std::endl;
	}

	imgui_cleanup();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}



void initialize_data()
{
	//srand (time(NULL));
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
	tree = mesh_drawable(create_tree());
	
	
	
	wall = mesh_drawable(create_wall(taille_terrain));
	image_raw const im3 = image_load_png("assets/rayure1.png");

	// Send this image to the GPU, and get its identifier texture_image_id
	GLuint const texture_image_id3 = opengl_texture_to_gpu(im3,
		GL_MIRRORED_REPEAT /**GL_TEXTURE_WRAP_S*/,
		GL_MIRRORED_REPEAT /**GL_TEXTURE_WRAP_T*/);

	wall.texture = texture_image_id3;
	wall.shading.alpha = 0.5f;

	caillou = mesh_drawable(mesh_load_file_obj("assets/caillou.obj"));
	caillou.shading.color={0.63f,0.63f,0.63f};
	caillou.transform.scale = 100;
	caillou.transform.rotate = rotation({ 1,0,0 }, 0.5f * M_PI);

	cloud1 = mesh_drawable(mesh_load_file_obj("assets/Cloud_1.obj"));
	cloud2 = mesh_drawable(mesh_load_file_obj("assets/Cloud_2.obj"));
	cloud3 = mesh_drawable(mesh_load_file_obj("assets/Cloud_3.obj"));
	cloud4 = mesh_drawable(mesh_load_file_obj("assets/Cloud_4.obj"));

	
	ship = mesh_drawable(mesh_load_file_obj("assets/ship.obj"));
	ship.transform.scale = 0.023f;
	ship.shading.color = { 0.24f,0.20f,0.2f };
	
	ring = mesh_drawable(mesh_load_file_obj("assets/cercle.obj"));
	ring.shading.color = { 1.0f,0,0 };
	ring.transform.scale = 0.5f;


	ocean_m = create_ocean(parameters, taille_terrain);
	ocean = mesh_drawable(ocean_m);
	//ocean.shading.color = { 0.0f, 0.0f, 1.0f };
	image_raw const im4 = image_load_png("assets/ocean.png");
	GLuint const texture_image_id4 = opengl_texture_to_gpu(im4,
		GL_MIRRORED_REPEAT /**GL_TEXTURE_WRAP_S*/,
		GL_MIRRORED_REPEAT /**GL_TEXTURE_WRAP_T*/);

	ocean.texture = texture_image_id4;

	create_bird();
	generate_terrain();

	

	
}


void display_scene()
{	
	wall.transform.rotate = rotation({ 0,0,1 }, 0);
	wall.transform.translate = { 0,0,0 };
	draw(wall, scene);

	wall.transform.translate = { 0.0f,-taille_terrain,0 };
	draw(wall, scene);

	wall.transform.rotate = rotation({ 0,0,1 }, 0.5f * M_PI);
	wall.transform.translate = { 0.0f,0,0 };
	draw(wall, scene);
	wall.transform.translate = { taille_terrain,0,0 };
	draw(wall, scene);
	update_ocean(ocean_m, ocean, parameters);
	for (int k = 0; k < nb_iles; k++) {
		liste_iles[k].transform.translate = ile_position[k];
		//liste_iles[k].transform.rotate = rotation({ 0,0,1 }, ile_orientation[k]);
		draw(liste_iles[k], scene);
		int j = 0;
		for (int i = 0; i < nb_arbres; i++) {
			if (j < 2) {
				tree.transform.translate = liste_tree_position[k][i];
				tree.transform.translate += ile_position[k];
				draw(tree, scene);
				//ocean.transform.translate = { 0,0,10*i };
				//draw(ocean, scene);
				j++;
			}
			else
			{
				j = 0;
				caillou.transform.translate = liste_tree_position[k][i];
				caillou.transform.translate += ile_position[k] - vec3({0, 0, 0.1f});
				draw(caillou, scene);
			}
		}
	}
	for (int k = 0; k< nb_ship; k++) {
		ship.transform.translate = ship_position[k];
		ship.transform.translate += {0, 0, ocean_height(ship_position[k][0]+taille_terrain/2, ship_position[k][1] + taille_terrain / 2, taille_terrain, parameters, 0.2f) * 0.8f};
		ship.transform.rotate = rotation({ { cos(ship_orientation[k]),0,sin(ship_orientation[k]) }, { sin(ship_orientation[k]),0,-cos(ship_orientation[k]) },{0,1,0} });
		//ship.transform.rotate = rotation({ 1,0,0 }, 0.5f * M_PI);
		draw(ship, scene);
	}
	cloud1.transform.translate = { 0,0,0 };
	draw(cloud1, scene);
	int j = 3;
	for (int i = 0; i < cloud_position.size(); i++) {
		cloud_position[i] = cloud_deplacement(cloud_position[i], taille_terrain);
		if (j == 0) {
			
			cloud1.transform.translate = cloud_position[i];
			cloud1.transform.rotate = rotation({ 0,0,1 }, cloud_orientation[i]);
			draw(cloud1, scene);
			j++;
		}
		else if (j == 1) {
			cloud2.transform.translate = cloud_position[i];
			cloud2.transform.rotate = rotation({ 0,0,1 }, cloud_orientation[i]);
			draw(cloud2, scene);
			j++;
		}
		else if (j == 2) {
			cloud3.transform.translate = cloud_position[i];
			cloud3.transform.rotate = rotation({ 0,0,1 }, cloud_orientation[i]);
			draw(cloud3, scene);
			j++;
		}
		else if (j == 3) {
			cloud4.transform.translate = cloud_position[i];
			cloud4.transform.rotate = rotation({ 0,0,1 }, cloud_orientation[i]);
			draw(cloud4, scene);
			//j=0;
		}
	}
	for (int i = 0; i < ring_position.size(); i++) {
		ring.transform.translate = ring_position[i];
		ring.transform.rotate = rotation({ 0,0,1 }, ring_orientation[i]);
		draw(ring, scene);
	}
	ocean.transform.translate = { 0,0,0 };
	draw(ocean, scene);	

	
	

	move_bird();
	move_camera_center();
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
	vec2 const  p1 = vec2(xpos, ypos);
	vec2 const& p0 = user.mouse_prev;
	glfw_state state = glfw_current_state(window);

	if(state.key_ctrl){
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
		// auto& camera = scene.camera;
		// vec2 const  p2 = glfw_get_mouse_cursor(window, xpos, ypos);
		// if(state.mouse_click_left && !state.key_shift)
		// 	scene.camera.manipulator_rotate_trackball(p0, p2);
		// if(state.mouse_click_left && state.key_shift)
		// 	camera.manipulator_translate_in_plane(p2-p0);
		// if(state.mouse_click_right)
		// 	camera.manipulator_scale_distance_to_center( (p2-p0).y );
	}
	else
		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if(! state.key_ctrl){
		move_camera_rotation(p0, p1);
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


