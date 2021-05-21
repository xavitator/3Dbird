#define _USE_MATH_DEFINES

#include "vcl/vcl.hpp"
#include <iostream>
#include <random>
#include "terrain.hpp"
#include "tree.hpp"
#include <math.h>

using namespace vcl;


struct gui_parameters {
	bool display_frame = true;
	bool add_sphere = true;
};

struct user_interaction_parameters {
	vec2 mouse_prev;
	timer_fps fps_record;
	mesh_drawable global_frame;
	gui_parameters gui;
	bool cursor_on_gui;
};
user_interaction_parameters user;

struct scene_environment
{
	camera_around_center camera;
	mat4 projection;
	vec3 light;
};
scene_environment scene;


void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
void window_size_callback(GLFWwindow* window, int width, int height);

void initialize_data();
void display_scene();
void display_interface();

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


std::vector<vcl::vec3> tree_position;
perlin_noise_parameters parameters;
mesh_drawable billboard_grass;
mesh_drawable billboard_bird;
std::vector<mesh_drawable> liste_iles;
std::vector<vcl::vec3> ile_position;
std::vector<std::vector<vcl::vec3>> liste_tree_position;
std::vector<float> ile_orientation;
std::vector<vcl::vec3> cloud_position;
std::vector<float> cloud_orientation;
std::vector<vcl::vec3> ship_position;
std::vector<float> ship_orientation;

std::vector<vcl::vec3> ring_position;
std::vector<float> ring_orientation;

std::vector<perlin_noise_parameters> liste_noise_ile;

int taille_terrain = 100;
int nb_iles = 6;
int nb_arbres = 20;
int nb_cloud = 40;
int nb_ship = 20;
int nb_ring = 20; 


