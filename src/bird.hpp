#pragma once

#include "vcl/vcl.hpp"
#include "variable.hpp"

using namespace vcl;

extern hierarchy_mesh_drawable hierarchy_bird;
extern vec3 orientation_bird;

void create_bird();
void move_bird();
vec3 get_pos_bird();

void fall();