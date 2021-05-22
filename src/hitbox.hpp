#pragma once

#include "vcl/vcl.hpp"
#include "variable.hpp"

using namespace vcl;

int hit_oiseau();
int get_plus_proche(vcl::vec3 pos, std::vector<vcl::vec3> liste, int taille_obj);
int hit_ois();