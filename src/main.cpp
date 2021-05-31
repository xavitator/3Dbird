#define _USE_MATH_DEFINES

#include "vcl/vcl.hpp"
#include <iostream>
#include <random>
#include <functional>
#include <algorithm>
#include <math.h>

#include "bird.hpp"
#include "controller.hpp"
#include "hitbox.hpp"
#include "shadow_map.hpp"
#include "terrain.hpp"
#include "tree.hpp"
#include "variable.hpp"
#include "view.hpp"

using namespace vcl;


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
		if (user.fps_record.event) {
			std::string const title = "VCL Display - " + str(user.fps_record.fps) + " fps";
			glfwSetWindowTitle(window, title.c_str());
		}

		ImGui::Begin("Inteface de jeu",NULL,ImGuiWindowFlags_AlwaysAutoResize);

		display_interface();
		display_scene();

		ImGui::End();
		imgui_render_frame(window);
		
		glfwSwapBuffers(window);
		glfwPollEvents();
		int a = hit_ois();
		if(a >= 0 && a <= 6){
			user.dead = true;
		}
		if(! user.dead){
			update_from_timer();
			if(a == RING){
					int ind = hit_ring();
					Ring *r = ring_objects[ind];
					r->cross();
					ring_objects.erase(ring_objects.begin() + ind);
					delete r;
			}
		}
}

	imgui_cleanup();
	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}