int main(int, char* argv[])
{
	std::cout << "Run " << argv[0] << std::endl;

	int const width = 1280, height = 1024;
	GLFWwindow* window = create_window(width, height);
	window_size_callback(window, width, height);
	std::cout << opengl_info_display() << std::endl;;

	imgui_init(window);
	glfwSetCursorPosCallback(window, mouse_move_callback);
	glfwSetWindowSizeCallback(window, window_size_callback);
	
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
		user.cursor_on_gui = ImGui::IsAnyWindowFocused();

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

mesh_drawable ile(perlin_noise_parameters par2) {
	terrain = create_terrain(par2);
	mesh_drawable terrain_visual1 = mesh_drawable(terrain);
	//terrain_visual1.shading.color = { 0.6f,0.85f,0.5f };
	terrain_visual1.shading.phong.specular = 0.0f; // non-specular terrain material

	image_raw const im2 = image_load_png("assets/sandgrass.png");

	// Send this image to the GPU, and get its identifier texture_image_id
	GLuint const texture_image_id2 = opengl_texture_to_gpu(im2,
		GL_MIRRORED_REPEAT /**GL_TEXTURE_WRAP_S*/,
		GL_MIRRORED_REPEAT /**GL_TEXTURE_WRAP_T*/);

	terrain_visual1.texture = texture_image_id2;

	return terrain_visual1;
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
	scene.camera.distance_to_center = 2.5f;
	scene.camera.look_at({4,3,2}, {0,0,0}, {0,0,1});

    // Create visual terrain surface
	tree = mesh_drawable(create_tree());
	
	
	ile_position = generate_positions_ile(nb_iles,taille_terrain);
	ile_orientation = generate_rotation(nb_iles);
	for (int k = 0; k < nb_iles; k++) {
		perlin_noise_parameters par2= generate_alea_ile();
		liste_iles.push_back(ile(par2));
		liste_tree_position.push_back(generate_positions_on_terrain(nb_arbres, par2));
		liste_noise_ile.push_back(par2);
	}
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

	cloud_position = generate_positions_clouds(nb_cloud, taille_terrain);
	cloud_orientation = generate_rotation(nb_cloud);
	
	ship = mesh_drawable(mesh_load_file_obj("assets/ship.obj"));
	ship.transform.scale = 0.023f;
	ship.shading.color = { 0.24f,0.20f,0.2f };
	
	ring = mesh_drawable(mesh_load_file_obj("assets/cercle.obj"));
	ring.shading.color = { 1.0f,0,0 };
	ring.transform.scale = 0.5f;

	ring_orientation = generate_rotation(nb_ring);
	ring_position = generate_positions_ring(nb_ring, taille_terrain);

	ship_position = generate_positions_ships(nb_ship, taille_terrain, ile_position);
	ship_orientation = generate_rotation(nb_ship);

	ocean_m = create_ocean(parameters, taille_terrain);
	ocean = mesh_drawable(ocean_m);
	//ocean.shading.color = { 0.0f, 0.0f, 1.0f };
	image_raw const im4 = image_load_png("assets/ocean.png");
	GLuint const texture_image_id4 = opengl_texture_to_gpu(im4,
		GL_MIRRORED_REPEAT /**GL_TEXTURE_WRAP_S*/,
		GL_MIRRORED_REPEAT /**GL_TEXTURE_WRAP_T*/);

	ocean.texture = texture_image_id4;

}


void display_scene()
{	
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
	int j = 0;
	for (int i = 0; i < cloud_position.size(); i++) {
		if (j == 0) {
			cloud1.transform.translate = cloud_deplacement(cloud_position[i],taille_terrain);
			cloud1.transform.rotate = rotation({ 0,0,1 }, cloud_orientation[i]);
			draw(cloud1, scene);
			j++;
		}
		else if (j == 1) {
			cloud2.transform.translate = cloud_deplacement(cloud_position[i], taille_terrain);
			cloud2.transform.rotate = rotation({ 0,0,1 }, cloud_orientation[i]);
			draw(cloud2, scene);
			j++;
		}
		else if (j == 2) {
			cloud3.transform.translate = cloud_deplacement(cloud_position[i], taille_terrain);
			cloud3.transform.rotate = rotation({ 0,0,1 }, cloud_orientation[i]);
			draw(cloud3, scene);
			j++;
		}
		else if (j == 3) {
			cloud4.transform.translate = cloud_deplacement(cloud_position[i], taille_terrain);
			cloud4.transform.rotate = rotation({ 0,0,1 }, cloud_orientation[i]);
			draw(cloud4, scene);
			j=0;
		}
	}
	for (int i = 0; i < ring_position.size(); i++) {
		ring.transform.translate = ring_position[i];
		ring.transform.rotate = rotation({ 0,0,1 }, ring_orientation[i]);
		draw(ring, scene);
	}
	ocean.transform.translate = { 0,0,0 };
	draw(ocean, scene);	

	wall.transform.rotate = rotation({ 0,0,1 },0);
	wall.transform.translate = { 0,0,0 };
	draw(wall, scene);

	wall.transform.translate = { 0.0f,-taille_terrain,0 };
	draw(wall, scene);

	wall.transform.rotate = rotation({ 0,0,1 }, 0.5f* M_PI);
	wall.transform.translate = { 0.0f,0,0 };
	draw(wall, scene);
	wall.transform.translate = { taille_terrain,0,0 };
	draw(wall, scene);
	
	draw(ring, scene);

	
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
	vec2 const  p1 = glfw_get_mouse_cursor(window, xpos, ypos);
	vec2 const& p0 = user.mouse_prev;
	glfw_state state = glfw_current_state(window);

	auto& camera = scene.camera;
	if(!user.cursor_on_gui){
		if(state.mouse_click_left && !state.key_ctrl)
			scene.camera.manipulator_rotate_trackball(p0, p1);
		if(state.mouse_click_left && state.key_ctrl)
			camera.manipulator_translate_in_plane(p1-p0);
		if(state.mouse_click_right)
			camera.manipulator_scale_distance_to_center( (p1-p0).y );
	}

	user.mouse_prev = p1;
}

void opengl_uniform(GLuint shader, scene_environment const& current_scene)
{
	opengl_uniform(shader, "projection", current_scene.projection);
	opengl_uniform(shader, "view", current_scene.camera.matrix_view());
	opengl_uniform(shader, "light", current_scene.light, false);
}



