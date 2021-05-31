#pragma once

#include "vcl/vcl.hpp"
#include "variable.hpp"


shadow_map_parameters initialize_depth_map();
void draw_with_shadow(vcl::mesh_drawable const& drawable, scene_environment const& scene);
void draw_depth_map(vcl::mesh_drawable const& drawable, scene_environment const& current_scene);