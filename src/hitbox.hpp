#pragma once

#include "vcl/vcl.hpp"
#include "variable.hpp"

using namespace vcl;

int get_plus_proche(vcl::vec3 pos, std::vector<vcl::vec3> liste, int taille_obj);
int hit_ois();
int hit_cercle();
int hit_cloud();
int on_ile();
int hit_ile(int k);