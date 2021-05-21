#pragma once

#include "vcl/vcl.hpp"
#include <iostream>
#include <list>
#include "variable.hpp"
#include "bird.hpp"

void init_camera();
void move_camera();
void keyboard_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void initialize_data_view();