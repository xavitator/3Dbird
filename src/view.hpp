#pragma once

#include "vcl/vcl.hpp"
#include <iostream>
#include <list>
#include "variable.hpp"
#include "bird.hpp"

void init_camera();
void move_camera_center();
void move_camera_rotation(vec2 p0, vec2 p1);

/*******
Intéraction avec le joueur
********/
void mouse_move_callback(GLFWwindow* window, double xpos, double ypos);
void window_size_callback(GLFWwindow* window, int width, int height);
void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);