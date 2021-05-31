#pragma once

#include "vcl/vcl.hpp"
#include "variable.hpp"

#include <iostream>

#include "view.hpp"
#include "terrain.hpp"
#include "tree.hpp"
#include "bird.hpp"
#include "variable.hpp"
#include "hitbox.hpp"
#include "view.hpp"
#include "shadow_map.hpp"


#include <random>
#include <functional>
#include <algorithm>
#include <math.h>

using namespace vcl;

void opengl_uniform(GLuint shader, scene_environment const& current_scene);

void initialize_data();
void display_scene();
void display_interface();
void restart_game();
void update_from_timer();