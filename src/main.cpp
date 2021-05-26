#define _USE_MATH_DEFINES

#include "vcl/vcl.hpp"
#include <iostream>

#include "view.hpp"
#include "terrain.hpp"
#include "tree.hpp"
#include "bird.hpp"
#include "variable.hpp"
#include "hitbox.hpp"
#include "shadow_map.hpp"


#include <random>
#include <functional>
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
		// scene.light = scene.camera.position();
		user.fps_record.update();
		
		glClearColor(189 / 255.0f, 217 / 255.0f, 242 / 255.0f, 0.95f);
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
		// std::cout << a << std::endl;
	}

	imgui_cleanup();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}



void initialize_data()
{
	//srand (time(NULL));
	GLuint const shader_mesh = opengl_create_shader_program(read_text_file("shader/mesh_with_shadow.vert.glsl"), read_text_file("shader/mesh_with_shadow.frag.glsl"));
	GLuint const shader_uniform_color = opengl_create_shader_program(opengl_shader_preset("single_color_vertex"), opengl_shader_preset("single_color_fragment"));
	GLuint const texture_white = opengl_texture_to_gpu(image_raw{1,1,image_color_type::rgba,{255,255,255,255}});
	mesh_drawable::default_shader = shader_mesh;
	mesh_drawable::default_texture = texture_white;
	curve_drawable::default_shader = shader_uniform_color;
	segments_drawable::default_shader = shader_uniform_color;
	
	// Initialize the light position and viewpoint
	scene.projection = projection_orthographic(-10,10,-10,10,0,30); // orthographic projection for simplicity
	scene.light.distance_to_center = 20.0f;
	scene.light.manipulator_rotate_spherical_coordinates(pi/4.0f, pi/4.0f);

	user.global_frame = mesh_drawable(mesh_primitive_frame());
	user.gui.display_frame = false;
	init_camera();

	// Initialize the FBO and texture used to handle the depth map
	scene.depth_map = initialize_depth_map();

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
	ocean.shading.phong.specular= 10;

	create_bird();
	generate_terrain();

	

	
}

using draw_func = std::function<void(mesh_drawable const& drawable, scene_environment const& current_scene)>;

void display_scene()
{	
	update_ocean(ocean_m, ocean, parameters);

	std::function<void(draw_func)> draw_all = [](draw_func draw_element) -> void {
		for (int k = 0; k < nb_iles; k++) {
			liste_iles[k].transform.translate = ile_position[k];
			//liste_iles[k].transform.rotate = rotation({ 0,0,1 }, ile_orientation[k]);
			draw_element(liste_iles[k], scene);
			int j = 0;
			for (int i = 0; i < nb_arbres; i++) {
				if (j < 2) {
					tree.transform.translate = liste_tree_position[k][i];
					tree.transform.translate += ile_position[k];
					draw_element(tree, scene);
					//ocean.transform.translate = { 0,0,10*i };
					//draw_element(ocean, scene);
					j++;
				}
				else
				{
					j = 0;
					caillou.transform.translate = liste_tree_position[k][i];
					caillou.transform.translate += ile_position[k] - vec3({0, 0, 0.1f});
					draw_element(caillou, scene);
				}
			}
		}
		for (int k = 0; k< nb_ship; k++) {
			ship.transform.translate = ship_position[k];
			ship.transform.translate += {0, 0, ocean_height(ship_position[k][0]+taille_terrain/2, ship_position[k][1] + taille_terrain / 2, taille_terrain, parameters, v_maree) * 0.8f};
			ship.transform.rotate = rotation({ { cos(ship_orientation[k]),0,sin(ship_orientation[k]) }, { sin(ship_orientation[k]),0,-cos(ship_orientation[k]) },{0,1,0} });
			//ship.transform.rotate = rotation({ 1,0,0 }, 0.5f * M_PI);
			draw_element(ship, scene);
		}
		for (int i = 0; i < ring_position.size(); i++) {
			std::cout << ring_position[i] << std::endl;
			ring.transform.translate = ring_position[i];
			ring.transform.rotate = rotation({ 0,0,1 }, ring_orientation[i]);
			draw_element(ring, scene);
		}

		cloud1.transform.translate = { 0,0,0 };
		draw_element(cloud1, scene);
		int j = 3;
		for (int i = 0; i < cloud_position.size(); i++) {
			cloud_position[i] = cloud_deplacement(cloud_position[i], taille_terrain);
			if (j == 0) {
				
				cloud1.transform.translate = cloud_position[i];
				cloud1.transform.rotate = rotation({ 0,0,1 }, cloud_orientation[i]);
				draw_element(cloud1, scene);
				j++;
			}
			else if (j == 1) {
				cloud2.transform.translate = cloud_position[i];
				cloud2.transform.rotate = rotation({ 0,0,1 }, cloud_orientation[i]);
				draw_element(cloud2, scene);
				j++;
			}
			else if (j == 2) {
				cloud3.transform.translate = cloud_position[i];
				cloud3.transform.rotate = rotation({ 0,0,1 }, cloud_orientation[i]);
				draw_element(cloud3, scene);
				j++;
			}
			else if (j == 3) {
				cloud4.transform.translate = cloud_position[i];
				cloud4.transform.rotate = rotation({ 0,0,1 }, cloud_orientation[i]);
				draw_element(cloud4, scene);
				//j=0;
			}
		}
		ocean.transform.translate = { 0,0,0 };
		draw_element(ocean, scene);	
	};

	// First pass: Draw all shapes that cast shadows
	{
		glViewport(0, 0, scene.depth_map.width, scene.depth_map.height);
		glBindFramebuffer(GL_FRAMEBUFFER, scene.depth_map.fbo); opengl_check;
		glClear(GL_DEPTH_BUFFER_BIT);

		draw_all(draw_depth_map);

		glBindFramebuffer(GL_FRAMEBUFFER, 0); opengl_check;
		
	}

	// Second pass: Draw all shapes that receives shadows
	{
		glViewport(0, 0, width, height);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		draw_all(draw_with_shadow);

	}
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
	opengl_uniform(shader, "light", current_scene.light.matrix_view());
	opengl_uniform(shader, "projection", current_scene.projection);
}


